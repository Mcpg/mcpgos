#include <McpgOS.h>

uint32_t CurrentPageDirectoryPhys = 0;
PageDirectoryEntry* CurrentPageDirectory = NULL;

void SwitchPageDirectory(PageDirectoryEntry* pd)
{
    CurrentPageDirectoryPhys = MmVirtToPhys(CurrentPageDirectory, (uintptr_t) pd);
    CurrentPageDirectory = pd;
    asm("mov %%eax, %%cr3" : : "a" (CurrentPageDirectoryPhys));
}

PageDirectoryEntry* MmAllocPageDirectory()
{
    int i;
    PageDirectoryEntry* current;

    PageDirectoryEntry* result =
        (PageDirectoryEntry*) malloc(sizeof(PageDirectoryEntry) * 1024);
    if (result == NULL)
        return NULL;

    // Map the kernel space
    for (i = 0; i < 128; i++)
    {
        current = &result[896 + i];
        memset(current, 0, sizeof(PageDirectoryEntry));
        current->Raw = K_VIRT_TO_PHYS(&KernelPageTables[i * 1024]);
        current->Present = 1;
        current->Writable = 1;
        current->User = 0;
    }

    // Identity map the first 4 MiB
    current = &result[0];
    memset(current, 0, sizeof(PageDirectoryEntry));
    current->Raw = K_VIRT_TO_PHYS(&ZeroIdentityPage[0]);
    current->Present = 1;
    current->Writable = 1;
    current->User = 0;

    return result;
}

PageTableEntry* MmGetTable(PageDirectoryEntry* pde, int index)
{
    KAssert(index >= 0 && index <= 1023);
    
    if (pde[index].Present == 0)
        return NULL;
    return (PageTableEntry*) MmKernelPhysToVirt(pde[index].Raw & 0xFFFFF000);
}

PageTableEntry* MmGetTableAddr(PageDirectoryEntry* pde, uintptr_t virt)
{
    return MmGetTable(pde, virt >> 22);
}

PageTableEntry* MmGetTableEntry(PageDirectoryEntry* pde, uintptr_t virt)
{
    PageTableEntry* pte = MmGetTableAddr(pde, virt);
    return pte ? &pte[(virt >> 12) & 1023] : NULL;
}

uint32_t MmVirtToPhys(PageDirectoryEntry* pde, uintptr_t virt)
{
    PageTableEntry* pte;

    KAssert(pde != NULL);

    pte = MmGetTableEntry(pde, virt);
    if (pte == NULL)
        return 0;

    return (pte->PhysAddress << 12) + (virt & 0xFFF);
}

void* MmKernelPhysToVirt(uint32_t phys)
{
    int i;

    for (i = 0; i < 131072; i++)
    {
        if ((KernelPageTables[i].Raw & ~0xFFF) == (phys & ~0xFFF))
            return (void*) (KernelPageTables[i].Raw & ~0xFFF) + (phys & 0xFFF);
    }

    return NULL;
}

void* MmMmap(void* ptr, uint32_t pages, bool user, bool writable)
{
    const uint32_t startingPage = 1024;
    const uint32_t endingPage = 917503;

    // TODO: mmap does not work

    bool blockBigEnough = false;
    bool foundStartingPoint;
    uint32_t ptrPage;
    uint32_t foundPageBase;
    uint32_t pagesFound = 0;
    uint32_t i;

    KAssert(pages > 0);

    if (ptr != NULL)
    {
        KAssert(((uintptr_t) ptr & 0xFFF) == 0);
        
        ptrPage = ((uintptr_t) ptr) >> 12;
        KAssert(ptrPage >= startingPage && ptrPage <= endingPage);
        KAssert((ptrPage + pages) <= endingPage);
    }

    if (ptr == NULL)
    {
        // Find a memory block big enough

        foundStartingPoint = false;

        for (ptrPage = startingPage; ptrPage <= endingPage; ptrPage++)
        {
            if (MmIsPresent(ptrPage << 12))
            {
                pagesFound = 0;
                foundStartingPoint = false;
                foundPageBase = 0;
            }
            else
            {
                if (!foundStartingPoint)
                    foundPageBase = ptrPage;
                foundStartingPoint = true;
                pagesFound++;
            }

            if (foundStartingPoint && pagesFound == pages)
                break;
        }

        if (!foundStartingPoint)
            return NULL;

        ptr = (void*) (foundPageBase << 12);
    }
    else
    {
        // Verify whether there's enough space for the mapping
        
        blockBigEnough = true;
        for (i = ptrPage; ptrPage < (ptrPage + pages); i++)
        {
            if (MmIsPresent(i << 12))
            {
                blockBigEnough = false;
                break;
            }
        }

        if (!blockBigEnough)
            return NULL;
    }

    for (i = ptrPage; ptrPage < (ptrPage + pages); i++)
    {
        MmMap(
            MmPmAllocate(), (uintptr_t) ptr,
            user, writable
        );
    }

    return ptr;
}

bool MmIsPresent(uintptr_t virt)
{
    PageTableEntry* pte = MmGetTableEntry(CurrentPageDirectory, virt);

    if (!pte)
        return false;

    return pte->Present;
}

bool MmIsWritable(uintptr_t virt)
{
    PageTableEntry* pte = MmGetTableEntry(CurrentPageDirectory, virt);

    if (!pte)
        return false;

    return pte->Writable;
}

bool MmIsUser(uintptr_t virt)
{
    PageTableEntry* pte = MmGetTableEntry(CurrentPageDirectory, virt);

    if (!pte)
        return false;

    return pte->User;
}

bool MmMap(uint32_t phys, uintptr_t virt, bool user, bool writable)
{
    PageDirectoryEntry* pde = CurrentPageDirectory;
    PageTableEntry* pte = MmGetTableEntry(pde, virt);

    if (!pte)
    {
        // TODO: move page table allocation to a separate function
        pde[virt >> 22].Raw
            = MmVirtToPhys(
                CurrentPageDirectory,
                malloc(sizeof(PageTableEntry) * 1024)
            ) & ~0xFFF;
        pde[virt >> 22].User = user;
        pde[virt >> 22].Writable = writable;
        pde[virt >> 22].Present = true;

        pte = MmGetTableEntry(pde, virt);
    }

    pte->PhysAddress = phys >> 12;
    pte->Present = 1;
    pte->User = user;
    pte->Writable = writable;

    return true;
}

bool MmUnmap(uintptr_t virt)
{
    PageDirectoryEntry* pde = CurrentPageDirectory;
    PageTableEntry* pte = MmGetTableEntry(pde, virt);

    if (!pte)
        return false;

    pte->PhysAddress = 0;
    pte->Present = 0;
    pte->User = 0;
    pte->Writable = 0;

    return true;
}

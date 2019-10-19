#include <McpgOS.h>

uint32_t CurrentPageDirectory = 0;

void SwitchPageDirectory(uint32_t newDir)
{
    CurrentPageDirectory = newDir;
    asm("mov %%eax, %%cr3" : : "a" (newDir));
}

PageDirectoryEntry* GetCurrentPageDirectory()
{
    return (PageDirectoryEntry*) K_PHYS_TO_VIRT(CurrentPageDirectory);
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
        current = &result[768 + i];
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
    return (PageTableEntry*) K_PHYS_TO_VIRT(pde[index].Raw & 0xFFFFF000);
}

PageTableEntry* MmGetTableAddr(PageDirectoryEntry* pde, uintptr_t virt)
{
    return MmGetTable(pde, virt >> 22);
}

PageTableEntry* MmGetTableEntry(PageDirectoryEntry* pde, uintptr_t virt)
{
    PageTableEntry* pte = MmGetTableAddr(pde, virt);
    return pte ? &pte[virt & 1023] : NULL;
}

uint32_t MmVirtToPhys(PageDirectoryEntry* pde, uintptr_t virt)
{
    PageTableEntry* pte;

    KAssert(pde != NULL);

    pte = MmGetTableEntry(pde, virt);
    if (pte == NULL)
        return 0;

    return (pte->Raw & (~0xFFF)) + (virt & 0xFFF);
}

bool MmIsPresent(uintptr_t virt)
{
    PageTableEntry* pte = MmGetTableEntry(GetCurrentPageDirectory(), virt);

    if (!pte)
        return false;

    return pte->Present;
}

bool MmIsWritable(uintptr_t virt)
{
    PageTableEntry* pte = MmGetTableEntry(GetCurrentPageDirectory(), virt);

    if (!pte)
        return false;

    return pte->Writable;
}

bool MmIsUser(uintptr_t virt)
{
    PageTableEntry* pte = MmGetTableEntry(GetCurrentPageDirectory(), virt);

    if (!pte)
        return false;

    return pte->User;
}

bool MmMap(uint32_t phys, uintptr_t virt, bool user, bool writable)
{
    PageDirectoryEntry* pde = GetCurrentPageDirectory();
    PageTableEntry* pte = MmGetTableEntry(pde, virt);

    if (!pte)
        return false;

    pte->PhysAddress = phys >> 12;
    pte->Present = 1;
    pte->User = user;
    pte->Writable = writable;

    return true;
}

bool MmUnmap(uintptr_t virt)
{
    PageDirectoryEntry* pde = GetCurrentPageDirectory();
    PageTableEntry* pte = MmGetTableEntry(pde, virt);

    if (!pte)
        return false;

    pte->PhysAddress = 0;
    pte->Present = 0;
    pte->User = 0;
    pte->Writable = 0;

    return true;
}

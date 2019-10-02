#include <McpgOS.h>

uint32_t CurrentPageDirectory;

void SwitchPageDirectory(uint32_t newDir)
{
    CurrentPageDirectory = newDir;
    asm("mov %%eax, %%cr3" : : "a" (newDir));
}

PageDirectoryEntry* GetCurrentPageDirectory()
{
    return (PageDirectoryEntry*) K_PHYS_TO_VIRT(CurrentPageDirectory);
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

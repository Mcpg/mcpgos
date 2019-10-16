#include <McpgOS.h>

// Kernel virtual memory allocator

void MmVmInitAllocator()
{
}



// TODO: Create a better allocator
uintptr_t MmVmAllocate(uint32_t pages)
{
    uint32_t i;
    uint32_t startingPage = 0;
    uint32_t pageCounter = 0;
    bool foundPages = false;

    for (i = 0; i < 131072; i++)
    {
        if (KernelPageTables[i].Present)
        {
            startingPage = 0;
            pageCounter = 0;
        }
        else
        {
            if (startingPage == 0)
            {
                startingPage = i;
                pageCounter = 1;
            }
            else
            {
                pageCounter++;
            }
        }

        if (startingPage != 0 && pageCounter == pages)
        {
            foundPages = true;
            break;
        }
    }

    if (!foundPages)
        return 0;

    for (i = startingPage; i < (startingPage + pages); i++)
    {
        KernelPageTables[i].Present = 1;
        KernelPageTables[i].User = 0;
        KernelPageTables[i].Writable = 1;
        KernelPageTables[i].PhysAddress = MmPmAllocate() >> 12;
    }

    return (uintptr_t) (&VirtualAddress) + (startingPage * 4096);
}

void MmVmFree(uintptr_t virt, uint32_t pages)
{
    int i;
    PageTableEntry* startingPte
        = &KernelPageTables[((virt & ~0xFFF) - VirtualAddress) / 4096];

    for (i = 0; i < pages; i++)
    {
        startingPte->Present = 0;
        startingPte->PhysAddress = 0;
        startingPte++;
    }
}

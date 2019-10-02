#include <McpgOS.h>

static bool LibAllocLock = false; 

int liballoc_lock()
{
    while (LibAllocLock) asm("hlt");
    LibAllocLock = true;
    return 0;
}

int liballoc_unlock()
{
    LibAllocLock = false;
    return 0;
}

void* liballoc_alloc(size_t amount)
{
    return (void*) MmVmAllocate(amount);
}

int liballoc_free(void* pageBase, size_t amount)
{
    MmVmFree((uintptr_t) pageBase, amount);
}

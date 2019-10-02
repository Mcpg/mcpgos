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

void* liballoc_alloc(int amount)
{
    return NULL; // todo you idiot
}

int liballoc_free(void* pageBase, int amount)
{
    // todo you idiot
}

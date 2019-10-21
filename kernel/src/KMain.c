#include <McpgOS.h>
#include <stdint.h>

void* KernelStart = &_KernelStart;
void* KernelEnd = &_KernelEnd;
uint32_t KernelSize = (uint32_t) &_KernelSize;
uint32_t KernelSizePages = (uint32_t) &_KernelSizePages;
uintptr_t VirtualAddress = (uintptr_t) &_VirtualAddress;
uintptr_t PhysicalAddress = (uintptr_t) &_PhysicalAddress;

void TestFunction();

void KMain()
{
    /* Initialize the basics */
    GdtInit();
    IdtInit();
    PicInit();
    PitInit();
    
    /* Initialize the memory manager */
    MmPmInitAllocator();
    MmVmInitAllocator();

    /* Initialize the drivers */
    DmInit();
    TTYInit();
    RamDiskInit();

    /* Initialize other kernel components */
    SchedInit();
    SyscallInit();
    VfsInit();

    DevfsInit();

    STI();
    while(1)
    {
        asm("hlt");
    }
}

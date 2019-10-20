#include <McpgOS.h>
#include <stdint.h>

void* KernelStart = &_KernelStart;
void* KernelEnd = &_KernelEnd;
uint32_t KernelSize = (uint32_t) &_KernelSize;
uint32_t KernelSizePages = (uint32_t) &_KernelSizePages;
uintptr_t VirtualAddress = (uintptr_t) &_VirtualAddress;
uintptr_t PhysicalAddress = (uintptr_t) &_PhysicalAddress;

void TestFunction()
{
    //asm("int $0x83" : : "a"(0x00), "b"(0x05));
    while(1) HALT();
}

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

    liballoc_dump();

    Process* proc = ProcessCreate(false, 0);
    KAssert(proc != NULL);
    SwitchPageDirectory(proc->PageDirectory);
    KAssert(MmMap(MmPmAllocate(), 0xA0000000, true, false));
    memcpy((void*) 0xA0000000, TestFunction, 1024);

    KAssert(
        SchedCreateUserTask(proc, 0xA0000000, 0xA0000000 + 1024) != NULL
    );

    //*((uint32_t*)0xB8000)=0x0733073A; /* :3 */
    STI();
    while(1)
    {
        asm("hlt");
    }
}

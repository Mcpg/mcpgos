#include <McpgOS.h>
#include <stdint.h>

uint32_t CurrentPageDirectory = 0;

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

    /* Initialize other kernel components */
    SchedInit();

    liballoc_dump();

    //*((uint32_t*)0xB8000)=0x0733073A; /* :3 */
    asm("sti");
    while(1)
    {
        asm("hlt");
    }
}

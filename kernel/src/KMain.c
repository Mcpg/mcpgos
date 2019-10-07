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

    /* Initialize other kernel components */
    SchedInit();

    *((uint32_t*)0xB8000)=0x0733073A; /* :3 */
    while(1)
    {
        asm volatile("hlt");
    }
}

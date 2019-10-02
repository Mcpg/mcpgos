#include <McpgOS.h>
#include <stdint.h>

uint32_t CurrentPageDirectory = 0;

void KMain()
{
    GdtInit();
    IdtInit();
    PicInit();
    
    MmPmInitAllocator();
    MmVmInitAllocator();

    *((uint32_t*)0xB8000)=0x0733073A; /* :3 */
    while(1)
    {
        asm volatile("hlt");
    }
}

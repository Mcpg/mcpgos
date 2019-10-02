#include <McpgOS.h>
#include <stdint.h>

uint32_t CurrentPageDirectory = 0;

void KMain()
{
    void* ptr1;
    void* ptr2;
    void* ptr3;

    GdtInit();
    IdtInit();
    PicInit();
    
    MmPmInitAllocator();
    MmVmInitAllocator();

    ptr1 = malloc(10);
    ptr2 = malloc(4095);
    ptr3 = malloc(4097);

    *((uint32_t*)0xB8000)=0x0733073A; /* :3 */
    while(1)
    {
        asm volatile("hlt");
    }
}

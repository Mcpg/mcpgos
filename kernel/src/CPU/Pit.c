#include <McpgOS.h>

#define PIT_MCR 0x43
#define PIT_C0  0x40

void PitInit()
{
    PitReconfigureC0(1);
}

void PitReconfigureC0(uint16_t div)
{
    // Start reconfiguring the channel 0, in 16-bit binary mode...
    Outb(PIT_MCR, 0x30);
    Outb(PIT_C0, div & 0xFF);
    Outb(PIT_C0, (div >> 8) & 0xFF);
}

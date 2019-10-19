#include <McpgOS.h>

static const uint8_t PitMcr = 0x43;
static const uint8_t PitChannel0 = 0x40;

void PitInit()
{
    // 1.193182 MHz / 1000 Hz = divider
    // The PIT is effectively running at a speed of 1 IRQ
    // per millisecond
    PitReconfigureC0(0x1234DE / 1000);
}

void PitReconfigureC0(uint16_t div)
{
    Outb(PitMcr, 0x34);
    Outb(PitChannel0, div & 0xFF);
    Outb(PitChannel0, (div >> 8) & 0xFF);
}

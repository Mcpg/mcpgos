#include <McpgOS.h>

typedef enum
{
    Pic1Command = 0x20,
    Pic1Data    = 0x21,
    Pic2Command = 0xA0,
    Pic2Data    = 0xA1
} PicIoPort;

const uint8_t PicIrqOffset = 0x20;

void PicInit()
{
    /* Start initialization in cascade mode */
    Outb(Pic1Command, 0x11);
    Outb(Pic2Command, 0x11);
    
    /* Specify the IRQ offsets */
    Outb(Pic1Data, PicIrqOffset);
    Outb(Pic2Data, PicIrqOffset + 7);
    
    /* Notify PIC1 about existance of a PIC2 and configure it */
    Outb(Pic1Data, 0x04);
    Outb(Pic2Data, 0x02);
    
    /* 8086 mode */
    Outb(Pic1Data, 0x01);
    Outb(Pic2Data, 0x01);
    
    /* Set the IRQ masks to not ignore any IRQs */
    Outb(Pic1Data, 0x00);
    Outb(Pic2Data, 0x00);
}

void PicEOI(uint8_t irq)
{
    if (irq > 7)
        Outb(Pic2Command, 0x20);
    Outb(Pic1Command, 0x20);
}

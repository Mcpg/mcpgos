#include <McpgOS.h>
#include <CPU/IO.h>

#define PIC1_CMD  0x20
#define PIC1_DATA 0x21
#define PIC2_CMD  0xA0
#define PIC2_DATA 0xA1

void PicInit()
{
    /* Start initialization in cascade mode */
    Outb(PIC1_CMD, 0x11);
    Outb(PIC2_CMD, 0x11);
    
    /* Specify the IRQ offsets */
    Outb(PIC1_DATA, IRQ_OFFSET);
    Outb(PIC2_DATA, IRQ_OFFSET + 7);
    
    /* Notify PIC1 about existance of a PIC2 and configure it */
    Outb(PIC1_DATA, 0x04);
    Outb(PIC2_DATA, 0x02);
    
    /* 8086 mode */
    Outb(PIC1_DATA, 0x01);
    Outb(PIC2_DATA, 0x01);
    
    /* Set the IRQ masks to not ignore any IRQs */
    Outb(PIC1_DATA, 0x00);
    Outb(PIC2_DATA, 0x00);
}

void PicEOI(uint8_t irq)
{
    if (irq > 7)
    {
        Outb(PIC2_CMD, 0x20);
    }
    Outb(PIC1_CMD, 0x20);
}

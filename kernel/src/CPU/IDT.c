#include <McpgOS.h>

uint64_t IdtTable[256];

IdtFrame* HLIntHandler(IdtFrame* idtFrame)
{
    if (idtFrame->InterruptNumber >= 0x20 &&
        idtFrame->InterruptNumber <= 0x2F)
    {
        PicEOI(idtFrame->InterruptNumber - 0x20);
    }
    return idtFrame;
}

void IdtInit()
{
    for (int i = 0; i < 256; i++)
    {
        IdtSetEntry(i, (void*) _IntDefault, IDT_GATE_INT32, 3);
    }
    
    IdtSetEntry(0x00, (void*) _Int_0x00, IDT_GATE_INT32, 0);
    IdtSetEntry(0x01, (void*) _Int_0x01, IDT_GATE_INT32, 0);
    IdtSetEntry(0x02, (void*) _Int_0x02, IDT_GATE_INT32, 0);
    IdtSetEntry(0x03, (void*) _Int_0x03, IDT_GATE_INT32, 0);
    IdtSetEntry(0x04, (void*) _Int_0x04, IDT_GATE_INT32, 0);
    IdtSetEntry(0x05, (void*) _Int_0x05, IDT_GATE_INT32, 0);
    IdtSetEntry(0x06, (void*) _Int_0x06, IDT_GATE_INT32, 0);
    IdtSetEntry(0x07, (void*) _Int_0x07, IDT_GATE_INT32, 0);
    IdtSetEntry(0x08, (void*) _Int_0x08, IDT_GATE_INT32, 0);
    IdtSetEntry(0x0A, (void*) _Int_0x0A, IDT_GATE_INT32, 0);
    IdtSetEntry(0x0B, (void*) _Int_0x0B, IDT_GATE_INT32, 0);
    IdtSetEntry(0x0C, (void*) _Int_0x0C, IDT_GATE_INT32, 0);
    IdtSetEntry(0x0D, (void*) _Int_0x0D, IDT_GATE_INT32, 0);
    IdtSetEntry(0x0E, (void*) _Int_0x0E, IDT_GATE_INT32, 0);
    IdtSetEntry(0x10, (void*) _Int_0x10, IDT_GATE_INT32, 0);
    IdtSetEntry(0x11, (void*) _Int_0x11, IDT_GATE_INT32, 0);
    IdtSetEntry(0x12, (void*) _Int_0x12, IDT_GATE_INT32, 0);
    IdtSetEntry(0x13, (void*) _Int_0x13, IDT_GATE_INT32, 0);
    IdtSetEntry(0x14, (void*) _Int_0x14, IDT_GATE_INT32, 0);
    IdtSetEntry(0x1E, (void*) _Int_0x1E, IDT_GATE_INT32, 0);
    IdtSetEntry(0x20, (void*) _Int_0x20, IDT_GATE_INT32, 0);
    IdtSetEntry(0x21, (void*) _Int_0x21, IDT_GATE_INT32, 0);
    IdtSetEntry(0x22, (void*) _Int_0x22, IDT_GATE_INT32, 0);
    IdtSetEntry(0x23, (void*) _Int_0x23, IDT_GATE_INT32, 0);
    IdtSetEntry(0x24, (void*) _Int_0x24, IDT_GATE_INT32, 0);
    IdtSetEntry(0x25, (void*) _Int_0x25, IDT_GATE_INT32, 0);
    IdtSetEntry(0x26, (void*) _Int_0x26, IDT_GATE_INT32, 0);
    IdtSetEntry(0x27, (void*) _Int_0x27, IDT_GATE_INT32, 0);
    IdtSetEntry(0x28, (void*) _Int_0x28, IDT_GATE_INT32, 0);
    IdtSetEntry(0x29, (void*) _Int_0x29, IDT_GATE_INT32, 0);
    IdtSetEntry(0x2A, (void*) _Int_0x2A, IDT_GATE_INT32, 0);
    IdtSetEntry(0x2B, (void*) _Int_0x2B, IDT_GATE_INT32, 0);
    IdtSetEntry(0x2C, (void*) _Int_0x2C, IDT_GATE_INT32, 0);
    IdtSetEntry(0x2D, (void*) _Int_0x2D, IDT_GATE_INT32, 0);
    IdtSetEntry(0x2E, (void*) _Int_0x2E, IDT_GATE_INT32, 0);
    IdtSetEntry(0x2F, (void*) _Int_0x2F, IDT_GATE_INT32, 0);
    IdtSetEntry(0x80, (void*) _Int_0x80, IDT_GATE_INT32, 3);
    IdtLoad();
    
    asm volatile("sti");
}

void IdtSetEntry(
    uint8_t index,
    void* callback,
    uint8_t gateType,
    uint8_t minDpl
)
{
    uint64_t result = 0;
    
    /* Callback */
    uint64_t callbackPtr = (uint64_t) callback;
    result |= callbackPtr & 0xFFFF;
    callbackPtr >>= 16;
    result |= (callbackPtr & 0xFFFF) << 48;
    
    /* Code selector */
    result |= GDT_KCODE << 16;
    
    /* Type and attributes */
    uint8_t typeAttributes = 1 << 7;
    typeAttributes |= minDpl << 5;
    typeAttributes |= gateType & 0xF;
    if (gateType == IDT_GATE_TASK32) typeAttributes |= 1 << 4;
    result |= ((uint64_t) typeAttributes) << 40; 
    
    IdtTable[index] = result;
}

#include <McpgOS.h>

uint64_t IdtTable[256];
IdtIntCallback IdtIntHandlers[256] = {0};

IdtFrame* IdtGlobalIntHandler(IdtFrame* idtFrame)
{
    uint8_t n = idtFrame->InterruptNumber;

    // TODO: remove the temporary handling of a PF
    if (idtFrame->InterruptNumber == 0x0E)
    {
        *((uint8_t*) 0xB8000) = 'P';
        *((uint8_t*) 0xB8002) = 'F';
        while(1);
    }

    if (idtFrame->InterruptNumber <= 0x1E)
    {
        *((uint8_t*) 0xB8000) = ':';
        *((uint8_t*) 0xB8002) = '/';
        while(1);
    }

    if (IdtIntHandlers[n] != NULL)
    {
        idtFrame = IdtIntHandlers[n](idtFrame);
    }

    if (n >= 0x20 && n <= 0x2F)
    {
        PicEOI(n - 0x20);
    }
    return idtFrame;
}

void IdtInit()
{
    for (int i = 0; i < 256; i++)
    {
        IdtSetEntry(i, (void*) _IntDefault, IdtGateInt32, 3);
    }
    
    IdtSetEntry(0x00, (void*) _Int_0x00, IdtGateInt32, 0);
    IdtSetEntry(0x01, (void*) _Int_0x01, IdtGateInt32, 0);
    IdtSetEntry(0x02, (void*) _Int_0x02, IdtGateInt32, 0);
    IdtSetEntry(0x03, (void*) _Int_0x03, IdtGateInt32, 0);
    IdtSetEntry(0x04, (void*) _Int_0x04, IdtGateInt32, 0);
    IdtSetEntry(0x05, (void*) _Int_0x05, IdtGateInt32, 0);
    IdtSetEntry(0x06, (void*) _Int_0x06, IdtGateInt32, 0);
    IdtSetEntry(0x07, (void*) _Int_0x07, IdtGateInt32, 0);
    IdtSetEntry(0x08, (void*) _Int_0x08, IdtGateInt32, 0);
    IdtSetEntry(0x0A, (void*) _Int_0x0A, IdtGateInt32, 0);
    IdtSetEntry(0x0B, (void*) _Int_0x0B, IdtGateInt32, 0);
    IdtSetEntry(0x0C, (void*) _Int_0x0C, IdtGateInt32, 0);
    IdtSetEntry(0x0D, (void*) _Int_0x0D, IdtGateInt32, 0);
    IdtSetEntry(0x0E, (void*) _Int_0x0E, IdtGateInt32, 0);
    IdtSetEntry(0x10, (void*) _Int_0x10, IdtGateInt32, 0);
    IdtSetEntry(0x11, (void*) _Int_0x11, IdtGateInt32, 0);
    IdtSetEntry(0x12, (void*) _Int_0x12, IdtGateInt32, 0);
    IdtSetEntry(0x13, (void*) _Int_0x13, IdtGateInt32, 0);
    IdtSetEntry(0x14, (void*) _Int_0x14, IdtGateInt32, 0);
    IdtSetEntry(0x1E, (void*) _Int_0x1E, IdtGateInt32, 0);
    IdtSetEntry(0x20, (void*) _Int_0x20, IdtGateInt32, 0);
    IdtSetEntry(0x21, (void*) _Int_0x21, IdtGateInt32, 0);
    IdtSetEntry(0x22, (void*) _Int_0x22, IdtGateInt32, 0);
    IdtSetEntry(0x23, (void*) _Int_0x23, IdtGateInt32, 0);
    IdtSetEntry(0x24, (void*) _Int_0x24, IdtGateInt32, 0);
    IdtSetEntry(0x25, (void*) _Int_0x25, IdtGateInt32, 0);
    IdtSetEntry(0x26, (void*) _Int_0x26, IdtGateInt32, 0);
    IdtSetEntry(0x27, (void*) _Int_0x27, IdtGateInt32, 0);
    IdtSetEntry(0x28, (void*) _Int_0x28, IdtGateInt32, 0);
    IdtSetEntry(0x29, (void*) _Int_0x29, IdtGateInt32, 0);
    IdtSetEntry(0x2A, (void*) _Int_0x2A, IdtGateInt32, 0);
    IdtSetEntry(0x2B, (void*) _Int_0x2B, IdtGateInt32, 0);
    IdtSetEntry(0x2C, (void*) _Int_0x2C, IdtGateInt32, 0);
    IdtSetEntry(0x2D, (void*) _Int_0x2D, IdtGateInt32, 0);
    IdtSetEntry(0x2E, (void*) _Int_0x2E, IdtGateInt32, 0);
    IdtSetEntry(0x2F, (void*) _Int_0x2F, IdtGateInt32, 0);
    IdtSetEntry(0x83, (void*) _Int_0x83, IdtGateInt32, 3);
    IdtLoad();
}

void IdtSetEntry(
    uint8_t index,
    void* callback,
    uint8_t gateType,
    uint8_t minDpl
)
{
    uint64_t result = 0;
    
    uint32_t callbackPtr = (uint32_t) callback;
    result |= callbackPtr & 0xFFFF;
    callbackPtr >>= 16;
    result |= ((uint64_t) (callbackPtr & 0xFFFF)) << 48;
    
    /* Code selector */
    result |= GdtKernelCode << 16;
    
    /* Type and attributes */
    uint8_t typeAttributes = 1 << 7;
    typeAttributes |= minDpl << 5;
    typeAttributes |= gateType & 0xF;
    if (gateType == IdtGateTask32) typeAttributes |= 1 << 4;
    result |= ((uint64_t) typeAttributes) << 40; 
    
    IdtTable[index] = result;
}

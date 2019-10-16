#pragma once

#include <McpgOS.h>

typedef enum
{
    IdtGateTask32 = 0x5,
    IdtGateInt32  = 0xE,
    IdtGateTrap32 = 0xF
} IdtGateType;

typedef struct
{
    // Pushed by int handler
    uint32_t Gs;
    uint32_t Fs;
    uint32_t Es;
    uint32_t Ds;

    // Pushed by pushad
    uint32_t Edi;
    uint32_t Esi;
    uint32_t Ebp;
    uint32_t Esp;
    uint32_t Ebx;
    uint32_t Edx;
    uint32_t Ecx;
    uint32_t Eax;

    // Pushed by int handler
    uint32_t InterruptNumber;

    // Pushed by the CPU
    uint32_t ErrorCode;
    uint32_t Eip;
    uint32_t Cs;
    uint32_t Eflags;
    uint32_t UserEsp;
    uint32_t Ss;
} IdtFrame;

extern uint64_t IdtTable[256];

typedef IdtFrame* (*IdtIntCallback)(IdtFrame*);
extern IdtIntCallback IdtIntHandlers[256];

// Default interrupt handler, running iret as soon as it's called
void _IntDefault();

// Raw interrupt handlers
void _Int_0x00();
void _Int_0x01();
void _Int_0x02();
void _Int_0x03();
void _Int_0x04();
void _Int_0x05();
void _Int_0x06();
void _Int_0x07();
void _Int_0x08();
void _Int_0x0A();
void _Int_0x0B();
void _Int_0x0C();
void _Int_0x0D();
void _Int_0x0E();
void _Int_0x10();
void _Int_0x11();
void _Int_0x12();
void _Int_0x13();
void _Int_0x14();
void _Int_0x1E();
void _Int_0x20();
void _Int_0x21();
void _Int_0x22();
void _Int_0x23();
void _Int_0x24();
void _Int_0x25();
void _Int_0x26();
void _Int_0x27();
void _Int_0x28();
void _Int_0x29();
void _Int_0x2A();
void _Int_0x2B();
void _Int_0x2C();
void _Int_0x2D();
void _Int_0x2E();
void _Int_0x2F();
void _Int_0x83();

IdtFrame* IdtGlobalIntHandler(IdtFrame* idtFrame);

void IdtInit();
void IdtLoad();
void IdtSetEntry(
    uint8_t index,
    void* callback,
    uint8_t gateType,
    uint8_t minDpl
);

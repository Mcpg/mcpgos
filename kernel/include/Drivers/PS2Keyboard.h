#pragma once

#include <McpgOS.h>

extern IoStream* PS2KbdStream;

typedef enum
{
    PS2KbdSetLED          = 0xED,
    PS2KbdEcho            = 0xEE,
    PS2KbdScanCode        = 0xF0,
    PS2KbdIdentify        = 0xF2,
    PS2KbdSetTypematic    = 0xF3,
    PS2KbdScanningOn      = 0xF4,
    PS2KbdScanningOff     = 0xF5,
    PS2KbdSetDefParams    = 0xF6,
    PS2KbdSetAllTypematic = 0xF7,
    PS2KbdSetAllMR        = 0xF8,
    PS2KbdSetAllMo        = 0xF9,
    PS2KbdSetAllTAMR      = 0xFA,
    PS2KbdSetKeyOnlyTA    = 0xFB,
    PS2KbdSetKeyOnlyMR    = 0xFC,
    PS2KbdSetKeyOnlyRel   = 0xFD,
    PS2KbdResend          = 0xFE,
    PS2KbdReset           = 0xFF
} PS2KbdCommandType;

typedef enum
{
    PS2KbdError1         = 0x00,
    PS2KbdSelfTestOK     = 0xAA,
    PS2KbdEchoResponse   = 0xEE,
    PS2KbdAck            = 0xFA,
    PS2KbdSelfTestFail1  = 0xFC,
    PS2KbdSelfTestFail2  = 0xFD,
    PS2KbdResendResponse = 0xFE,
    PS2KbdError2         = 0xFF
} PS2KbdResponse;

typedef enum
{
    PS2KbdIoData  = 0x60,
    PS2KbdIoRead  = 0x64,
    PS2KbdIoWrite = 0x64
} PS2KbdIoPort;

typedef struct
{
    uint8_t Command;
    uint8_t* Data;
    uint8_t* DataSize;
} PS2KbdCommand;

void PS2KbdInit();
bool PS2KbdSendCommand(PS2KbdCommand* cmd);

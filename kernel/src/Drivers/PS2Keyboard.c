#include <McpgOS.h>

static DmDriverDefinition PS2KbdDef;

static IdtFrame* PS2KbdIrq1Handler(IdtFrame* frame)
{

    return frame;
}

void PS2KbdInit()
{
    IdtIntHandlers[0x21] = PS2KbdIrq1Handler;

    PS2KbdDef.DriverID = "PS2Kbd";
    DmRegisterDriver(&PS2KbdDef);
}

bool PS2KbdSendCommand(PS2KbdCommand* cmd)
{
    return false;
}

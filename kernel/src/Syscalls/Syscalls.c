#include <McpgOS.h>

SyscallHandler SyscallHandlers[SYSCALL_HANDLER_AMOUNT] = {0};

int SyscallTest(uint32_t n)
{
    uint32_t i;
    for (i = 0; i < n; i++)
    {
        Kprintf("Hello world\n");
    }
    return n * 2;
}

static IdtFrame* SyscallIntHandler(IdtFrame* frame)
{
    if (frame->Eax >= SYSCALL_HANDLER_AMOUNT)
        return frame;

    frame->Eax = SyscallHandlers[frame->Eax](
        frame->Ebx, frame->Ecx, frame->Edx, frame->Esi, frame->Edi
    );

    return frame;
}

void SyscallInit()
{
    SyscallHandlers[0] = (SyscallHandler) SyscallTest;

    IdtSetHandler(0x83, SyscallIntHandler);
}


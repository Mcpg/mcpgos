#include <McpgOS.h>

uint16_t CurrentTask = 0;
uint16_t SchedTaskCount = 0;
SchedTask* SchedTasks[SCHED_MAX_TASK_AMOUNT];
uint64_t SchedTickCounter = 0;
static uint32_t TaskIDCounter = 0;

static SchedTask IdleTask;
static SchedTask IdleTask2;

static IdtFrame* Irq0Handler(IdtFrame* idtFrame)
{
    SchedTask* task;

    SchedTickCounter++;

    CurrentTask++;
    if (CurrentTask >= SchedTaskCount)
    {
        CurrentTask = 0;
    }

    if (CurrentTask == 0)
    {
        Kprintf("task is 0\n");
    }

    task = SchedTasks[CurrentTask];
    idtFrame->Eip = task->CPUFrame.Eip;
    idtFrame->Eflags = task->CPUFrame.Eflags;
    idtFrame->UserEsp = task->CPUFrame.Esp;

    idtFrame->Cs = task->CPUFrame.Cs;
    idtFrame->Ds = task->CPUFrame.Ds;
    idtFrame->Es = task->CPUFrame.Es;
    idtFrame->Fs = task->CPUFrame.Fs;
    idtFrame->Gs = task->CPUFrame.Gs;
    idtFrame->Ss = task->CPUFrame.Ss;

    idtFrame->Eax = task->CPUFrame.Eax;
    idtFrame->Ebx = task->CPUFrame.Ebx;
    idtFrame->Ecx = task->CPUFrame.Ecx;
    idtFrame->Edx = task->CPUFrame.Edx;
    idtFrame->Esi = task->CPUFrame.Esi;
    idtFrame->Edi = task->CPUFrame.Edi;
    idtFrame->Ebp = task->CPUFrame.Ebp;

    return idtFrame;
}

static void Task2()
{
    while(1)
    {
        Kprintf("Task 2!\n");
        asm("hlt");
    }
}

void SchedInit()
{
    asm("cli");
    IdtHLHandlers[0x20] = Irq0Handler;

    IdleTask.IsKernelTask = 1;
    IdleTask.CPUFrame.Gs = GDT_KDATA;
    IdleTask.CPUFrame.Fs = GDT_KDATA;
    IdleTask.CPUFrame.Ss = GDT_KDATA;
    IdleTask.CPUFrame.Es = GDT_KDATA;
    IdleTask.CPUFrame.Ds = GDT_KDATA;
    IdleTask.CPUFrame.Cs = GDT_KCODE;

    IdleTask.CPUFrame.Edi = 0;
    IdleTask.CPUFrame.Esi = 0;
    IdleTask.CPUFrame.Ebp = ((uint32_t) malloc(4096)) + 4096;
    IdleTask.CPUFrame.Esp = IdleTask.CPUFrame.Ebp;
    IdleTask.CPUFrame.Ebx = 0;
    IdleTask.CPUFrame.Edx = 0;
    IdleTask.CPUFrame.Ecx = 0;
    IdleTask.CPUFrame.Eax = 0;
    
    IdleTask.CPUFrame.Eip = (uint32_t) SchedIdle;
    IdleTask.CPUFrame.Eflags = 0x200; // int flag set

    ///////

    IdleTask2.IsKernelTask = 1;
    IdleTask2.CPUFrame.Gs = GDT_KDATA;
    IdleTask2.CPUFrame.Fs = GDT_KDATA;
    IdleTask2.CPUFrame.Ss = GDT_KDATA;
    IdleTask2.CPUFrame.Es = GDT_KDATA;
    IdleTask2.CPUFrame.Ds = GDT_KDATA;
    IdleTask2.CPUFrame.Cs = GDT_KCODE;

    IdleTask2.CPUFrame.Edi = 0;
    IdleTask2.CPUFrame.Esi = 0;
    IdleTask2.CPUFrame.Ebp = ((uint32_t) malloc(4096)) + 4096;
    IdleTask2.CPUFrame.Esp = IdleTask.CPUFrame.Ebp;
    IdleTask2.CPUFrame.Ebx = 0;
    IdleTask2.CPUFrame.Edx = 0;
    IdleTask2.CPUFrame.Ecx = 0;
    IdleTask2.CPUFrame.Eax = 0;
    
    IdleTask2.CPUFrame.Eip = (uint32_t) Task2;
    IdleTask2.CPUFrame.Eflags = 0x200; // int flag set


    SchedAddTask(&IdleTask);
    SchedAddTask(&IdleTask2);
}

void SchedIdle()
{
    //asm("sti");
    while(1)
    {
        Kprintf("Task 1!\n");
        asm("hlt");
    }
}

int SchedAddTask(SchedTask* task)
{
    if (SchedTaskCount == SCHED_MAX_TASK_AMOUNT)
        return 0;

    KAssert(task != NULL);

    SchedTasks[SchedTaskCount++] = task;
    task->TaskID = TaskIDCounter++;

    return 1;
}

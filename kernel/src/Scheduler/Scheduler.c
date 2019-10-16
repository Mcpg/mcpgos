#include <McpgOS.h>

uint64_t SchedTickCounter = 0;

uint32_t SchedTaskCount = 0;
SchedTask* SchedIdleTask = NULL;
SchedTask* SchedCurrentTask;

static SchedTask* SchedTaskListEnd;

static inline void SchedSaveFrame(IdtFrame* source, SchedTask* target)
{
    target->CPUFrame.Eax = source->Eax;
    target->CPUFrame.Ebx = source->Ebx;
    target->CPUFrame.Ecx = source->Ecx;
    target->CPUFrame.Edx = source->Edx;
    target->CPUFrame.Esi = source->Esi;
    target->CPUFrame.Edi = source->Edi;
    target->CPUFrame.Esp = source->Esp;
    target->CPUFrame.Ebp = source->Ebp;
    target->CPUFrame.Eip = source->Eip;
    target->CPUFrame.Eflags = source->Eflags;
    target->CPUFrame.Cs = source->Cs;
    target->CPUFrame.Ds = source->Ds;
    target->CPUFrame.Es = source->Es;
    target->CPUFrame.Fs = source->Fs;
    target->CPUFrame.Gs = source->Gs;
    target->CPUFrame.Ss = source->Ss;
}

static inline void SchedLoadFrame(IdtFrame* target, SchedTask* source)
{
    target->Eax = source->CPUFrame.Eax;
    target->Ebx = source->CPUFrame.Ebx;
    target->Ecx = source->CPUFrame.Ecx;
    target->Edx = source->CPUFrame.Edx;
    target->Esi = source->CPUFrame.Esi;
    target->Edi = source->CPUFrame.Edi;
    target->Esp = source->CPUFrame.Esp;
    target->Ebp = source->CPUFrame.Ebp;
    target->Eip = source->CPUFrame.Eip;
    target->Eflags = source->CPUFrame.Eflags;
    target->Cs = source->CPUFrame.Cs;
    target->Ds = source->CPUFrame.Ds;
    target->Es = source->CPUFrame.Es;
    target->Fs = source->CPUFrame.Fs;
    target->Ss = source->CPUFrame.Ss;
}

static IdtFrame* Irq0Handler(IdtFrame* idtFrame)
{
    static bool switchedAtLeastOnce = false;

    SchedTickCounter++;

    // TODO:
    //   * Sleep
    //   * Giving tasks a bit more work time
    if (switchedAtLeastOnce)
        SchedSaveFrame(idtFrame, SchedCurrentTask);
    else
        switchedAtLeastOnce = true;

    SchedCurrentTask = SchedCurrentTask->Next;
    SchedLoadFrame(idtFrame, SchedCurrentTask);

    return idtFrame;
}

static void SchedIdle()
{
    while (1)
    {
        HALT();
    }
}

void SchedInit()
{
    if (!SchedCreateKernelTask("IdleTask", SchedIdle, 128))
        KPanic("Couldn't start the IdleTask!");

    CLI();
}

void SchedEnable()
{
    IdtIntHandlers[0x20] = Irq0Handler;
    STI();
}

SchedTask* SchedGetTask(int taskID)
{
    bool foundTask = false;
    SchedTask* currentTask = SchedIdleTask;

    do
    {
        if (currentTask->TaskID == taskID)
        {
            foundTask = true;
            break;
        }
        else
        {
            currentTask = currentTask->Next;
            continue;
        }
    } while (true);

    return foundTask ? currentTask : NULL;
}

int SchedAddTask(SchedTask* task)
{
    static uint32_t TaskIDCounter = 1;

    KAssert(task != NULL);

    CLI();

    if (SchedIdleTask == NULL)
    {
        SchedIdleTask = task;
        SchedCurrentTask = task;
        SchedTaskListEnd = task;

        task->Prev = task;
        task->Next = task;
    }
    else
    {
        SchedIdleTask->Prev = task;
        SchedTaskListEnd->Next = task;
        SchedTaskListEnd = task;
    }

    SchedTaskCount++;
    task->TaskID = TaskIDCounter++;

    STI();

    return task->TaskID;
}

int SchedRemoveTask(int taskID)
{
    SchedTask* task;
    SchedTask* next;

    CLI();

    task = SchedGetTask(taskID);
    if (!task)
        return false;

    next = task->Next;
    task->Next = task->Prev;
    task->Prev = next;

    SchedTaskCount--;

    free(task);

    STI();

    return true;
}

int SchedCreateKernelTask(char* name, void* func, size_t stackSize)
{
    SchedTask* task = (SchedTask*) malloc(sizeof(SchedTask));
    if (!task)
        return false;

    task->IsKernelTask = true;
    task->KernelTaskInfo.TaskName = name;
    task->CPUFrame.Cs = GdtKernelCode;
    task->CPUFrame.Ds = GdtKernelData;
    task->CPUFrame.Es = GdtKernelData;
    task->CPUFrame.Fs = GdtKernelData;
    task->CPUFrame.Gs = GdtKernelData;
    task->CPUFrame.Ss = GdtKernelData;
    task->CPUFrame.Eax = 0;
    task->CPUFrame.Ebx = 0;
    task->CPUFrame.Ecx = 0;
    task->CPUFrame.Edx = 0;
    task->CPUFrame.Esi = 0;
    task->CPUFrame.Edi = 0;
    task->CPUFrame.Ebp = ((uint32_t) malloc(stackSize)) + stackSize;
    task->CPUFrame.Esp = task->CPUFrame.Ebp;
    task->CPUFrame.Eip = (uint32_t) func;
    task->CPUFrame.Eflags = 0x200; // int bit set

    return SchedAddTask(task);
}

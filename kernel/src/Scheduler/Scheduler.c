#include <McpgOS.h>

SchedKernelTaskElement* SchedKernelTasks = NULL;
SchedTask* SchedCurrentTask = NULL;

uint64_t SchedTickCounter = 0;

// Currently iterated process
ProcessListElement* SchedCurrentProcess = NULL; // may be NULL

// 2 ms of CPU time per task
static const uint32_t SchedTaskTime = 2;

// Currently iterated kernel task
static SchedKernelTaskElement* SchedCurrKernelTask = NULL;
// Currently iterated process thread
static ProcessThreadElement* SchedCurrUserTask = NULL;

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

static inline void SchedStartKernelTaskIteration()
{
    SchedCurrentProcess = NULL;
    SchedCurrKernelTask = SchedKernelTasks;
    SchedCurrentTask = SchedCurrKernelTask->Task;
}

static inline void SchedFindNextProcess()
{
    while (true)
    {
        SchedCurrUserTask = SchedCurrentProcess->Process->Threads;
        if (SchedCurrUserTask == NULL)
        {
            SchedCurrentProcess =
                (ProcessListElement*) SchedCurrentProcess->ListElement.Next;
            if (SchedCurrentProcess == NULL)
            {
                SchedStartKernelTaskIteration();
                return;
            }
            continue;
        }
        break;
    }
    SchedCurrentTask = SchedCurrentProcess->Process->Threads->Task;
}

static void SchedPickNextTask()
{
    if (SchedCurrentProcess == NULL)
    {
        SchedCurrKernelTask =
            (SchedKernelTaskElement*) SchedCurrKernelTask->ListElement.Next;
        if (SchedCurrKernelTask == NULL)
        {
            SchedCurrentProcess = ProcessList;
            if (SchedCurrentProcess == NULL)
                SchedStartKernelTaskIteration();
            else
                SchedFindNextProcess();
            return;
        }
        else
        {
            SchedCurrentTask = SchedCurrKernelTask->Task;
            return;
        }
    }
    else
    {
        SchedCurrUserTask =
            (ProcessThreadElement*) SchedCurrUserTask->ListElement.Next;
        if (SchedCurrUserTask == NULL)
        {
            SchedCurrentProcess =
                (ProcessListElement*) SchedCurrentProcess->ListElement.Next;
            SchedFindNextProcess();
            return;
        }
        else
        {
            SchedCurrentTask = SchedCurrUserTask->Task;
            return;
        }
    }
}

static IdtFrame* SchedIrqHandler(IdtFrame* frame)
{
    static bool initialized = false;

    SchedTickCounter++;

    if (!initialized)
        initialized = true;
    else
        SchedSaveFrame(frame, SchedCurrentTask);
    
    SchedCurrentTask->RemainingCpuTime--;
    if (SchedCurrentTask->RemainingCpuTime == 0)
    {
        do
        {
            SchedPickNextTask();
            if (SchedCurrentTask->TaskState == TASK_STATE_SLEEPING)
                SchedCurrentTask->RemainingCpuTime--;
        } while (SchedCurrentTask->TaskState == TASK_STATE_SLEEPING);

        SchedCurrentTask->TaskState = TASK_STATE_RUNNING;
        SchedCurrentTask->RemainingCpuTime = SchedTaskTime;
    }

    SchedLoadFrame(frame, SchedCurrentTask);

    if (SchedCurrentProcess != NULL)
    {
        if (GetCurrentPageDirectory()
                != SchedCurrentProcess->Process->PageDirectory)
        {
            SwitchPageDirectory(MmVirtToPhys(
                GetCurrentPageDirectory(),
                (uintptr_t) SchedCurrentProcess->Process->PageDirectory
            ));
        }
    }

    return frame;
}

static void SchedIdle()
{
    while (true) HALT();
}

void SchedInit()
{
    CLI();

    IdtSetHandler(0x20, SchedIrqHandler);

    SchedCurrentTask = SchedCreateKernelTask("KIdle", (void*) SchedIdle, 64);
    if (SchedCurrentTask == NULL)
        KPanic("Couldn't create kernel idle task!");

    SchedCurrKernelTask = SchedKernelTasks;
}

void SchedSleep(SchedTask* task, uint32_t millis)
{
    if (task == NULL)
        task = SchedCurrentTask;

    task->TaskState = TASK_STATE_SLEEPING;
    task->RemainingCpuTime = millis;
}

SchedTask* SchedCreateUserThread(Process* owner, uint32_t eip, uint32_t stackPtr)
{
    ProcessThreadElement* element;
    SchedTask* task;

    KAssert(owner != NULL);

    task = (SchedTask*) malloc(sizeof(SchedTask));
    if (task == NULL)
        return NULL;

    task->IsKernelTask = owner->IsKernelMode;
    task->UserTaskInfo.ParentProcess = owner;
    task->CPUFrame.Cs = GdtUserCode;
    task->CPUFrame.Ds = GdtUserData;
    task->CPUFrame.Es = GdtUserData;
    task->CPUFrame.Fs = GdtUserData;
    task->CPUFrame.Gs = GdtUserData;
    task->CPUFrame.Ss = GdtUserData;
    task->CPUFrame.Eax = 0;
    task->CPUFrame.Ebx = 0;
    task->CPUFrame.Ecx = 0;
    task->CPUFrame.Edx = 0;
    task->CPUFrame.Esi = 0;
    task->CPUFrame.Edi = 0;
    task->CPUFrame.Ebp = stackPtr;
    task->CPUFrame.Esp = stackPtr;
    task->CPUFrame.Eip = eip;
    task->CPUFrame.Eflags = 0x200; // int bit set

    task->TaskState = TASK_STATE_RUNNING;
    task->RemainingCpuTime = SchedTaskTime;

    element = (ProcessThreadElement*) malloc(sizeof(ProcessThreadElement));
    if (element == NULL)
    {
        free((void*) task->CPUFrame.Ebp);
        free(task);
        return NULL;
    }

    element->ListElement.Next = NULL;
    element->ListElement.Prev = NULL;
    element->Task = task;

    if (owner->Threads == NULL)
        owner->Threads = element;
    else
        LListPushBackC(owner->Threads, element);

    return task;
}

SchedTask* SchedCreateKernelTask(char* name, void* func, size_t stackSize)
{
    SchedKernelTaskElement* element;

    SchedTask* task = (SchedTask*) malloc(sizeof(SchedTask));
    if (task == NULL)
        return NULL;

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

    task->TaskState = TASK_STATE_RUNNING;
    task->RemainingCpuTime = SchedTaskTime;

    element = (SchedKernelTaskElement*) malloc(sizeof(SchedKernelTaskElement));
    if (element == NULL)
    {
        free((void*) task->CPUFrame.Ebp);
        free(task);
        return NULL;
    }

    element->ListElement.Next = NULL;
    element->ListElement.Prev = NULL;
    element->Task = task;

    if (SchedKernelTasks == NULL)
        SchedKernelTasks = element;
    else
        LListPushBackC(SchedKernelTasks, element);

    return task;
}

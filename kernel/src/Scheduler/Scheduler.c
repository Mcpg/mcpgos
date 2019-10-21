#include <McpgOS.h>

// TODO: Verify whether SchedSaveFrame and SchedLoadFrame are safe

static inline void SchedSaveFrame(IdtFrame* source, SchedThread* target)
{
    target->CPUFrame.Eax = source->Eax;
    target->CPUFrame.Ebx = source->Ebx;
    target->CPUFrame.Ecx = source->Ecx;
    target->CPUFrame.Edx = source->Edx;
    target->CPUFrame.Esi = source->Esi;
    target->CPUFrame.Edi = source->Edi;
    //target->CPUFrame.Esp = source->Esp;
    target->CPUFrame.Esp = source->UserEsp;
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

static inline void SchedLoadFrame(IdtFrame* target, SchedThread* source)
{
    target->Eax = source->CPUFrame.Eax;
    target->Ebx = source->CPUFrame.Ebx;
    target->Ecx = source->CPUFrame.Ecx;
    target->Edx = source->CPUFrame.Edx;
    target->Esi = source->CPUFrame.Esi;
    target->Edi = source->CPUFrame.Edi;
    //target->Esp = source->CPUFrame.Esp;
    target->UserEsp = source->CPUFrame.Esp;
    target->Ebp = source->CPUFrame.Ebp;
    target->Eip = source->CPUFrame.Eip;
    target->Eflags = source->CPUFrame.Eflags;
    target->Cs = source->CPUFrame.Cs;
    target->Ds = source->CPUFrame.Ds;
    target->Es = source->CPUFrame.Es;
    target->Fs = source->CPUFrame.Fs;
    target->Gs = source->CPUFrame.Gs;
    target->Ss = source->CPUFrame.Ss;
}

uint64_t SchedTickCounter = 0;
ProcListElement* SchedProcessList;

static ProcListElement* CurrIteratedProc;
static ProcThreadElement* CurrIteratedThread;

// 2 ms of CPU time per thread
static const int SchedTimePerThread = 2;

static inline void PickNextThread()
{
    CurrIteratedThread =
        (ProcThreadElement*) CurrIteratedThread->ListElement.Next;

    if (CurrIteratedThread == NULL)
    {
        while (true)
        {
            CurrIteratedProc =
                (ProcListElement*) CurrIteratedProc->ListElement.Next;
            if (CurrIteratedProc == NULL)
                CurrIteratedProc = SchedProcessList;

            CurrIteratedThread = CurrIteratedProc->Process->Threads;
            if (CurrIteratedThread == NULL)
                continue;

            break;
        }
    }
}

static IdtFrame* SchedIrqHandler(IdtFrame* frame)
{
    static bool initialized = false;

    SchedTickCounter++;

    if (initialized)
        SchedSaveFrame(frame, CurrIteratedThread->Thread);
    else
        initialized = true;

    if (CurrIteratedThread->Thread->State == TASK_STATE_RUNNING)
    {
        if (CurrIteratedThread->Thread->RemainingCpuTime > 0)
            CurrIteratedThread->Thread->RemainingCpuTime--;

        if (CurrIteratedThread->Thread->RemainingCpuTime == 0)
        {
            pickNextTask:
            do
            {
                PickNextThread();
                if (CurrIteratedThread->Thread->State == TASK_STATE_SLEEPING)
                    CurrIteratedThread->Thread->RemainingCpuTime--;
            } while (CurrIteratedThread->Thread->State == TASK_STATE_SLEEPING);
            CurrIteratedThread->Thread->RemainingCpuTime = SchedTimePerThread;
        }
    }
    else // sleeping
    {
        CurrIteratedThread->Thread->RemainingCpuTime--;
        if (CurrIteratedThread->Thread->RemainingCpuTime == 0)
        {
            CurrIteratedThread->Thread->State = TASK_STATE_RUNNING;
            CurrIteratedThread->Thread->RemainingCpuTime = SchedTimePerThread;
        }
        else
        {
            goto pickNextTask;
        }
    }

    if (CurrentPageDirectory != CurrIteratedProc->Process->PageDirectory &&
        CurrIteratedProc->Process->PageDirectory != NULL)
    {
        SwitchPageDirectory(CurrIteratedProc->Process->PageDirectory);
    }

    SchedLoadFrame(frame, CurrIteratedThread->Thread);

    return frame;
}

static void SchedIdle()
{
    while (true) HALT();
}

void SchedInit()
{
    SchedCreateProcess(true, false, 0);
    SchedCreateThread(
        SchedProcessList->Process, "KIdle",
        SchedIdle, (void*) STACK_ALLOCATE(128)
    );

    CurrIteratedProc = SchedProcessList;
    CurrIteratedThread = CurrIteratedProc->Process->Threads;

    IdtSetHandler(0x20, SchedIrqHandler);
}

void SchedSleep(SchedThread* thread, uint32_t millis)
{
    KAssert(thread != NULL);

    thread->State = TASK_STATE_SLEEPING;
    thread->RemainingCpuTime = millis;
}

Process* SchedCreateProcess(
    bool kernelMode, bool allocPageDir,
    uint32_t uid
)
{
    static uint32_t currentPid = 0;

    Process* process;
    ProcListElement* element;

    process = tmalloc(Process);
    KAssert(process != NULL);
    element = tmalloc(ProcListElement);
    KAssert(element != NULL);
    
    process->ProcessId = currentPid++;
    process->UserId = uid;
    process->IsKernelMode = kernelMode;
    process->PageDirectory = NULL;
    process->Threads = NULL;

    if (allocPageDir)
        process->PageDirectory = MmAllocPageDirectory();

    element->ListElement.Prev = NULL;
    element->ListElement.Next = NULL;
    element->Process = process;

    if (SchedProcessList == NULL)
        SchedProcessList = element;
    else
        LListPushBackC(SchedProcessList, element);

    return process;
}


SchedThread* SchedCreateThread(
    Process* owner, char* name,
    void* eip, void* stack
)
{
    SchedThread* thread;
    ProcThreadElement* element;

    uint32_t codeSegment = owner->IsKernelMode ? GdtKernelCode : (GdtUserCode+3);
    uint32_t dataSegment = owner->IsKernelMode ? GdtKernelData : (GdtUserData+3);

    KAssert(owner != NULL);
    KAssert(name != NULL);

    thread = tmalloc(SchedThread);
    KAssert(thread != NULL);
    element = tmalloc(ProcThreadElement);
    KAssert(element != NULL);

    thread->ParentProcess = owner;
    thread->ThreadName = name;
    thread->State = TASK_STATE_RUNNING;
    thread->RemainingCpuTime = SchedTimePerThread;

    thread->CPUFrame.Cs = codeSegment;
    thread->CPUFrame.Ds = dataSegment;
    thread->CPUFrame.Es = dataSegment;
    thread->CPUFrame.Fs = dataSegment;
    thread->CPUFrame.Gs = dataSegment;
    thread->CPUFrame.Ss = dataSegment;

    thread->CPUFrame.Eax = 0;
    thread->CPUFrame.Ebx = 0;
    thread->CPUFrame.Ecx = 0;
    thread->CPUFrame.Edx = 0;
    thread->CPUFrame.Esi = 0;
    thread->CPUFrame.Edi = 0;

    thread->CPUFrame.Ebp = (uint32_t) stack;
    thread->CPUFrame.Esp = (uint32_t) stack;

    thread->CPUFrame.Eip = (uint32_t) eip;
    thread->CPUFrame.Eflags = 0x200;

    element->ListElement.Prev = NULL;
    element->ListElement.Next = NULL;
    element->Thread = thread;

    if (owner->Threads == NULL)
        owner->Threads = element;
    else
        LListPushBackC(owner->Threads, element);

    return thread;
}


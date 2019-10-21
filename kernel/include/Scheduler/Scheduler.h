#pragma once

#include <McpgOS.h>

#define STACK_ALLOCATE(size) \
    (((uint32_t) (malloc(size))) + size)

typedef enum
{
    TASK_STATE_RUNNING,
    TASK_STATE_SLEEPING
} SchedTaskState;

struct Process;
typedef struct Process Process;

typedef struct
{
    Process* ParentProcess;
    char* ThreadName;

    SchedTaskState State;
    uint32_t RemainingCpuTime;

    struct
    {
        uint32_t Gs;
        uint32_t Fs;
        uint32_t Ss;
        uint32_t Es;
        uint32_t Ds;
        uint32_t Cs;

        uint32_t Edi;
        uint32_t Esi;
        uint32_t Ebp;
        uint32_t Esp;
        uint32_t Ebx;
        uint32_t Edx;
        uint32_t Ecx;
        uint32_t Eax;

        uint32_t Eip;
        uint32_t Eflags;
    } CPUFrame;
} SchedThread;

typedef struct
{
    LListElement ListElement;
    SchedThread* Thread;
} ProcThreadElement;

struct Process
{
    uint32_t ProcessId;
    uint32_t UserId;
    bool IsKernelMode;

    PageDirectoryEntry* PageDirectory;
    ProcThreadElement* Threads;
};

typedef struct
{
    LListElement ListElement;
    Process* Process;
} ProcListElement;

extern uint64_t SchedTickCounter;
extern ProcListElement* SchedProcessList;

void SchedInit();
void SchedSleep(SchedThread* thread, uint32_t millis);

Process* SchedCreateProcess(
    bool kernelMode, bool allocPageDir,
    uint32_t uid
);

SchedThread* SchedCreateThread(
    Process* owner, char* name,
    void* eip, void* stack
);


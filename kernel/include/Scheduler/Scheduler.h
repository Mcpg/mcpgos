#pragma once

#include <McpgOS.h>

typedef enum
{
    TASK_STATE_RUNNING,
    TASK_STATE_SLEEPING
} SchedTaskState;

typedef struct SchedTask SchedTask;
struct SchedTask
{
    bool IsKernelTask;

    SchedTaskState TaskState;
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

    union
    {
        struct
        {
            char* TaskName;
        } KernelTaskInfo;
        struct
        {
            Process* ParentProcess;
        } UserTaskInfo;
    };
};

typedef struct
{
    LListElement ListElement;
    SchedTask* Task;
} SchedKernelTaskElement;

extern SchedKernelTaskElement* SchedKernelTasks;
extern SchedTask* SchedCurrentTask;
extern ProcessListElement* SchedCurrentProcess; // may be NULL

extern uint64_t SchedTickCounter;

void SchedInit();
void SchedSleep(SchedTask* task, uint32_t millis);

SchedTask* SchedCreateUserTask(Process* owner, void* func, uint32_t stackPtr);
SchedTask* SchedCreateKernelTask(char* name, void* func, size_t stackSize);


#pragma once

#include <stdbool.h>

typedef struct SchedTask SchedTask;
struct SchedTask
{
    uint32_t TaskID;
    bool IsKernelTask;

    SchedTask* Next;
    SchedTask* Prev;

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
            void* ParentProcess; // TODO: change the pointer type to process struct pointer type
        } UserTaskInfo;
    };
};


extern uint32_t SchedTaskCount;
extern SchedTask* SchedIdleTask;
extern SchedTask* SchedCurrentTask;

extern uint64_t SchedTickCounter;

void SchedInit();
void SchedEnable();
SchedTask* SchedGetTask(int taskID);
int SchedAddTask(SchedTask* task);
int SchedRemoveTask(int taskID);
int SchedCreateKernelTask(char* name, void* func, size_t stackSize);

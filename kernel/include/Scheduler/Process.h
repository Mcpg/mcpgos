#pragma once

#include <McpgOS.h>

struct SchedTask;

typedef struct
{
    LListElement ListElement;
    struct SchedTask* Task;
} ProcessThreadElement;

typedef struct
{
    PageDirectoryEntry* PageDirectory;
    bool IsKernelMode;
    ProcessThreadElement* Threads;
    uint32_t UserId;
    uint32_t ProcessId;
} Process;

typedef struct
{
    LListElement ListElement;
    Process* Process;
} ProcessListElement;

extern ProcessListElement* ProcessList;

Process* ProcessCreate(bool isKernelMode, uint32_t uid);

#include <McpgOS.h>

ProcessListElement* ProcessList = NULL;

Process* ProcessCreate(bool isKernelMode, uint32_t uid)
{
    static uint32_t pidCounter = 0;

    ProcessListElement* element;

    Process* result = (Process*) malloc(sizeof(Process));
    if (result == NULL)
        return NULL;

    result->PageDirectory = MmAllocPageDirectory();
    if (result->PageDirectory == NULL)
    {
        free(result);
        return NULL;
    }

    result->IsKernelMode = isKernelMode;
    result->UserId = uid;
    result->ProcessId = pidCounter++;
    result->Threads = NULL;

    element = (ProcessListElement*) malloc(sizeof(ProcessListElement));
    KAssert(element != NULL);

    element->ListElement.Next = NULL;
    element->ListElement.Prev = NULL;
    element->Process = result;

    if (ProcessList == NULL)
        ProcessList = element;
    else
        LListPushBackC(ProcessList, element);

    return result;
}

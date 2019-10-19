#include <McpgOS.h>

LListElement* LListGet(LListElement* list, uint32_t index)
{
    uint32_t i;

    KAssert(list != NULL);
    KAssert(index < 0xFFFFFFFF);

    list = LListGetFirst(list);
    for (i = 0; i < index; i++)
    {
        list = list->Next;
        if (list == NULL)
            return NULL;
    }

    return list;
}

LListElement* LListGetFirst(LListElement* list)
{
    KAssert(list != NULL);

    while (list->Prev != NULL)
        list = list->Prev;
    return list;
}

LListElement* LListGetLast(LListElement* list)
{
    KAssert(list != NULL);

    while (list->Next != NULL)
        list = list->Next;
    return list;
}

void LListPushFront(LListElement* list, LListElement* elementToAdd)
{
    KAssert(list != NULL);
    KAssert(elementToAdd != NULL);

    list = LListGetFirst(list);

    elementToAdd->Next = list;
    elementToAdd->Prev = NULL;
    list->Prev = elementToAdd;
}

void LListPushBack(LListElement* list, LListElement* elementToAdd)
{
    KAssert(list != NULL);
    KAssert(elementToAdd != NULL);

    list = LListGetLast(list);

    elementToAdd->Next = NULL;
    elementToAdd->Prev = list;
    list->Next = elementToAdd;
}

LListElement* LListPopFront(LListElement* list)
{
    KAssert(list != NULL);

    list = LListGetFirst(list);
    list->Next->Prev = NULL;
    return list;
}

LListElement* LListPopBack(LListElement* list)
{
    KAssert(list != NULL);

    list = LListGetLast(list);
    list->Prev->Next = NULL;
    return list;
}

uint32_t LListGetSize(LListElement* list)
{
    uint32_t result;

    KAssert(list != NULL);

    for (result = 1; list->Next != NULL; list = list->Next)
        result++;

    return result;
}

#pragma once

#include <McpgOS.h>

typedef struct LListElement LListElement;
struct LListElement
{
    LListElement* Next;
    LListElement* Prev;
};

LListElement* LListGet(LListElement* list, uint32_t index);
LListElement* LListGetFirst(LListElement* list);
LListElement* LListGetLast(LListElement* list);
void LListPushFront(LListElement* list, LListElement* elementToAdd);
void LListPushBack(LListElement* list, LListElement* elementToAdd);
LListElement* LListPopFront(LListElement* list);
LListElement* LListPopBack(LListElement* list);
uint32_t LListGetSize(LListElement* list);

// Cast versions of the functions above
#define LListGetC(list, index) \
    LListGet((LListElement*) (list), (index))

#define LListGetFirstC(list) \
    LListGetFirst((LListElement*) (list))

#define LListGetLastC(list) \
    LListGetLast((LListElement*) (list))

#define LListPushFrontC(list, element) \
    LListPushFront((LListElement*) (list), (LListElement*) (element))

#define LListPushBackC(list, element) \
    LListPushBack((LListElement*) (list), (LListElement*) (element))

#define LListPopFrontC(list) \
    LListPopFront((LListElement*) (list))

#define LListPopBackC(list) \
    LListPopBack((LListElement*) (list))

#define LListGetSizeC(list) \
    LListGetSize((LListElement*) (list))

#pragma once

#include <McpgOS.h>

typedef IoStream* (*DevfsDevOpenReq)();

typedef struct
{
    uint32_t (*GetSizeCallback)();
    IoStream* (*FileOpenCallback)();
} DevfsOperations;

typedef struct
{
    char* Name;
    DevfsOperations* Operations;
} DevfsDevice;

void DevfsInit();
void DevfsAddDevice(DevfsDevice* device);

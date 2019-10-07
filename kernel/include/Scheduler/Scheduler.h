#pragma once

#include <stdbool.h>

typedef struct
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
} SchedCPUFrame;

typedef struct
{
    SchedCPUFrame CPUFrame;

} SchedTask;

void SchedInit();

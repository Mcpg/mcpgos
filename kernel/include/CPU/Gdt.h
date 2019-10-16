#pragma once

typedef enum
{
    GdtKernelCode = 0x08,
    GdtKernelData = 0x10,
    GdtUserCode   = 0x18,
    GdtUserData   = 0x20,
    GdtTss        = 0x28
} GdtSegment;

void GdtInit();
void TssSetEsp(uintptr_t newEsp);

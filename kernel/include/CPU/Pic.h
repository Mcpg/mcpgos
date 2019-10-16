#pragma once

#include <McpgOS.h>

extern const uint8_t PicIrqOffset;

void PicInit();
void PicEOI(uint8_t irq);

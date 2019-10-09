#pragma once

#include <McpgOS.h>

#define printf Kprintf

extern IOStream* KprintfStream;
void Kprintf(char* msg, ...);

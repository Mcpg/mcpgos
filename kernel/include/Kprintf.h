#pragma once

#include <McpgOS.h>

#define printf Kprintf

extern IoStream* KprintfStream;
void Kprintf(char* msg, ...);

#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include <stddef.h>

void* memset(void* ptr, int val, size_t n);
void* memcpy(void* dest, void* src, size_t n);
size_t strlen(char* str);
int strcmp(char* s1, char* s2);

#ifdef __cplusplus
}
#endif

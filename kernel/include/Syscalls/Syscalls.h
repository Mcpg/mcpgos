#pragma once

#include <McpgOS.h>

//
// Quick explanation of the magic of McpgOS' syscalls.
//
// Called via int 0x83
// The kernel uses eax as the syscall ID register and
// ebx, ecx, edx, esi and edi as potential parameters.
// Value returned from the syscall can only be a number,
// moved into eax.
//
// if a syscall has a function:
// int SyscallWrite(uint32_t fd, void* buffer, uint32_t len)
//
// eax must contain the ID for write syscall
// ebx must contain fd
// ecx must contain buffer pointer
// edx must contain the length
//
// value returned by this function is moved into eax.
//

typedef uint32_t (*SyscallHandler)(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);

#define SYSCALL_HANDLER_AMOUNT 1
extern SyscallHandler SyscallHandlers[SYSCALL_HANDLER_AMOUNT];

int SyscallTest(uint32_t n);

void SyscallInit();

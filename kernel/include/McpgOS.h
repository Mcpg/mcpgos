#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

void KPanic(char* reason);
#define KAssert(...)                              \
    if (!((int) (__VA_ARGS__)))                   \
        KPanic("Assertion failed: " #__VA_ARGS__ "\n " __FILE__ ":" __STR(__LINE__))

#define __STR_HELPER(x) #x
#define __STR(x) __STR_HELPER(x)
#define UNIMPLEMENTED() \
    KPanic("Reached unimplemented: " __FILE__ ":" __STR(__LINE__))

#define CLI() asm volatile("cli")
#define HALT() asm volatile("hlt")
#define STI() asm volatile("sti")

#include <Cpu/Gdt.h>
#include <Cpu/Idt.h>
#include <Cpu/Io.h>
#include <Cpu/Pic.h>
#include <Cpu/Pit.h>

#include <Utils/LinkedList.h>
#include <IoStream.h>
#include <Kprintf.h>

#include <Mm/VirtualMemory.h>
#include <Mm/PhysicalMemory.h>
#include <Mm/Paging.h>
#include <Mm/LibAlloc.h>
#define free(...) ;
// TODO: fix memory freeing

#include <Drivers/DriverManager.h>

#include <Scheduler/Scheduler.h>

#include <Drivers/TTY.h>
#include <Drivers/RamDisk.h>
#include <Drivers/PS2Keyboard.h>

#include <Syscalls/Syscalls.h>

// Linker-defined symbols
extern void* _KernelStart;
extern void* _KernelEnd;
extern void* _KernelSize;
extern void* _KernelSizePages;

extern void* _VirtualAddress;
extern void* _PhysicalAddress;

// Defined in _start.asm
extern void* StackTop;

// Linker symbols defined in a more accessible form
extern void* KernelStart;
extern void* KernelEnd;
extern uint32_t KernelSize;
extern uint32_t KernelSizePages;

extern uintptr_t VirtualAddress;
extern uintptr_t PhysicalAddress;

/* Up to 4K of multiboot tags can be stored here. Just so memory management
   is a little bit easier (mboot tags don't have to be taken into
   consideration when generating an initial page directory).
   
   If the tags take more than 4K of space, then the kernel just hangs. */
extern uint8_t MultibootTags[4096];

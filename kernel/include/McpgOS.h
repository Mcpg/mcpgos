#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

// TODO: definition of system functions
#define KPanic(...)
#define KAssert(...)

#include <CPU/Gdt.h>
#include <CPU/Idt.h>
#include <CPU/IO.h>
#include <CPU/Pic.h>

#include <MM/VirtualMemory.h>
#include <MM/PhysicalMemory.h>
#include <MM/Paging.h>
//#include <MM/LibAlloc.h>

/* Linker-defined symbols */
extern void* KernelStart;
extern void* KernelEnd;
extern uint32_t KernelSize;
extern uint32_t KernelSizePages;

extern uint32_t VirtualAddress;
extern uint32_t PhysicalAddress;

/* Up to 4K of multiboot tags can be stored here. Just so memory management
   is a little bit easier (mboot tags don't have to be taken into
   consideration when generating an initial page directory).
   
   If the tags take more than 4K of space, then the kernel just hangs. */
extern uint8_t MultibootTags[4096];

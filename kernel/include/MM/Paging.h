#pragma once

#include <stdint.h>
#include <stdbool.h>

#define K_VIRT_TO_PHYS(virt) (((uint32_t) virt) - 0xE0000000 + 0x100000)
#define K_PHYS_TO_VIRT(phys) (((uint32_t) phys) + 0xE0000000 - 0x100000)

typedef union PageTableEntry
{
    struct
    {
        uint32_t Present      : 1;
        uint32_t Writable     : 1;
        uint32_t User         : 1;
        uint32_t WriteThrough : 1;
        uint32_t Cached       : 1;
        uint32_t Accessed     : 1;
        uint32_t Dirty        : 1;
        uint32_t Reserved     : 1;
        uint32_t Global       : 1;
        uint32_t Available    : 3;
        uint32_t PhysAddress  : 20;
    };
    uint32_t Raw;
} PageTableEntry;

typedef union PageDirectoryEntry
{
    struct
    {
        uint32_t Present      : 1;
        uint32_t Writable     : 1;
        uint32_t User         : 1;
        uint32_t WriteThrough : 1;
        uint32_t CacheDisable : 1;
        uint32_t Accessed     : 1;
        uint32_t Reserved     : 1;
        uint32_t PageSize     : 1;
        uint32_t Reserved2    : 1;
        uint32_t Available    : 3;
        uint32_t PhysAddress  : 20;
    };
    uint32_t Raw;
} PageDirectoryEntry;

extern uint32_t CurrentPageDirectoryPhys;
extern PageDirectoryEntry* CurrentPageDirectory;
void SwitchPageDirectory(PageDirectoryEntry* pd);

PageDirectoryEntry* MmAllocPageDirectory();

PageTableEntry* MmGetTable(PageDirectoryEntry* pde, int index);
PageTableEntry* MmGetTableAddr(PageDirectoryEntry* pde, uintptr_t virt);
PageTableEntry* MmGetTableEntry(PageDirectoryEntry* pde, uintptr_t virt);

uint32_t MmVirtToPhys(PageDirectoryEntry* pde, uintptr_t virt);

//
// Converts a physical address to a virtual one, as long as it's mapped
// to the kernel space.
//
void* MmKernelPhysToVirt(uint32_t phys);

//
// Maps a specified amount of pages starting at ptr in current
// paging directory.
//
// If not NULL, ptr must be page aligned.
// If NULL, the kernel will find the address itself.
//
// Note, that the search is done on the userspace (above 4 MiB
// and below 0xE0000000). For kernel space memory mapping use
// MmVm* functions.
//
// MmMmap returns NULL on failure or the base of the allocated
// space.
//
void* MmMmap(void* ptr, uint32_t pages, bool user, bool writable);

bool MmIsPresent(uintptr_t virt);
bool MmIsWritable(uintptr_t virt);
bool MmIsUser(uintptr_t virt);

bool MmMap(uint32_t phys, uintptr_t virt, bool user, bool writable);
bool MmUnmap(uintptr_t virt);

// 512 kB in size, created in _start.asm, maps the entire kernel space
extern PageTableEntry KernelPageTables[131072];

// First 4 MiB of address space, identity mapped for kernel only
extern PageTableEntry ZeroIdentityPage[1024];

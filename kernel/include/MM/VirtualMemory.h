#pragma once

#include <stdint.h>

void MmVmInitAllocator();

/**
 * Finds and allocates a block of N pages in kernel space. If the block
 * cannot be found, 0 is returned.
 *
 * @param pages how many pages should be allocated
 * @return page-aligned virtual address of the block's start. nullptr
 *         is returned on failure.
 */
uint32_t MmVmAllocate(uint32_t pages);
void MmVmFree(uintptr_t virt, uint32_t pages);

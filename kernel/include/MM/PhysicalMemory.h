#pragma once

#include <stdint.h>

void MmPmInitAllocator();
    
/**
 * Finds a free physical page and marks it as used.
 *
 * @return Page-aligned address of the page. 0 is returned on failure.
 */
uint32_t MmPmAllocate();
void MmPmFree(uint32_t phys);

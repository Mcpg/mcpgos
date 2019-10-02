#pragma once

#define GDT_KCODE 0x08
#define GDT_KDATA 0x10
#define GDT_UCODE 0x18
#define GDT_UDATA 0x20
#define GDT_TSS   0x28

/**
 * Initializes the Global Descriptor Table and sets all segment
 * registers to kernel segments (cs to code, the rest to data)
 */
void GdtInit();
        
/**
 * Sets the kernel esp field in the TSS
 * @param newEsp new esp field value
 */
void TssSetEsp(uintptr_t newEsp);

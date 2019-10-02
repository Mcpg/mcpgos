#pragma once

#include <stdint.h>

#define IRQ_OFFSET 0x20

/**
 * Initializes the 8259 PIC chips and remaps IRQs to interrupts
 * 0x20-0x2F
 */
void PicInit();

/**
 * Sends End-Of-Interrupt signal to PIC(s) based on the IRQ
 *
 * @param irq IRQ
 */
void PicEOI(uint8_t irq);

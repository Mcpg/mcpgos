#pragma once

#include <stdint.h>

static inline void Outb(uint16_t port, uint8_t val)
{
    asm volatile("outb %1, %0" : : "a" (val), "Nd" (port));
}

static inline void Outw(uint16_t port, uint16_t val)
{
    asm volatile("outw %1, %0" : : "a" (val), "Nd" (port));
}

static inline void Outl(uint16_t port, uint32_t val)
{
    asm volatile("outl %1, %0" : : "a" (val), "Nd" (port));
}

static inline uint8_t Inb(uint16_t port)
{
    uint8_t ret;
    asm volatile("inb %0, %1" : "=a" (ret) : "Nd" (port));
    return ret;
}

static inline uint16_t Inw(uint16_t port)
{
    uint16_t ret;
    asm volatile("inw %0, %1" : "=a" (ret) : "Nd" (port));
    return ret;
}

static inline uint32_t Inl(uint16_t port)
{
    uint32_t ret;
    asm volatile("inl %0, %1" : "=a" (ret) : "Nd" (port));
    return ret;
}

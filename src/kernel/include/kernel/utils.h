#ifndef _KERNEL_UTIL_H
#define _KERNEL_UTIL_H

#if !defined(__cpluscplus)
#include <stdbool.h>
#endif
#include <stddef.h>
#include <stdint.h>

void outb(uint16_t port, uint8_t value);

void outw(uint16_t port, uint16_t value);

void outl(uint16_t port, uint32_t value);

uint8_t inb(uint16_t port);

uint16_t inw(uint16_t port);

uint32_t inl(uint16_t port);

void io_wait();

uint8_t one_count(uint32_t value);
#endif
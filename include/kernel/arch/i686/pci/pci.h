#ifndef _KERNEL_UTIL_H
#define _KERNEL_UTIL_H 1

#include <stddef.h>
#include <stdint.h>

uint16_t pci_read_config(uint8_t bus, uint8_t slot, 
						 uint8_t func, uint8_t offset);

#endif
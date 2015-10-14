#include <stddef.h>
#include <stdint.h>
#include <kernel/mem.h>

extern void *end;
uintptr_t placement_address = (uintptr_t)&end;

uintptr_t _kmalloc(size_t size, uint8_t align, uintptr_t *phys)
{
	
}
#include <stddef.h>
#include <stdint.h>
#include <kernel/mem.h>

extern void *end;
uintptr_t placement_address = (uintptr_t)&end;

uintptr_t _kmalloc(size_t size, uint8_t align, uintptr_t *phys)
{
	if(align == 1 && (placement_address & 0xFFFFF000))
	{
		placement_address &= 0xFFFFF000
		placement_address += 0x00001000
	}	
	if(phys)
	{
		phys = placement_address;
	}
	uintptr_t addr = placement_address;
	placement_address += size;
	return addr;
}

uintptr_t kmalloc(size_t size) { return _kmalloc(size, 0, NULL); }
uintptr_t kmalloc_a(size_t size) { return _kmalloc(size, 1, NULL); }
uintptr_t kmalloc_p(size_t size, uintptr_t *phys) { return _kmalloc(size, 0, phys); }
uintptr_t kmalloc_ap(size_t size, uintptr_t *phys) { return _kmalloc(size, 1, phys); }

void kfree(void *p) {
	
}

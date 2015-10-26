#include <stddef.h>
#include <stdint.h>
#include <assert.h>
#include <kernel/mem.h>
#include <kernel/paging.h>
#include <kernel/tty.h>

extern void *end;
extern page_directory_t *kernel_directory;

uintptr_t placement_address = (uintptr_t)&end;
uintptr_t heap_end = (uintptr_t) NULL;

uintptr_t _kmalloc(size_t size, uint8_t align, uintptr_t *phys)
{

	if (heap_end)
    {
        void *addr = alloc(size, (uint8_t)align);
        if (phys)
        {
            *phys = map_to_physical((uintptr_t)addr)
        }
        return (uintptr_t)addr;
    }

	if(align && (placement_address & 0xFFFFF000))
	{
		placement_address &= 0xFFFFF000;
		placement_address += 0x1000;
	}	
	if(phys)
	{
		*phys = placement_address;
	}
	uintptr_t addr = placement_address;
	placement_address += size;
	return addr;
}

uintptr_t kmalloc(size_t size) { return _kmalloc(size, 0, NULL); }
uintptr_t kmalloc_a(size_t size) { return _kmalloc(size, 1, NULL); }
uintptr_t kmalloc_p(size_t size, uintptr_t *phys) { return _kmalloc(size, 0, phys); }
uintptr_t kmalloc_ap(size_t size, uintptr_t *phys) { return _kmalloc(size, 1, phys); }

void kfree(void *p) 
{ 
	free(p, kheap);
}

void *alloc(size_t size, uint8_t page_align)
{
	return (void *)NULL;
}
void free(void *p, heap_t *heap)
{}

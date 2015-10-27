#include <stddef.h>
#include <stdint.h>
#include <assert.h>
#include <kernel/mem.h>
#include <kernel/paging.h>
#include <kernel/tty.h>

extern void *end;
extern page_directory_t *kernel_directory;
extern page_directory_t *current_directory;
uintptr_t placement_address = (uintptr_t)&end;
uintptr_t heap_end = (uintptr_t) NULL;

uintptr_t _kmalloc(size_t size, uint8_t align, uintptr_t *phys)
{

#if 0
	if (heap_end)
    {
        void *addr = alloc(size, (uint8_t)align);
        if (phys)
        {
            *phys = map_to_physical((uintptr_t)addr);
        }
        return (uintptr_t)addr;
    }
#endif
    
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

uintptr_t map_to_physical(uintptr_t virt) 
{
	uintptr_t remaining = virt % 0x1000;
	uintptr_t frameidx = virt / 0x1000;
	uintptr_t tableidx = frameidx / 1024;
	frameidx &= 0x3ff;

	if(current_directory->tables[tableidx])
	{
		return current_directory->tables[tableidx]->pages[frameidx].frame * 0x1000 + remaining;		
	}
	else
	{
		return (uintptr_t) NULL;
	}
}
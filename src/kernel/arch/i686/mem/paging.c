#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <kernel/alloc.h>
#include <kernel/mem.h>
#include <kernel/paging.h>
#include <kernel/rwcr.h>
#include <kernel/panic.h>
#include <kernel/tty.h>

page_directory_t *kernel_directory = NULL;
page_directory_t *current_directory=0;

uint32_t *frames;
uint32_t nframes;

extern uintptr_t placement_address;
extern heap_t *kernel_heap;

#define INDEX_FROM_BIT(a) ((a) / 0x20)
#define OFFSET_FROM_BIT(a) ((a) % 0x20)

static void set_frame(uintptr_t addr)
{
	if(addr < nframes * 0x1000)
	{
		uint32_t frame = addr/0x1000;
		uint32_t idx = INDEX_FROM_BIT(frame);
		uint32_t off = OFFSET_FROM_BIT(frame);
		frames[idx] |= (0x1 << off);
	}
}

static void clear_frame(uintptr_t addr)
{
	uintptr_t frame = addr/0x1000;
	uint32_t idx = INDEX_FROM_BIT(frame);
	uint32_t off = OFFSET_FROM_BIT(frame);
	frames[idx] &= ~(0x1 << off);
}

static uint32_t test_frame(uintptr_t addr)
{
	uintptr_t frame = addr/0x1000;
	uint32_t idx = INDEX_FROM_BIT(frame);
	uint32_t off = OFFSET_FROM_BIT(frame);
	return (frames[idx] & (0x1 << off));	
}

static uint32_t first_frame()
{
	for (uint32_t i = 0; i < INDEX_FROM_BIT(nframes); i++)
	{
		if (frames[i] != 0xFFFFFFFF)
		{
			// at least one bit is free here.
			for (uint32_t j = 0; j < 0x20; j++)
			{
				if ( !(frames[i] & (0x1 << j)) )
				{
					return i*0x20+j;
				}
			}
		}
	}
	return 0xFFFFFFFF;
}

void alloc_frame(page_entry_t *page, int32_t is_kernel, int32_t is_writeable)
{
	if (page->frame != 0)
	{
		page->present = 1;
		page->rw	  = (is_writeable) ? 1 : 0;
		page->user	= (is_kernel)	? 0 : 1;
		return;
	}
	else
	{
		uint32_t idx = first_frame();
		if (idx == (uint32_t)-1)
		{
			PANIC("No free frame");
		}
		set_frame(idx*0x1000);
		page->frame = idx;
		page->present = 1;
		page->rw = (is_writeable) ? 1 : 0;
		page->user = (is_kernel) ? 0 : 1;
		page->frame = idx;
	}
}

void direct_frame(page_entry_t * page, int32_t is_kernel, int32_t is_writeable, uintptr_t addr)
{
	page->present = 1;
	page->rw = (is_writeable) ? 1 : 0;
	page->user = (is_kernel) ? 0 : 1;
	page->frame = addr / 0x1000;

	set_frame(addr);
}

void free_frame(page_entry_t *page)
{
	uint32_t frame;
	if (!(frame=page->frame))
	{
		return;
	}
	else
	{
		clear_frame(frame);
		page->frame = 0x0;
	}
}

void initialize_paging(uint32_t memsize)
{
	nframes = memsize / 4;
	frames = (uint32_t *)kmalloc(INDEX_FROM_BIT(nframes));
	uintptr_t pg;

	assert(frames != NULL);

	memset(frames, 0, INDEX_FROM_BIT(nframes));

	uintptr_t physical;
	kernel_directory = (page_directory_t *)kmalloc_ap(sizeof(page_directory_t), &physical);
	memset(kernel_directory, 0, sizeof(page_directory_t));
	current_directory = kernel_directory;

#if 1
	get_page(0,1,kernel_directory)->present = 0;
	set_frame(0);

	for(uintptr_t i = 0x1000; i < placement_address+0x3000; i += 0x1000)
#else
	for(uintptr_t i = 0x0; i < placement_address+0x3000; i += 0x1000)
#endif
	{
		direct_frame( get_page(i, 1, kernel_directory), 1, 0, i);
	}
	
	kernel_directory->physical_addr = (uintptr_t)kernel_directory->tables_physical;

	uintptr_t heap_start = KERNEL_HEAP_START;

	if(heap_start <= placement_address + 0x3000)
	{
		heap_start = placement_address + 0x100000;
	}
	
	for (uintptr_t i = placement_address + 0x3000; i < heap_start; i += 0x1000)
	{
		alloc_frame(get_page(i, 1, kernel_directory), 1, 0);
	}

	for(uintptr_t i = heap_start; i < heap_start + KERNEL_HEAP_INIT; i += 0x1000)
	{
		get_page(i, 1, kernel_directory);
	}

	for(uintptr_t i = heap_start; i < heap_start + KERNEL_HEAP_INIT; i += 0x1000)
	{
		alloc_frame(get_page(i, 1, kernel_directory), 0, 0);
	}
	
	register_isr_handler(13, general_protection_fault);
	register_isr_handler(14, page_fault);	

	switch_page_directory(kernel_directory);

	kernel_heap = create_heap(heap_start, heap_start + KERNEL_HEAP_INIT, KERNEL_HEAP_END, 0, 0);
	//kernel_heap = create_heap(heap_start, KERNEL_HEAP_END, KERNEL_HEAP_END, 0, 0);
}

void switch_page_directory(page_directory_t *dir)
{
	write_cr3((uint32_t)&dir->tables_physical);
	write_cr4(read_cr4() | 0x00000010); // Set PSE;
	write_cr0(read_cr0() | 0x80000000); // Set PG;
}

page_entry_t *get_page(uint32_t address, int make, page_directory_t *dir)
{
	address /= 0x1000;
	uint32_t table_idx = address / 1024;
	if(dir->tables[table_idx])
	{
		return &dir->tables[table_idx]->pages[address % 1024];
	}
	else if(make)
	{
		uint32_t tmp;
		dir->tables[table_idx] = (page_table_t*)kmalloc_ap(sizeof(page_table_t), (uintptr_t*)(&tmp));
		memset(dir->tables[table_idx], 0, sizeof(page_table_t));
		dir->tables_physical[table_idx] = tmp | 0x7;
		return &dir->tables[table_idx]->pages[address % 1024];
	}
	else
	{
		return NULL;
	}
}
void *general_protection_fault(registers_t *regs)
{
	uint32_t err_code = regs->err_code;
	terminal_writestring("Gen Protection fault");
	if(err_code)
	{
		int8_t external = err_code & 0x1;
		int8_t table = (err_code & 0x6) >> 1;
		int16_t index = (err_code & 0xF8) >> 3;
		if(external) {terminal_writestring("of external origin"); }
		terminal_writestring("!");
		if(table & 0x1) 		{ terminal_writestring("From IDT @ index 0x"); }
		else if(table & 0x2)	{ terminal_writestring("From LDT @ index 0x"); }
		else 					{ terminal_writestring("From GDT @ index 0x"); }
		terminal_writehex((uint32_t)index);
		terminal_writestring("\n");
	}
	else 
	{
		terminal_writestring("!\n");
	}
	PANIC("General protection fault");
	return NULL;
}

void *page_fault(registers_t *regs)
{
	uint32_t err_code = regs->err_code;
	int32_t present   = !(err_code & 0x1); // Page not present
	int32_t rw = err_code & 0x2;		   // Write operation?
	int32_t us = err_code & 0x4;		   // Processor was in user-mode?
	int32_t reserved = err_code & 0x8;	 // Overwritten CPU-reserved bits of page entry?
	int32_t id = err_code & 0x10;		  // Caused by an instruction fetch?
	terminal_writestring("Page fault! ( ");
	if (present)	terminal_writestring("present ");
	if (rw)			terminal_writestring("read-only ");
	if (us)			terminal_writestring("user-mode ");
	if (reserved)   terminal_writestring("reserved ");
	terminal_writestring(") at 0x");
	terminal_writehex((uint32_t)read_cr2());
	terminal_writestring("\n");
	PANIC("Page fault");
	return NULL;
}


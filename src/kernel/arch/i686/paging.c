#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <kernel/mem.h>
#include <kernel/paging.h>
#include <kernel/panic.h>
#include <kernel/tty.h>

page_directory_t *kernel_directory = NULL;

page_directory_t *current_directory=0;
uint32_t *frames;
uint32_t nframes;

extern uintptr_t placement_address;
extern heap_t* kheap;

#define INDEX_FROM_BIT(a) ((a) / 32)
#define OFFSET_FROM_BIT(a) ((a) % 32)

static void set_frame(uintptr_t addr)
{
	uintptr_t frame = addr/0x1000;
	uint32_t idx = INDEX_FROM_BIT(frame);
	uint32_t off = OFFSET_FROM_BIT(frame);
	frames[idx] |= (0x1 << off);
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
    uint32_t i, j;
    for (i = 0; i < INDEX_FROM_BIT(nframes); i++)
    {
        if (frames[i] != 0xFFFFFFFF)
        {
            // at least one bit is free here.
            for (j = 0; j < 32; j++)
            {
                if ( !(frames[i] & 0x1 << j) )
                {
                    return i*32+j;
                }
            }
        }
    }
}

void alloc_frame(page_t *page, int32_t is_kernel, int32_t is_writeable)
{
    if (page->frame != 0)
    {
        return;
    }
    else
    {
        uint32_t idx = first_frame();
        if (idx == (uint32_t)-1)
        {
            PANIC("No free frame!");
        }
        set_frame(idx*0x1000);
        page->present = 1;
        page->rw = (is_writeable)?1:0;
        page->user = (is_kernel)?0:1;
        page->frame = idx;
    }
}

void free_frame(page_t *page)
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

void initialize_paging()
{
    uintptr_t mem_end_page = 0x1000000;
    
    nframes = mem_end_page / 0x1000;
    frames = (uintptr_t*)kmalloc(INDEX_FROM_BIT(nframes));
    memset(frames, 0, INDEX_FROM_BIT(nframes));
    
    kernel_directory = (page_directory_t*)kmalloc_a(sizeof(page_directory_t));
    memset(kernel_directory, 0, sizeof(page_directory_t));
    current_directory = kernel_directory;

    int i = 0;
    for (i = KERNEL_HEAP_START; i < KERNEL_HEAP_START + KERNEL_HEAP_INIT; i += 0x1000)
        get_page(i, 1, kernel_directory);

    i = 0;
    while (i < placement_address+0x1000)
    {
        alloc_frame( get_page(i, 1, kernel_directory), 0, 0);
        i += 0x1000;
    }

    for (i = KERNEL_HEAP_START; i < KERNEL_HEAP_START + KERNEL_HEAP_INIT; i += 0x1000)
        alloc_frame( get_page(i, 1, kernel_directory), 0, 0);

    terminal_writestring("initializing Paging...\n-- Registering Page Fault Handler...\t");
    register_int_err_handler(14, &page_fault);
    
    terminal_writestring("[DONE]\n-- Switching To Kernel Page Directory...\t");
    switch_page_directory(kernel_directory);

    terminal_writestring("[DONE]\n-- Creating Kernel Heap...\t");
    kheap = create_heap(KERNEL_HEAP_START, KERNEL_HEAP_START + KERNEL_HEAP_INIT, 0x0020000, 0, 0);
    terminal_writestring("[DONE]\n[DONE]");
    
}

void switch_page_directory(page_directory_t *dir)
{
    current_directory = dir;
    __asm__ __volatile__ ("mov %0, %%cr3":: "r"(&dir->tables_physical));
    uint32_t cr0;
    __asm__ __volatile__ ("mov %%cr0, %0": "=r"(cr0));
    cr0 |= 0x80000000; // Enable paging!
    __asm__ __volatile__ ("mov %0, %%cr0":: "r"(cr0));
}

page_t *get_page(uint32_t address, int make, page_directory_t *dir)
{
    address /= 0x1000;
    uint32_t table_idx = address / 1024;

    if (dir->tables[table_idx]) // If this table is already assigned
    {
        return &dir->tables[table_idx]->pages[address%1024];
    }
    else if(make)
    {
        uint32_t tmp;
        dir->tables[table_idx] = (page_table_t*)kmalloc_ap(sizeof(page_table_t), &tmp);
        memset(dir->tables[table_idx], 0, 0x1000);
        dir->tables_physical[table_idx] = tmp | 0x7; // PRESENT, RW, US.
        return &dir->tables[table_idx]->pages[address%1024];
    }
    else
    {
        return 0;
    }
}


void *page_fault(void *ctx, uint32_t err_code)
{
	uintptr_t faulting_address;
    __asm__ __volatile__ ("mov %%cr2, %0" : "=r" (faulting_address));
    
    int32_t present   = !(err_code & 0x1); // Page not present
    int32_t rw = err_code & 0x2;           // Write operation?
    int32_t us = err_code & 0x4;           // Processor was in user-mode?
    int32_t reserved = err_code & 0x8;     // Overwritten CPU-reserved bits of page entry?
    int32_t id = err_code & 0x10;          // Caused by an instruction fetch?

    PANIC("Page fault");
    return NULL;
}

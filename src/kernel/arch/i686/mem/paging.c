#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <kernel/mem.h>
#include <kernel/rwcr.h>
#include <kernel/paging.h>
#include <kernel/panic.h>
#include <kernel/tty.h>

page_directory_t *kernel_directory = NULL;
page_directory_t *current_directory=0;

uint32_t *frames;
uint32_t nframes;

extern uintptr_t placement_address;
extern uintptr_t heap_end;

#define INDEX_FROM_BIT(a) ((a) / 0x20)
#define OFFSET_FROM_BIT(a) ((a) % 0x20)

static void set_frame(uintptr_t addr)
{
	if(addr < nframes * 0x1000)
    {
        uintptr_t frame = addr/0x1000;
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
        page->present = 1;
        page->rw = (is_writeable) ? 1 : 0;
        page->user = (is_kernel) ? 0 : 1;
        page->frame = idx;
    }
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
    frames = (uint32_t *)kmalloc(INDEX_FROM_BIT(nframes * 8));
    memset(frames, 0, INDEX_FROM_BIT(nframes * 8));

    uintptr_t physical;
    kernel_directory = (page_directory_t *)kmalloc_ap(sizeof(page_directory_t), &physical);
    memset(kernel_directory, 0, sizeof(page_directory_t));
}

void switch_page_directory(page_directory_t *dir)
{
    current_directory = dir;
    write_cr3((uint32_t)&dir->tables_physical);
    write_cr0(read_cr0() | 0x80000000); // Enable paging!
}

page_entry_t *get_page(uint32_t address, int make, page_directory_t *dir)
{
    address /= 0x1000;
    uint32_t table_idx = address / 1024;
    if(dir->tables[table_idx])
    {
        return &dir->tables[table_idx]->pages[address&0x03ff];
    }
    else if(make)
    {
        uint32_t tmp;
        dir->tables[table_idx] = (page_table_t*)kmalloc_ap(sizeof(page_table_t), (uintptr_t*)(&tmp));
        memset(dir->tables[table_idx], 0, sizeof(page_table_t));
        dir->tables_physical[table_idx] = tmp | 0x7;
        return &dir->tables[table_idx]->pages[address&0x03ff];
    }
    else
    {
        return NULL;
    }
}


void *page_fault(void *ctx, uint32_t err_code)
{
	int32_t present   = !(err_code & 0x1); // Page not present
    int32_t rw = err_code & 0x2;           // Write operation?
    int32_t us = err_code & 0x4;           // Processor was in user-mode?
    int32_t reserved = err_code & 0x8;     // Overwritten CPU-reserved bits of page entry?
    int32_t id = err_code & 0x10;          // Caused by an instruction fetch?
    terminal_writestring("Page fault! ( ");
    if (present)    terminal_writestring("present ");
    if (rw)         terminal_writestring("read-only ");
    if (us)         terminal_writestring("user-mode ");
    if (reserved)   terminal_writestring("reserved ");
    terminal_writestring(") at 0x");
    terminal_writehex((uint32_t)read_cr2());
    terminal_writestring("\n");
    PANIC("Page fault");
    return NULL;
}


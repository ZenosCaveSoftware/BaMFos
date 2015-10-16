#ifndef _KERNEL_PAGING_H
#define _KERNEL_PAGING_H 1

#include <kernel/isr.h>

typedef struct page_t 
{
	uint32_t present	: 1;
	uint32_t rw			: 1;
	uint32_t user		: 1;
	uint32_t accessed	: 1;
	uint32_t dirty		: 1;
	uint32_t unused		: 7;
	uint32_t frame		: 20;
	
} page_t;

typedef struct page_table
{
	page_t pages[1024];
} page_table_t;

typedef struct page_directory
{
	uintptr_t tables_physical[1024];
	page_table_t *tables[1024];
	uintptr_t physical_addr;
	
} page_directory_t;

void alloc_frame(page_t *page, int32_t is_kernel, int32_t is_writeable);
void free_frame(page_t *page);

void initialize_paging();
void switch_page_directory(page_directory_t *new_dir);
page_t *get_page(uintptr_t address, int32_t make, page_directory_t *dir);
void *page_fault(void *ctx, uint32_t err_code);

#endif
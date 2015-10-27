#ifndef _KERNEL_MEM_H
#define _KERNEL_MEM_H value

#define KERNEL_HEAP_START	0x00800000
#define KERNEL_HEAP_INIT	0x00100000
#define KERNEL_HEAP_MAGIC 	0xDEADFADE
#define KERNEL_HEAP_INDEX	0x00020000
#define KERNEL_HEAP_MIN		0x00070000

uintptr_t _kmalloc(size_t size, uint8_t align, uintptr_t* phys);

uintptr_t kmalloc(size_t size);
uintptr_t kmalloc_a(size_t size);
uintptr_t kmalloc_p(size_t size, uintptr_t *phys);
uintptr_t kmalloc_ap(size_t size, uintptr_t *phys);

uintptr_t map_to_physical(uintptr_t);

#endif

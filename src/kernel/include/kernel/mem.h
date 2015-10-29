#ifndef _KERNEL_MEM_H
#define _KERNEL_MEM_H value

uintptr_t _kmalloc(size_t size, uint8_t align, uintptr_t* phys);

uintptr_t kmalloc(size_t size);
uintptr_t kmalloc_a(size_t size);
uintptr_t kmalloc_p(size_t size, uintptr_t *phys);
uintptr_t kmalloc_ap(size_t size, uintptr_t *phys);

uintptr_t map_to_physical(uintptr_t);

#endif

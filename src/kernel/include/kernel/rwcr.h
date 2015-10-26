#ifndef _KERNEL_RWCR_H
#define _KERNEL_RWCR_H 1

#if !defined(__cpluscplus)
#include <stdbool.h>
#endif
#include <stddef.h>
#include <stdint.h>

extern uint32_t read_cr0();
extern void 	write_cr0(uint32_t);
extern uint32_t read_cr2();
extern uint32_t	read_cr3();
extern void 	write_cr3(uint32_t);

#endif

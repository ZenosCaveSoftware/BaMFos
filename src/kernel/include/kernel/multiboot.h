#ifndef _KERNEL_MULTIBOOT_H
#define _KERNEL_MULTIBOOT_H 1

#if !defined(__cpluscplus)
#include <stdbool.h>
#endif
#include <stddef.h>
#include <stdint.h>

#define MULTIBOOT_MAGIC 		0xC001C0D3
#define MULTIBOOT_EAX_MAGIC 	0xC001C0D3
#define MULTIBOOT_FLAG_ALIGN   	0x001
#define MULTIBOOT_FLAG_MEMINFO 	0x002
#define MULTIBOOT_FLAG_CMDLINE 	0x004
#define MULTIBOOT_FLAG_MODS    	0x008
#define MULTIBOOT_FLAG_AOUT    	0x010
#define MULTIBOOT_FLAG_ELF     	0x020
#define MULTIBOOT_FLAG_MMAP    	0x040
#define MULTIBOOT_FLAG_CONFIG  	0x080
#define MULTIBOOT_FLAG_LOADER  	0x100
#define MULTIBOOT_FLAG_APM     	0x200
#define MULTIBOOT_FLAG_VBE     	0x400

typedef struct multiboot_struct_t
{
	uintptr_t flags;
	uintptr_t mem_lower;
	uintptr_t mem_upper;
	uintptr_t boot_device;
	uintptr_t cmdline;
	uintptr_t mods_count;
	uintptr_t mods_addr;
	uintptr_t num;
	uintptr_t size;
	uintptr_t addr;
	uintptr_t shndx;
	uintptr_t mmap_length;
	uintptr_t mmap_addr;
	uintptr_t drives_length;
	uintptr_t drives_addr;
	uintptr_t config_table;
	uintptr_t boot_loader_name;
	uintptr_t apm_table;
	uintptr_t vbe_control_info;
	uintptr_t vbe_mode_info;
	uintptr_t vbe_mode;
	uintptr_t vbe_interface_seg;
	uintptr_t vbe_interface_off;
	uintptr_t vbe_interface_len;
} multiboot_t;

#endif

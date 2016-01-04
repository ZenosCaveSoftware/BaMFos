#include <boot/a2p.h> 
.set ALIGN,    1<<0             # align loaded modules on page boundaries
.set MEMINFO,  1<<1             # provide memory map
#.set KLUDGE,   1<<16
.set FLAGS,    ALIGN | MEMINFO	# this is the Multiboot 'flag' field
.set MAGIC,    0x1BADB002       # 'magic number' lets bootloader find the header
.set CHECKSUM, -(MAGIC + FLAGS) # checksum of above, to prove we are multiboot


.section .text
.code64
.global multiboot
.type multiboot, @function
multiboot:
	.align 4
	.long MAGIC
	.long FLAGS
	.long CHECKSUM
	.long (multiboot)
	.long KA2P(code)
	.long KA2P(data_end)
	.long KA2P(end)
	.long KA2P(_start)

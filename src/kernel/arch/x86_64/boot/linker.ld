OUTPUT_FORMAT(elf64-x86-64)
ENTRY(_start)
load = 0x0000001000;
phys = 0x0000100000;
virt = 0xFFFFFFFF80000000;

SECTIONS
{
	. = load;
	.bootstrap : AT(ADDR(.bootstrap))
	{
		*bootstrap.S.o(.text)
	}

	. = virt + phys;

	.text :  AT(ADDR(.text) - virt)
	{
		code = .;
		*(EXCLUDE_FILE(*bootstrap.S.o) .text)
		*(.rodata*)
		. = ALIGN(4096);
	}

	.data : AT(ADDR(.data) - virt)
	{
		data = .;
		*(.data)
		data_end = .;
		. = ALIGN(4096);
	}

	.eh_frame : AT(ADDR(.eh_frame) - virt)
	{
		ehframe = .;
		*(.eh_frame)
		. = ALIGN(4096);
	}

	.bss : AT(ADDR(.bss) - virt)
	{
		bss = .;
		*(COMMON)
		*(.bss)
		*(.bootstrap_stack)
		. = ALIGN(4096);
	}

	end = .;
	
	/DISCARD/ : {
		*(.note.GNU-stack)
		*(.note.gnu.*)
		*(.comment*)
	}

}
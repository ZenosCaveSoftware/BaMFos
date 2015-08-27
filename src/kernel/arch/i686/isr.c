/* Macro to create hardware interrupt handling functions. */
/* It will call "irqfunc" with the register context and the IRQ number as parameters. */
#define DEFIRQWRAPPER(irqnum)\
void *irq##irqnum##handler(void)\
{\
	volatile void *addr;\
	asm goto("jmp %l[endofISR]" ::: "memory" : endofISR);\
	asm volatile(".align 16" ::: "memory");\
	startofISR:\
	asm volatile("pushal\n\tpushl %%ebp\n\tmovl %%esp, %%ebp\n\tcld" ::: "memory");\
	asm volatile(\
		"pushl %%ds       \n\t"\
		"pushl %%es       \n\t"\
		"movw $16, %%cx   \n\t"\
		"movw %%cx, %%ds  \n\t"\
		"movw %%cx, %%es  \n\t"\
		"pushl %%ebp      \n\t"\
		"addl $4, (%%esp) \n\t"\
		"pushl %%ebx      \n\t"\
		"call *%%eax      \n\t"\
		"addl $8, %%esp       "\
		:: "a"(irqfunc), "b"((uint32_t) 0) : "memory");\
	asm volatile("popl %%es\n\tpopl %%ds\n\tleave\n\tpopal\n\tiret" ::: "memory");\
	endofISR:\
	asm goto(\
		".intel_syntax noprefix\n\t"\
		"mov eax, offset %l[startofISR]\n\t"\
		"mov [ebx], eax\n\t"\
		".att_syntax"\
		:: "b"(&addr) : "eax", "memory" : startofISR);\
	return((void *) addr);\
}

/* Macro to create sotfware interrupt handling functions. */
/* t will call "intfunc" with the register context and the interrupt number as parameters. */
#define DEFINTWRAPPER(intnum)\
void *int##intnum##handler(void)\
{\
	volatile void *addr;\
	asm goto("jmp %l[endofISR]" ::: "memory" : endofISR);\
	asm volatile(".align 16" ::: "memory");\
	startofISR:\
	asm volatile("pushal\n\tpushl %%ebp\n\tmovl %%esp, %%ebp\n\tcld" ::: "memory");\
	asm volatile(\
		"pushl %%ds       \n\t"\
		"pushl %%es       \n\t"\
		"movw $16, %%cx   \n\t"\
		"movw %%cx, %%ds  \n\t"\
		"movw %%cx, %%es  \n\t"\
		"pushl %%ebp      \n\t"\
		"addl $4, (%%esp) \n\t"\
		"pushl %%ebx      \n\t"\
		"call *%%eax      \n\t"\
		"addl $8, %%esp       "\
		:: "a"(intfunc), "b"((uint32_t) intnum) : "memory");\
	asm volatile("popl %%es\n\tpopl %%ds\n\tleave\n\tpopal\n\tiret" ::: "memory");\
	endofISR:\
	asm goto(\
		".intel_syntax noprefix         \n\t"\
		"mov eax, offset %l[startofISR] \n\t"\
		"mov [ebx], eax                 \n\t"\
		".att_syntax                        "\
		:: "b"(&addr) : "eax", "memory" : startofISR);\
	return((void *) addr);\
}

/*  Macro to create exception handling functions, for exceptions with error code. */
/* It will call intfunc_err, with the error code, the register context, and the interrupt number as parameters. */
#define DEFINTWRAPPER_ERR(intnum)\
void *int##intnum##handler(void)\
{\
	volatile void *addr;\
	asm goto("jmp %l[endofISR]" ::: "memory" : endofISR);\
	asm volatile(".align 16" ::: "memory");\
	startofISR:\
	asm volatile(\
		"pushal                \n\t"\
		"pushl %%ebp           \n\t"\
		"movl %%esp, %%ebp     \n\t"\
		"pushl %%ds            \n\t"\
		"pushl %%es            \n\t"\
		"movw $16, %%cx        \n\t"\
		"movw %%cx, %%ds       \n\t"\
		"movw %%cx, %%es       \n\t"\
		"movl 36(%%ebp), %%edx \n\t"\
		"movl %%ebp, %%esi     \n\t"\
		"addl $32, %%esi       \n\t"\
		"movl %%esi, %%edi     \n\t"\
		"addl $4, %%edi        \n\t"\
		"movl $11, %%ecx       \n\t"\
		"std                   \n\t"\
		"rep movsl             \n\t"\
		"add $4, %%esp         \n\t"\
		"cld                       "\
		::: "memory");\
	asm volatile(\
		"pushl %%edx       \n\t"\
		"pushl %%ebp       \n\t"\
		"addl $8, (%%esp)  \n\t"\
		"pushl %%ebx       \n\t"\
		"call *%%eax       \n\t"\
		"addl $12, %%esp       "\
		:: "a"(intfunc_err), "b"((uint32_t) intnum) : "memory");\
	asm volatile("popl %%es\n\tpopl %%ds\n\tleave\n\tpopal\n\tiret" ::: "memory");\
	endofISR:\
	asm goto(\
		".intel_syntax noprefix         \n\t"\
		"mov eax, offset %l[startofISR] \n\t"\
		"mov [ebx], eax                 \n\t"\
		".att_syntax                        "\
		:: "b"(&addr) : "eax", "memory" : startofISR);\
	return((void *) addr);\
}

//IRQs (the 16 IRQs the PIC has)
DEFIRQWRAPPER(0);
DEFIRQWRAPPER(1);
DEFIRQWRAPPER(2);
DEFIRQWRAPPER(3);
DEFIRQWRAPPER(4);
DEFIRQWRAPPER(5);
DEFIRQWRAPPER(6);
DEFIRQWRAPPER(7);
DEFIRQWRAPPER(8);
DEFIRQWRAPPER(9);
DEFIRQWRAPPER(10);
DEFIRQWRAPPER(11);
DEFIRQWRAPPER(12);
DEFIRQWRAPPER(13);
DEFIRQWRAPPER(14);
DEFIRQWRAPPER(15);
 
//exceptions without error code
DEFINTWRAPPER(0);//division by 0
DEFINTWRAPPER(1);//debug
DEFINTWRAPPER(2);//NMI
DEFINTWRAPPER(3);//breakpoint
DEFINTWRAPPER(4);//INTO
DEFINTWRAPPER(5);//BOUND
DEFINTWRAPPER(6);//invalid opcode
DEFINTWRAPPER(7);//coprocessor not available
DEFINTWRAPPER(9);//coprocessor segment overrun
DEFINTWRAPPER(16);//coprocessor error
 
//exceptions with error code
DEFINTWRAPPER_ERR(8);//double fault
DEFINTWRAPPER_ERR(10);//TSS error
DEFINTWRAPPER_ERR(11);//segment not present
DEFINTWRAPPER_ERR(12);//stack fault
DEFINTWRAPPER_ERR(13);//general protection fault
DEFINTWRAPPER_ERR(14);//page fault
 
//software interrupts
DEFINTWRAPPER(0x80);//system call
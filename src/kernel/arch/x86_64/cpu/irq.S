.section .text
.align 4

.macro IRQ ident offset
	 .global _irq\ident
	 _irq\ident:
		  cli
		  push $0x00
		  push $\offset
		  jmp irq_common
.endm

.macro PUSHAQ
	pushq %rax		#save current rax
	pushq %rbx		#save current rbx
	pushq %rcx		#save current rcx
	pushq %rdx		#save current rdx
	pushq %rbp		#save current rbp
	pushq %rsi		#save current rsi
	pushq %rdi		#save current rdi
	pushq %r8		#save current r8
	pushq %r9		#save current r9
	pushq %r10		#save current r10
	pushq %r11		#save current r11
	pushq %r12		#save current r12
	pushq %r13		#save current r13
	pushq %r14		#save current r14
	pushq %r15		#save current r15
.endm

.macro POPAQ
	popq %r15		#restore current r15
	popq %r14		#restore current r14
	popq %r13		#restore current r13
	popq %r12		#restore current r12
	popq %r11		#restore current r11
	popq %r10		#restore current r10
	popq %r9		#restore current r9
	popq %r8		#restore current r8
	popq %rdi		#restore current rdi
	popq %rsi		#restore current rsi
	popq %rbp		#restore current rbp
	popq %rdx		#restore current rdx
	popq %rcx		#restore current rcx
	popq %rbx		#restore current rbx
	popq %rax		#restore current rax
.endm


IRQ 0, 32
IRQ 1, 33
IRQ 2, 34
IRQ 3, 35
IRQ 4, 36
IRQ 5, 37
IRQ 6, 38
IRQ 7, 39
IRQ 8, 40
IRQ 9, 41
IRQ 10, 42
IRQ 11, 43
IRQ 12, 44
IRQ 13, 45
IRQ 14, 46
IRQ 15, 47

.extern irq_handler

irq_common:

	PUSHAQ

	movq %rsp, %rdi
	callq irq_handler
	
	POPAQ
	add $0x10, %esp
	iretq
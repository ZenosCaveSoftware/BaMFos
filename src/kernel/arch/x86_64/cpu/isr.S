.section .text
.align 4

.macro ISR_NOERR idx
	.global _isr\idx
	_isr\idx:
		cli
		push $0
		push $\idx
		jmp isr_common
.endm

.macro ISR_ERR idx
	.global _isr\idx
	_isr\idx:
		cli
		push $\idx
		jmp isr_common
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

ISR_NOERR 0
ISR_NOERR 1
ISR_NOERR 2
ISR_NOERR 3
ISR_NOERR 4
ISR_NOERR 5
ISR_NOERR 6
ISR_NOERR 7
ISR_ERR 8
ISR_NOERR 9
ISR_ERR 10
ISR_ERR 11
ISR_ERR 12
ISR_ERR 13
ISR_ERR 14
ISR_NOERR 15
ISR_NOERR 16
ISR_NOERR 17
ISR_NOERR 18
ISR_NOERR 19
ISR_NOERR 20
ISR_NOERR 21
ISR_NOERR 22
ISR_NOERR 23
ISR_NOERR 24
ISR_NOERR 25
ISR_NOERR 26
ISR_NOERR 27
ISR_NOERR 28
ISR_NOERR 29
ISR_NOERR 30
ISR_NOERR 31
ISR_NOERR 127

.extern fault_handler
.type fault_handler, @function

isr_common:
	PUSHAQ
	

	movq %rsp, %rdi
	call fault_handler

	POPAQ
	add $0x10, %esp
	iretq
	
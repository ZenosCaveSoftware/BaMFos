# Reserve a stack for the initial thread.
.section .bootstrap_stack,"aw",@nobits
.code64
stack_bottom:
	.skip 16384 # 16 KiB
stack_top:

# The kernel entry point.
.section .text
.code64
.global _start
.type _start, @function
_start:
	movq $stack_top, %rsp

	and $-16, %rsp

	pushq %rsp
	pushq %rax
	pushq %rbx

	cli
	# Initialize the core kernel before running the global constructors.
	call kernel_early

	# Call the global constructors.
	call _init

	# Transfer control to the main kernel.
	call kernel_main

	# Hang if kernel_main unexpectedly returns.
	cli
.Lhang:
	hlt
	jmp .Lhang
.size _start, . - _start

.global gdt_flush       	# Allows the C code to call gdt_flush().

gdt_flush:
   lgdt (%rdi)          	# Load the new GDT pointer

   pushq %rsi				# push code selector
   movabsq $.flush, %r10
   pushq %r10
   lretq
.flush:
   movq %rdx, %es
   movq %rdx, %fs
   movq %rdx, %gs
   movq %rdx, %ds
   movq %rdx, %ss
   ret
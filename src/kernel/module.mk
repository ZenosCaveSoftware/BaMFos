KSRC += ../src/kernel/kernel.c 							\
$(shell find ../src/kernel/arch/$(ARCH)/ -name '*.c')	\
$(shell find ../src/kernel/arch/$(ARCH)/ ! -name 'crt*.S' -name '*.S')

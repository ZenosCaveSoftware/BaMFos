KSRC += $(shell ls kernel/*.c) 		\
$(shell ls kernel/arch/$(ARCH)/*.c) \
$(shell find ../src/kernel/arch/$(ARCH)/ ! -name 'crt*.S' -name '*.S')

KSRC += $(shell ls kernel/*.c) 		\
$(shell ls kernel/arch/$(ARCH)/*.c) \
$(shell ls kernel/arch/$(ARCH)/*.S)
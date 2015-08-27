LSRC += $(shell ls libc/*.c) 		\
$(shell ls libc/stdio/*.c) 			\
$(shell ls libc/stdlib/*.c)			\
$(shell ls libc/string/*.c) 		\
$(shell ls libc/arch/$(ARCH)/*.c)	\
$(shell ls libc/arch/$(ARCH)/*.S)	\
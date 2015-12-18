KSRC += $(shell find ../src/kernel/arch/$(ARCH)/ -name '*.c') \
$(shell find ../src/kernel/arch/$(ARCH)/ ! -name 'crt*.S' -name '*.S')

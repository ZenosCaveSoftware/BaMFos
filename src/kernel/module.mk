KSRC += $(shell find kernel/ -name '*.c') \
				$(shell find kernel/ ! -name 'crt*.S' -name '*.S')

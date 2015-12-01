
MODULES = kernel libc

CFLAGS += $(patsubst %,-I%/include,	$(MODULES)) \
	-ffreestanding -nostdlib -lgcc -std=c11 -DNDEBUG

ARCH ?= i686
ARCHDIR := kernel/arch/$(ARCH)

PREFIX?=/usr/local
EXEC_PREFIX?=$(PREFIX)
BOOTDIR?=$(EXEC_PREFIX)/boot
INCLUDEDIR?=$(PREFIX)/include

LIBS :=

KSRC :=
LSRC :=

include $(patsubst %,\
	%/module.mk,$(MODULES))


KSRC := $(filter-out kernel/arch/$(ARCH)/boot/crti.s, $(KSRC))
KSRC := $(filter-out kernel/arch/$(ARCH)/boot/crtn.s, $(KSRC))

CRTI_OBJ:=$(ARCHDIR)/boot/crti.S.o
CRTBEGIN_OBJ:=$(shell $(CC) $(CFLAGS) $(LDFLAGS) -print-file-name=crtbegin.o)
CRTEND_OBJ:=$(shell $(CC) $(CFLAGS) $(LDFLAGS) -print-file-name=crtend.o)
CRTN_OBJ:=$(ARCHDIR)/boot/crtn.S.o

KOBJ :=									\
		$(patsubst %.c,%.c.o,			\
			$(filter %.c,$(KSRC)))		\
		$(patsubst %.S,%.S.o,			\
			$(filter %.S,$(KSRC)))		\

LOBJ :=									\
		$(patsubst %.c,%.c.o,				\
			$(filter %.c,$(LSRC)))		\
		$(patsubst %.S,%.S.o,				\
			$(filter %.S,$(LSRC)))		\

OBJ_LINKER :=	\
$(CRTI_OBJ) 	\
$(CRTBEGIN_OBJ) \
$(KOBJ) 		\
$(LOBJ) 		\
$(CRTEND_OBJ)	\
$(CRTN_OBJ) 	\

SRCS := $(KSRC) $(LSRC)

all: BaMF.kernel

.PHONY: all clean install install-headers install-kernel

BaMF.kernel: $(OBJ_LINKER) $(ARCHDIR)/boot/linker.ld
	$(CC) -T $(ARCHDIR)/boot/linker.ld -o $@ $(CFLAGS) $(OBJ_LINKER) $(LIBS) 2>&1 > ../log/$@.log
	@echo "--- KERNEL LINKED ---"

%.c.o: %.c
	$(CC) -c $< -o $@ -std=c11 $(CFLAGS) 2>&1 > ../log/$(shell basename $@ .o).log
	@echo "--- COMPILED $(shell basename $<) ---"
	
%.S.o: %.S
	$(CC) -c $< -o $@ $(CFLAGS) 2>&1 > ../log/$(shell basename $@ .o).log
	@echo "--- ASSEMBLED $(shell basename $<) ---"

install: install-headers install-kernel

install-headers:
	@mkdir -p $(DESTDIR)$(INCLUDEDIR)
	@for module in $(MODULES); do cp -RT $$module/include $(DESTDIR)$(INCLUDEDIR); done
	@echo "--- HEADERS INSTALLED ---"

install-kernel: BaMF.kernel
	@mkdir -p $(DESTDIR)$(BOOTDIR)
	@cp BaMF.kernel $(DESTDIR)$(BOOTDIR)
	@echo "--- KERNEL INSTALLED ---"

clean: 
	find ./ -name "*.o" -exec rm {} \;
	find ./ -name "*.d" -exec rm {} \;
	@echo "--- CLEANED DIRS ---"

MODULES = kernel libc

CFLAGS += $(patsubst %,-I%/include,\
	$(MODULES)) -ffreestanding -nostdlib -lgcc -std=c11

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


KSRC := $(filter-out kernel/arch/$(ARCH)/crti.s, $(KSRC))
KSRC := $(filter-out kernel/arch/$(ARCH)/crtn.s, $(KSRC))

CRTI_OBJ:=$(ARCHDIR)/crti.o
CRTBEGIN_OBJ:=$(shell $(CC) $(CFLAGS) $(LDFLAGS) -print-file-name=crtbegin.o)
CRTEND_OBJ:=$(shell $(CC) $(CFLAGS) $(LDFLAGS) -print-file-name=crtend.o)
CRTN_OBJ:=$(ARCHDIR)/crtn.o

KOBJ :=									\
		$(patsubst %.c,%.c.o,				\
			$(filter %.c,$(KSRC)))		\
		$(patsubst %.S,%.S.o,				\
			$(filter %.S,$(KSRC)))		\

LOBJ :=									\
		$(patsubst %.c,%.o,				\
			$(filter %.c,$(LSRC)))		\
		$(patsubst %.S,%.o,				\
			$(filter %.S,$(LSRC)))		\

OBJ_LINKER :=	\
$(CRTI_OBJ) 	\
$(CRTBEGIN_OBJ) \
$(KOBJ) 		\
$(LOBJ) 		\
$(CRTEND_OBJ)	\
$(CRTN_OBJ) 	\


.PHONY: all clean install install-headers install-kernel

all: BaMF.kernel

BaMF.kernel: $(OBJ_LINKER) $(ARCHDIR)/linker.ld
	$(CC) -T $(ARCHDIR)/linker.ld -o $@ $(CFLAGS) $(OBJ_LINKER) $(LIBS) \
	2>&1 | tee ../log/$@.log

%.c.o: %.c
	$(CC) -c $< -o $@ -std=c11 $(CFLAGS) 2>&1 | tee ../log/$(shell basename $@ .o).log

%.S.o: %.S
	$(CC) -c $< -o $@ $(CFLAGS) 2>&1 | tee ../log/$(shell basename $@ .o).log

clean: 
	rm -f BaMF.kernel $(CRTI_OBJ) $(KOBJ) $(LOBJ) $(CRTN_OBJ) 

install: install-headers install-kernel

install-headers:
	mkdir -p $(DESTDIR)$(INCLUDEDIR)
	for module in $(MODULES); do \
		cp -RTv $$module/include $(DESTDIR)$(INCLUDEDIR); \
	done

install-kernel: BaMF.kernel
	mkdir -p $(DESTDIR)$(BOOTDIR)
	cp BaMF.kernel $(DESTDIR)$(BOOTDIR)

#include $(KOBJ:.o=.d) $(LOBJ:.o=.d)

%.d: %.c
	depend.sh `dirname $*.c` $(CFLAGS) $< > $@
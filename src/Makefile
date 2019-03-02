MODULES ?= kernel libc
HOST ?= i686-elf
AR ?= $(HOST)-ar
AS ?= $(HOST)-as
CC ?= $(HOST)-gcc
LD ?= $(HOST)-ld

PREFIX ?= /usr
EXEC_PREFIX ?= /usr
BOOTDIR ?= /boot
INCLUDEDIR ?= $(PREFIX)/include
CFLAGS ?= --sysroot=$PWD/../sysroot -isystem=$INCLUDEDIR
LDFLAGS ?= --sysroot=$PWD/../sysroot -isystem=$INCLUDEDIR

CFLAGS += -I../include/kernel \
	-I ../include/libc -O3 \
	-ffreestanding -fpic -fno-common \
	-fpic -nostdlib -lgcc -std=c11 -DNDEBUG \

ARCHDIR = kernel

LIBS :=

KSRC :=
LSRC :=

include $(patsubst %,\
	%/module.mk,$(MODULES))

KSRC := $(filter-out boot/crti.s, $(KSRC))
KSRC := $(filter-out boot/crtn.s, $(KSRC))

CRTI_OBJ:=$(ARCHDIR)/boot/crti.S.o
CRTBEGIN_OBJ:=../crt/crtbegin.o
CRTEND_OBJ:=../crt/crtend.o
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
$(CRTBEGIN_OBJ)	\
$(KOBJ) 		\
$(LOBJ) 		\
$(CRTEND_OBJ)	\
$(CRTN_OBJ) 	\

SRCS := $(KSRC) $(LSRC)

.PHONY: clean install install-headers install-kernel

BaMF.kernel: $(OBJ_LINKER) $(ARCHDIR)/boot/linker.ld
	$(CC) -Wl,--build-id=none -T $(ARCHDIR)/boot/linker.ld -o $@ $(CFLAGS) $(OBJ_LINKER) $(LIBS) 2>&1 | tee ../log/$@.log
	@echo "--- KERNEL LINKED ---"

%.c.o: %.c
	$(CC) -c $< -o $@ -std=c11 $(CFLAGS) 2>&1 | tee ../log/$(shell basename $@ .o).log
	@echo "--- COMPILED $(shell basename $<) ---"

%.S.o: %.S
	$(CC) -c $< -o $@ $(CFLAGS) 2>&1 | tee ../log/$(shell basename $@ .o).log
	@echo "--- ASSEMBLED $(shell basename $<) ---"

install: install-headers install-kernel

install-headers:
	@mkdir -p $(DESTDIR)$(INCLUDEDIR)
	@cp -RT ../include/kernel $(DESTDIR)$(INCLUDEDIR);
	@cp -RT ../include/libc $(DESTDIR)$(INCLUDEDIR);
	@echo "--- HEADERS INSTALLED ---"

install-kernel: BaMF.kernel
	@mkdir -p $(DESTDIR)$(BOOTDIR)
	@mv BaMF.kernel $(DESTDIR)$(BOOTDIR)
	@echo "--- KERNEL INSTALLED ---"

clean:
	find ./ -name "*.o" -exec rm {} \;
	find ./ -name "*.d" -exec rm {} \;
	@echo "--- CLEANED DIRS ---"

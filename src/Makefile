MODULES ?= kernel libc
HOST ?= i686-elf
AR ?= $(HOST)-ar
AS ?= $(HOST)-as
CC ?= $(HOST)-gcc
LD ?= $(HOST)-ld
NASM ?= nasm

CFLAGS += -Ikernel/include \
          -Ilibc/include \
		  -ffreestanding \
		  -fno-common \
		  -fpic \
		  -nostdlib \
		  -nodefaultlibs \
		  -nostartfiles \
		  -std=c11 \
		  -DNDEBUG

PREFIX?=/usr/local
BOOTDIR?=/boot
INCLUDEDIR?=$(PREFIX)/include

KSRC  :=
LSRC  :=
BOOTSRC := bootloader/Stage1/Stage1.asm
LDRSRC  := bootloader/Stage2/Stage2.asm
LIBS	:=

include $(patsubst %,\
	%/module.mk,$(MODULES))

KSRC := $(filter-out boot/crti.s, $(KSRC))
KSRC := $(filter-out boot/crtn.s, $(KSRC))

CRTI_OBJ:=kernel/boot/crti.S.o
CRTBEGIN_OBJ:=../crt/crtbegin.o
CRTEND_OBJ:=../crt/crtend.o
CRTN_OBJ:=kernel/boot/crtn.S.o

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

all: build/Boot.bin build/KRNLDR.SYS build KRNL.SYS

.PHONY: all clean install install-headers install-kernel

Boot.bin:
	$(NASM) -i bootloader/inc/ -f bin $(BOOTSRC) -o $@
	@echo "--- BOOT Stage1 BUILT ---"

KRNLDR.SYS:
	$(NASM) -i bootloader/inc/ -f bin $(LDRSRC) -o $@
	@echo "--- LOADER Stage2 BUILT ---"

KRNL.SYS: $(OBJ_LINKER) kernel/boot/linker.ld
	$(CC) -Wl,--build-id=none -T kernel/boot/linker.ld -o $@ $(CFLAGS) $(OBJ_LINKER) $(LIBS) 2>&1 | tee ../log/$@.log
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
	@cp -RT kernel/include/ $(DESTDIR)$(INCLUDEDIR);
	@cp -RT libc/include/ $(DESTDIR)$(INCLUDEDIR);
	@echo "--- HEADERS INSTALLED ---"

install-kernel: KRNL.SYS KRNLDR.SYS Boot.bin
	@mkdir -p $(DESTDIR)$(BOOTDIR)
	@mv Boot.bin KRNLDR.SYS KRNL.SYS $(DESTDIR)$(BOOTDIR)
	@echo "--- KERNEL INSTALLED ---"

clean:
	find ./ -name "*.o" -exec rm {} \;
	find ./ -name "*.d" -exec rm {} \;
	@echo "--- CLEANED DIRS ---"

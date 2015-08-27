
MODULES = kernel libc

CFLAGS += $(patsubst %,-I%/include,\
	$(MODULES))

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

KOBJ :=								\
		$(patsubst %.c,%.o,			\
			$(filter %.c,$(KSRC)))	\
		$(patsubst %.S,%.o,			\
			$(filter %.S,$(KSRC)))

LOBJ :=								\
		$(patsubst %.c,%.o,			\
			$(filter %.c,$(LSRC)))	\
		$(patsubst %.S,%.o,			\
			$(filter %.S,$(LSRC)))

.PHONY: all clean install install-headers install-kernel

all: BaMF.kernel

BaMF.kernel: $(KOBJ) $(ARCHDIR)/linker.ld
	$(CC) -T $(ARCHDIR)/linker.ld -o $@ $(CFLAGS) $(KOBJ) $(LDFLAGS) $(LIBS)

%.o: %.c
	$(CC) -c $< -o $@ -std=c11 $(CFLAGS)

%.o: %.S
	$(AS) -c $< -o $@ $(CFLAGS)

clean: 
	rm -f BaMF.kernel $(KOBJ) $(LOBJ)

install: install-headers install-kernel

install-headers:
	mkdir -p $(DESTDIR)$(INCLUDEDIR)
	for module in $(MODULES); do \
		cp -RTv $$module/include $(DESTDIR)$(INCLUDEDIR); \
	done

install-kernel: BaMF.kernel
	mkdir -p $(DESTDIR)$(BOOTDIR)
	cp BaMF.kernel $(DESTDIR)$(BO

#include $(KOBJ:.o=.d) $(LOBJ:.o=.d)

%.d: %.c
	depend.sh `dirname $*.c` $(CFLAGS) $< > $@
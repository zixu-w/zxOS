include ../make.config

DEFAULT_HOST != ../utils/default-host.sh
HOST         ?= DEFAULT_HOST
HOSTARCH     != ../utils/target-triplet-to-arch.sh $(HOST)
HOSTPLAT     != ../utils/target-triplet-to-plat.sh $(HOST)
BOOT_TYPE    != ../utils/default-boot.sh

CFLAGS   ?= -O2 -g
CPPFLAGS ?=
LDFLAGS  ?=
LIBS     ?=

DESTDIR     ?=
PREFIX      ?= /usr/local
EXEC_PREFIX ?= $(PREFIX)
BOOTDIR     ?= $(EXEC_PREFIX)/boot
INCLUDEDIR  ?= $(PREFIX)/include

CFLAGS   := $(CFLAGS) -ffreestanding -Wall -Wextra
CPPFLAGS := $(CPPFLAGS) -D__is_kernel -Iinclude
LDFLAGS  := $(LDFLAGS)
LIBS     := $(LIBS) -nostdlib -lk -lgcc

KERNEL_DIR     := kernel
ARCHDIR        := arch/$(HOSTARCH)

AUX_DIRS=\
$(DESTDIR)$(INCLUDEDIR) \
$(DESTDIR)$(BOOTDIR) \

include $(ARCHDIR)/make.config

CFLAGS   := $(CFLAGS)   $(KERNEL_ARCH_CFLAGS)
CPPFLAGS := $(CPPFLAGS) $(KERNEL_ARCH_CPPFLAGS)
LDFLAGS  := $(LDFLAGS)  $(KERNEL_ARCH_LDFLAGS)
LIBS     := $(LIBS)     $(KERNEL_ARCH_LIBS)

KERNEL_OBJS=\
$(KERNEL_ARCH_OBJS) \
$(KERNEL_DIR)/kernel.o \

OBJS=\
$(ARCHDIR)/crti.o \
$(ARCHDIR)/crtbegin.o \
$(KERNEL_OBJS) \
$(ARCHDIR)/crtend.o \
$(ARCHDIR)/crtn.o \

LINK_LIST=\
$(LDFLAGS) \
$(ARCHDIR)/crti.o \
$(ARCHDIR)/crtbegin.o \
$(KERNEL_OBJS) \
$(LIBS) \
$(ARCHDIR)/crtend.o \
$(ARCHDIR)/crtn.o \

PROG_NAME    ?= zxos
KERNEL_NAME  := $(PROG_NAME).bin
BUILD_TARGET := $(KERNEL_DIR)/$(KERNEL_NAME)

CHECK := $(CROSS_GRUB)-file --is-$(HOSTPLAT)-$(BOOT_TYPE) $(BUILD_TARGET)

.PHONY    : all clean install install-headers install-kernel
.SUFFIXES :
.SUFFIXES : .o .c .S

all : $(BUILD_TARGET)

$(BUILD_TARGET) : $(OBJS) $(ARCHDIR)/linker.ld | $(KERNEL_BIN_DIR)
	@:$(call check_defined, CROSS_LINKER, cross platform linker)
	$(CROSS_LINKER) -T $(ARCHDIR)/linker.ld -o $@ $(CFLAGS) $(LINK_LIST)
	@:$(call check_defined, CROSS_GRUB, cross platform grub)
	$(CHECK)

$(ARCHDIR)/crtbegin.o $(ARCHDIR)/crtend.o :
	@:$(call check_defined, CROSS_CC, cross platform C compiler)
	OBJ=`$(CROSS_CC) $(CFLAGS) $(LDFLAGS) -print-file-name=$(@F)` && cp "$$OBJ" $@

.c.o :
	@:$(call check_defined, CROSS_CC, cross platform C compiler)
	$(CROSS_CC) -MD -c $< -o $@ -std=gnu11 $(CFLAGS) $(CPPFLAGS)

.S.o :
	@:$(call check_defined, CROSS_AS, cross platform assembler)
	$(CROSS_AS) -c $< -o $@

$(AUX_DIRS) :
	mkdir -p $@

clean :
	$(RM) $(BUILD_TARGET)
	$(RM) $(OBJS) *.o */*.o */*/*.o
	$(RM) $(OBJS:.o=.d) *.d */*.d */*/*.d

install : install-headers install-kernel

install-headers : | $(DESTDIR)$(INCLUDEDIR)
	cp -R --preserve=timestamps include/. $(DESTDIR)$(INCLUDEDIR)/.

install-kernel : $(BUILD_TARGET) | $(DESTDIR)$(BOOTDIR)
	cp $(BUILD_TARGET) $(DESTDIR)$(BOOTDIR)

-include $(OBJS:.o=.d)

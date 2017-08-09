include make.config

.PHONY : all pre-install kernel libc install image qemu-run clean

PWD != pwd

export HOST       != utils/default-host.sh
export PREFIX     ?= /usr
export EXEC_PREFIX = $(PREFIX)
export BOOTDIR     = /boot
export LIBDIR      = $(EXEC_PREFIX)/lib
export INCLUDEDIR  = $(PREFIX)/include
export PROG_NAME   = zxos

KERNEL_NAME := $(PROG_NAME).bin
KERNEL_DIR  := kernel
LIBC_DIR    := libc

IMGDIR       := image
IMG_WORK_DIR := $(IMGDIR)/iso
IMG_BOOT_DIR := $(IMG_WORK_DIR)/boot
IMG_GRUB_DIR := $(IMG_BOOT_DIR)/grub
IMG_TARGET   := $(IMGDIR)/zxos.iso

CROSS_PATH ?= /usr/local/cross
SYSROOT    ?= $(PWD)/sysroot

KERNEL_TARGET := $(SYSROOT)/boot/$(KERNEL_NAME)

export DESTDIR = $(SYSROOT)

CROSS_AR     ?= $(CROSS_PATH)/bin/$(HOST)-ar
CROSS_AS     ?= $(CROSS_PATH)/bin/$(HOST)-as
CROSS_CC     ?= $(CROSS_PATH)/bin/$(HOST)-gcc
CROSS_LINKER ?= $(CROSS_PATH)/bin/$(HOST)-gcc
CROSS_GRUB   ?= $(CROSS_PATH)/bin/grub

CC_MOD       != utils/decorate-cc.sh $(HOST) $(INCLUDEDIR)
CROSS_CC     := $(CROSS_CC) --sysroot=$(SYSROOT) $(CC_MOD)
CROSS_LINKER := $(CROSS_LINKER) --sysroot=$(SYSROOT)

export CROSS_AR CROSS_AS CROSS_CC CROSS_LINKER CROSS_GRUB

QEMU         ?= qemu
QEMU_PLAT    != utils/target-triplet-to-arch.sh $(HOST)
QEMU_COMMAND := $(QEMU)-system-$(QEMU_PLAT)
GRUB_CONFIG  := utils/grub.cfg

DIRS=\
$(IMGDIR) \
$(IMG_WORK_DIR) \
$(IMG_BOOT_DIR) \
$(IMG_GRUB_DIR) \
$(SYSROOT) \

CLEAN_DIRS := $(DIRS)

all : libc kernel

kernel : pre-install
	$(MAKE) -C $(KERNEL_DIR)

libc : pre-install
	$(MAKE) -C $(LIBC_DIR)

pre-install : | $(SYSROOT)
	$(MAKE) -C $(LIBC_DIR) install-headers
	$(MAKE) -C $(KERNEL_DIR) install-headers
	$(MAKE) -C $(LIBC_DIR) install-libs

install : all
	$(MAKE) -C $(LIBC_DIR) install
	$(MAKE) -C $(KERNEL_DIR) install

image : install $(IMG_TARGET)

$(IMG_TARGET) : $(GRUB_CONFIG) | $(IMG_GRUB_DIR) $(IMG_BOOT_DIR) $(IMG_WORK_DIR)
	cp $(KERNEL_TARGET) $(IMG_BOOT_DIR)
	cp $(GRUB_CONFIG) $(IMG_GRUB_DIR)
	$(CROSS_GRUB)-mkrescue -o $@ $(IMG_WORK_DIR)

$(DIRS) :
	mkdir -p $@

qemu-run : image
	$(QEMU_COMMAND) -cdrom $(IMG_TARGET)

clean :
	$(MAKE) -C kernel clean
	$(MAKE) -C libc clean
	$(RM) -r $(CLEAN_DIRS)

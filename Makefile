include make.config

.PHONY : all pre-install kernel libc install image qemu-run clean dependencies

PWD != pwd

export HOST       != utils/default-host.sh
export BOOT_TYPE  != utils/default-boot.sh
export PREFIX     ?= /usr
export EXEC_PREFIX = $(PREFIX)
export BOOTDIR     = /boot
export LIBDIR      = $(EXEC_PREFIX)/lib
export INCLUDEDIR  = $(PREFIX)/include
export PROG_NAME   = zxos

HOSTARCH    != utils/target-triplet-to-arch.sh $(HOST)
KERNEL_NAME := $(PROG_NAME).$(HOSTARCH)
KERNEL_DIR  := kernel
LIBC_DIR    := libc
DEP_DIR     ?= dep

export KERNEL_NAME

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

QEMU             ?= qemu
QEMU_COMMAND     := $(QEMU)-system-$(HOSTARCH)
GRUB_CONFIG      := $(IMG_GRUB_DIR)/grub.cfg
GRUB_CONFIG_TMP  := utils/grub.cfg.template

DIRS=\
$(IMGDIR) \
$(IMG_WORK_DIR) \
$(IMG_BOOT_DIR) \
$(IMG_GRUB_DIR) \
$(SYSROOT) \
$(DEP_DIR) \

CLEAN_DIRS=\
$(IMGDIR) \
$(IMG_WORK_DIR) \
$(IMG_BOOT_DIR) \
$(IMG_GRUB_DIR) \
$(SYSROOT) \

all : libc kernel

kernel : pre-install
	$(MAKE) -C $(KERNEL_DIR)

libc : pre-install
	$(MAKE) -C $(LIBC_DIR)

pre-install : | $(SYSROOT)
	$(MAKE) -C $(LIBC_DIR) install-headers
	$(MAKE) -C $(KERNEL_DIR) install-headers
	$(MAKE) -C $(LIBC_DIR) install-libs

install :
	$(MAKE) -C $(LIBC_DIR) install
	$(MAKE) -C $(KERNEL_DIR) install

image : install $(IMG_TARGET)

$(IMG_TARGET) : | $(IMG_GRUB_DIR) $(IMG_BOOT_DIR) $(IMG_WORK_DIR) $(GRUB_CONFIG)
	cp $(KERNEL_TARGET) $(IMG_BOOT_DIR)
	$(CROSS_GRUB)-mkrescue -o $@ $(IMG_WORK_DIR)

$(GRUB_CONFIG) : | $(GRUB_CONFIG_TMP)
	cp $(GRUB_CONFIG_TMP) $(GRUB_CONFIG)
	sed -i "s/{PROG_NAME}/$$PROG_NAME/g" $(GRUB_CONFIG)
	sed -i "s/{BOOT_TYPE}/$$BOOT_TYPE/g" $(GRUB_CONFIG)
	sed -i "s/{KERNEL_NAME}/$$KERNEL_NAME/g" $(GRUB_CONFIG)

$(DIRS) :
	mkdir -p $@

qemu-run : image
	$(QEMU_COMMAND) -cdrom $(IMG_TARGET)

clean :
	$(MAKE) -C kernel clean
	$(MAKE) -C libc clean
	$(RM) -r $(CLEAN_DIRS)

dependencies : | $(DEP_DIR)
	env WORK_DIR=$(DEP_DIR) \
	PREFIX=$(CROSS_PATH) \
	TARGET=$(HOST) \
	utils/install-dependencies.sh

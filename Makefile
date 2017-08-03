.PHONY : all clean check image qemu-run

check_defined = \
  $(strip $(foreach 1,$1, \
    $(call __check_defined,$1,$(strip $(value 2)))))
__check_defined = \
  $(if $(value $1),, \
    $(error Undefined $1$(if $2, ($2))))

BINDIR := bin
OBJDIR := obj
IMGDIR := image
IMG_WORK_DIR := $(IMGDIR)/iso
IMG_BOOT_DIR := $(IMG_WORK_DIR)/boot
IMG_GRUB_DIR := $(IMG_BOOT_DIR)/grub

CLEAN_DIRS := $(BINDIR) $(OBJDIR) $(IMGDIR)

BUILD_TARGET := $(BINDIR)/myos.bin
IMG_TARGET := $(IMGDIR)/myos.iso
BOOTSTRAP_SRC := boot.s
KERNEL_SRC := kernel.c
BOOTSTRAP_OBJ := $(OBJDIR)/boot.o
KERNEL_OBJ := $(OBJDIR)/kernel.o
LINKER_SCRIPT := linker.ld
GRUB_CONFIG := grub.cfg

TARGET := i686-elf
PLAT := x86
BOOT_TYPE := multiboot
QEMU_PLAT := i386
CROSS_PATH := /usr/local/cross

CROSS_AS := $(CROSS_PATH)/bin/$(TARGET)-as
CROSS_CC := $(CROSS_PATH)/bin/$(TARGET)-gcc
CROSS_LINKER := $(CROSS_PATH)/bin/$(TARGET)-gcc
CROSS_GRUB := $(CROSS_PATH)/bin/grub
QEMU := qemu

CFLAGS := -std=gnu99 -ffreestanding -O2 -Wall -Wextra
LINKFLAGS := -ffreestanding -O2 -nostdlib -lgcc

CHECK := $(CROSS_GRUB)-file --is-$(PLAT)-$(BOOT_TYPE) $(BUILD_TARGET)
QEMU_COMMAND := $(QEMU)-system-$(QEMU_PLAT)

all : $(BUILD_TARGET)
	@:$(call check_defined, TARGET, target info)
	@:$(call check_defined, CROSS_PATH, path to cross-platform utils)

image : all $(IMG_TARGET)
	@:$(call check_defined, PLAT, platform info)
	@:$(call check_defined, BOOT_TYPE, boot type)

$(BUILD_TARGET) : $(BOOTSTRAP_OBJ) $(KERNEL_OBJ) | $(BINDIR) $(LINKER_SCRIPT)
	$(info Linking target.)
	$(CROSS_LINKER) -T $(LINKER_SCRIPT) $^ $(LINKFLAGS) -o $@
	$(info Build complete. Performing sanity check.)
	$(CHECK)
	@if [ $$? -ne 0 ]; then\
		echo "Sanity check failed!";\
		exit $$?;\
	else\
		echo "Sanity check succeeded.";\
	fi

$(BOOTSTRAP_OBJ) : $(BOOTSTRAP_SRC) | $(OBJDIR)
	$(info Assembling bootstrap.)
	$(CROSS_AS) $^ -o $@

$(KERNEL_OBJ) : $(KERNEL_SRC) | $(OBJDIR)
	$(info Compiling kernel.)
	$(CROSS_CC) -c $^ $(CFLAGS) -o $@

$(IMG_TARGET) : $(BUILD_TARGET) $(GRUB_CONFIG)
	mkdir -p $(IMG_GRUB_DIR)
	cp $(BUILD_TARGET) $(IMG_BOOT_DIR)
	cp $(GRUB_CONFIG) $(IMG_GRUB_DIR)
	$(CROSS_GRUB)-mkrescue -o $@ $(IMG_WORK_DIR)

$(BINDIR) :
	mkdir -p $@

$(OBJDIR) :
	mkdir -p $@

check :
	$(info Performing sanity check.)
	$(CHECK)
	@if [ $$? -ne 0 ]; then\
		echo "Sanity check failed!";\
		exit $$?;\
	else\
		echo "Sanity check succeeded.";\
	fi

qemu-run : image
	@:$(call check_defined, QEMU_PLAT, qemu target platform)
	$(QEMU_COMMAND) -cdrom $(IMG_TARGET)

clean :
	$(RM) -r $(CLEAN_DIRS)

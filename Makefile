.PHONY : all clean check

THIS := $(lastword $(MAKEFILE_LIST))

BINDIR := bin
OBJDIR := obj

TARGET := $(BINDIR)/myos.bin
BOOTSTRAP_SRC := boot.s
KERNEL_SRC := kernel.c
BOOTSTRAP_OBJ := $(OBJDIR)/boot.o
KERNEL_OBJ := $(OBJDIR)/kernel.o
LINKER_SCRIPT := linker.ld

CROSS_AS := i686-elf-as
CROSS_CC := i686-elf-gcc
CROSS_LINKER := i686-elf-gcc

CFLAGS := -std=gnu99 -ffreestanding -O2 -Wall -Wextra
LINKFLAGS := -ffreestanding -O2 -nostdlib -lgcc

CHECK := grub-file --is-x86-multiboot $(TARGET)

all : $(TARGET)

$(TARGET) : $(BOOTSTRAP_OBJ) $(KERNEL_OBJ) | $(BINDIR) $(LINKER_SCRIPT)
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

$(BINDIR) :
	mkdir $@

$(OBJDIR) :
	mkdir $@

check :
	$(info Performing sanity check.)
	$(CHECK)
	@if [ $$? -ne 0 ]; then\
		echo "Sanity check failed!";\
		exit $$?;\
	else\
		echo "Sanity check succeeded.";\
	fi

clean :
	$(RM) -r $(BINDIR) $(OBJDIR)

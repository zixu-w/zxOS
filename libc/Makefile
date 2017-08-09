include ../make.config

DEFAULT_HOST != ../utils/default-host.sh
HOST         ?= DEFAULT_HOST
HOSTARCH     != ../utils/target-triplet-to-arch.sh $(HOST)

CFLAGS   ?= -O2 -g
CPPFLAGS ?=
LDFLAGS  ?=
LIBS     ?=

DESTDIR     ?=
PREFIX      ?= /usr/local
EXEC_PREFIX ?= $(PREFIX)
BOOTDIR     ?= $(EXEC_PREFIX)/boot
INCLUDEDIR  ?= $(PREFIX)/include

CFLAGS        := $(CFLAGS) -ffreestanding -Wall -Wextra
CPPFLAGS      := $(CPPFLAGS) -D__is_libc -Iinclude
LIBK_CFLAGS   := $(CFLAGS)
LIBK_CPPFLAGS := $(CPPFLAGS) -D__is_libk

ARCHDIR := arch/$(HOSTARCH)

include $(ARCHDIR)/make.config

AUX_DIRS=\
$(DESTDIR)$(INCLUDEDIR) \
$(DESTDIR)$(BOOTDIR) \
$(DESTDIR)$(LIBDIR) \

CFLAGS        := $(CFLAGS) $(ARCH_CFLAGS)
CPPFLAGS      := $(CPPFLAGS) $(ARCH_CPPFLAGS)
LIBK_CFLAGS   := $(LIBK_CFLAGS) $(KERNEL_ARCH_CFLAGS)
LIBK_CPPFLAGS := $(LIBK_CPPFLAGS) $(KERNEL_ARCH_CPPFLAGS)

FREEOBJS=\
$(ARCH_FREEOBJS) \
stdio/printf.o \
stdio/putchar.o \
stdio/puts.o \
stdlib/abort.o \
string/memcmp.o \
string/memcpy.o \
string/memmove.o \
string/memset.o \
string/strlen.o \

HOSTEDOBJS=\
$(ARCH_HOSTEDOBJS) \

OBJS=\
$(FREEOBJS) \
$(HOSTEDOBJS) \

LIBK_OBJS = $(FREEOBJS:.o=.libk.o)

#BINARIES=libc.a libk.a # Not ready for libc yet.
BINARIES = libk.a

.PHONY: all clean install install-headers install-libs
.SUFFIXES: .o .libk.o .c .S

all : $(BINARIES)

libc.a : $(OBJS) | $(BIN_DIR)
	@:$(call check_defined, CROSS_AR, cross platform ar)
	$(CROSS_AR) rcs $@ $(OBJS)

libk.a : $(LIBK_OBJS) | $(BIN_DIR)
	@:$(call check_defined, CROSS_AR, cross platform ar)
	$(CROSS_AR) rcs $@ $(LIBK_OBJS)

.c.o :
	@:$(call check_defined, CROSS_CC, cross platform compiler)
	$(CROSS_CC) -MD -c $< -o $@ -std=gnu11 $(CFLAGS) $(CPPFLAGS)

.c.S :
	@:$(call check_defined, CROSS_CC, cross platform compiler)
	$(CROSS_CC) -MD -c $< -o $@ $(CFLAGS) $(CPPFLAGS)

.c.libk.o :
	@:$(call check_defined, CROSS_CC, cross platform compiler)
	$(CROSS_CC) -MD -c $< -o $@ -std=gnu11 $(LIBK_CFLAGS) $(LIBK_CPPFLAGS)

.S.libk.o :
	@:$(call check_defined, CROSS_AS, cross platform assembler)
	$(CROSS_AS) -c $< -o $@

clean :
	$(RM) $(BINARIES) *.a
	$(RM) $(OBJS) $(LIBK_OBJS) *.o */*.o */*/*.o
	$(RM) $(OBJS:.o=.d) $(LIBK_OBJS:.o=.d) *.d */*.d */*/*.d

install : install-headers install-libs

install-headers : | $(DESTDIR)$(INCLUDEDIR)
	cp -R --preserve=timestamps include/. $(DESTDIR)$(INCLUDEDIR)/.

install-libs: $(BINARIES) | $(DESTDIR)$(LIBDIR)
	cp $(BINARIES) $(DESTDIR)$(LIBDIR)

$(AUX_DIRS) :
	mkdir -p $@

-include $(OBJS:.o=.d)
-include $(LIBK_OBJS:.o=.d)

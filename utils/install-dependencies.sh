#!/bin/bash
set -e

BINUTILS_VER=2.29
GCC_VER=7.1.0
GRUB_VER=2.02

export PATH="$PREFIX/bin:$PATH"

pushd $WORK_DIR

rm -rf binutils-$BINUTILS_VER gcc-$GCC_VER grub-$GRUB_VER
rm -rf build-binutils build-gcc build-grub

if [ ! -f binutils-$BINUTILS_VER.tar.gz ]; then
  wget https://ftp.gnu.org/gnu/binutils/binutils-$BINUTILS_VER.tar.gz
else
  echo "binutils-$BINUTILS_VER.tar.gz exists."
fi

if [ ! -f gcc-$GCC_VER.tar.gz ]; then
  wget https://ftp.gnu.org/gnu/gcc/gcc-$GCC_VER/gcc-$GCC_VER.tar.gz
else
  echo "gcc-$GCC_VER.tar.gz exists."
fi

if [ ! -f grub-$GRUB_VER.tar.gz ]; then
  wget https://ftp.gnu.org/gnu/grub/grub-$GRUB_VER.tar.gz
else
  echo "grub-$GRUB_VER.tar.gz exists."
fi

tar xvf binutils-$BINUTILS_VER.tar.gz
tar xvf gcc-$GCC_VER.tar.gz
tar xvf grub-$GRUB_VER.tar.gz

mkdir build-binutils build-gcc build-grub

pushd build-binutils

. ../binutils-$BINUTILS_VER/configure \
--target=$TARGET --prefix="$PREFIX" --with-sysroot \
--disable-nls --disable-werror

make -j
make install -j

popd

which -- $TARGET-as || echo "$TARGET-as is not in the PATH"

pushd build-gcc

. ../gcc-$GCC_VER/configure \
--target=$TARGET --prefix="$PREFIX" \
--disable-nls --enable-languages=c,c++ --without-headers

make all-gcc -j
make all-target-libgcc -j
make install-gcc -j
make install-target-libgcc -j

popd

pushd build-grub

. ../grub-$GRUB_VER/configure \
--target=$TARGET --prefix="$PREFIX" \
--disable-nls --disable-werror

make -j
make install -j

popd

popd

#!/bin/bash
set -e

echo
echo "============================================="
echo "Installing tool-chain and required libraries."
echo "Please enter password for sudo when prompted."
echo "============================================="
echo
sleep 2

# TODO: more compatible with other systems
yes | sudo apt-get install libgmp3-dev libmpfr-dev libmpc-dev texinfo bison flex

BINUTILS_VER=2.29
GCC_VER=7.1.0
GRUB_VER=2.02

PATH="$PREFIX/bin:$PATH"

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

../binutils-$BINUTILS_VER/configure \
--target=$TARGET --prefix="$PREFIX" --with-sysroot \
--disable-nls --disable-werror

make -j4
sudo make install -j4

popd

which -- $TARGET-as || echo "$TARGET-as is not in the PATH"

pushd build-gcc

../gcc-$GCC_VER/configure \
--target=$TARGET --prefix="$PREFIX" \
--disable-nls --enable-languages=c,c++ --without-headers

make all-gcc -j4
make all-target-libgcc -j4
sudo make install-gcc -j4
sudo make install-target-libgcc -j4

popd

pushd build-grub

../grub-$GRUB_VER/configure \
--target=$TARGET --prefix="$PREFIX" \
--disable-nls --disable-werror

make -j4
sudo make install -j4

popd

popd

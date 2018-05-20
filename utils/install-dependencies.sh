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

NPROC=$(nproc)

BINUTILS_VER=2.29
GCC_VER=7.1.0
GRUB_VER=2.02

PATH="$PREFIX/bin:$PATH"

pushd $WORK_DIR

rm -rf binutils-$BINUTILS_VER gcc-$GCC_VER grub-$GRUB_VER
rm -rf build-binutils build-gcc build-grub

if [ ! -f binutils-$BINUTILS_VER.tar.gz ]; then
  echo "Dowloading binutils-$BINUTILS_VER.tar.gz ..."
  wget https://ftp.gnu.org/gnu/binutils/binutils-$BINUTILS_VER.tar.gz
else
  echo "binutils-$BINUTILS_VER.tar.gz exists."
fi

if [ ! -f gcc-$GCC_VER.tar.gz ]; then
  echo "Dowloading gcc-$GCC_VER.tar.gz ..."
  wget https://ftp.gnu.org/gnu/gcc/gcc-$GCC_VER/gcc-$GCC_VER.tar.gz
else
  echo "gcc-$GCC_VER.tar.gz exists."
fi

if [ ! -f grub-$GRUB_VER.tar.gz ]; then
  echo "Dowloading grub-$GRUB_VER.tar.gz ..."
  wget https://ftp.gnu.org/gnu/grub/grub-$GRUB_VER.tar.gz
else
  echo "grub-$GRUB_VER.tar.gz exists."
fi

tar xvf binutils-$BINUTILS_VER.tar.gz
tar xvf gcc-$GCC_VER.tar.gz
tar xvf grub-$GRUB_VER.tar.gz

mkdir build-binutils build-gcc build-grub

pushd build-binutils

echo "Configuring binutils-$BINUTILS_VER ..."

../binutils-$BINUTILS_VER/configure \
--target=$TARGET --prefix="$PREFIX" --with-sysroot \
--disable-nls --disable-werror

echo "Building binutils-$BINUTILS_VER with $NPROC threads ..."

make -j$NPROC
sudo make install -j$NPROC

popd

which -- $TARGET-as || echo "$TARGET-as is not in the PATH"

pushd build-gcc

echo "Configuring gcc-$GCC_VER ..."

../gcc-$GCC_VER/configure \
--target=$TARGET --prefix="$PREFIX" \
--disable-nls --enable-languages=c,c++ --without-headers

echo "Building gcc-$GCC_VER with $NPROC threads ..."

make all-gcc -j$NPROC
make all-target-libgcc -j$NPROC
sudo make install-gcc -j$NPROC
sudo make install-target-libgcc -j$NPROC

popd

pushd build-grub

echo "Configuring grub-$GRUB_VER ..."

../grub-$GRUB_VER/configure \
--target=$TARGET --prefix="$PREFIX" \
--disable-nls --disable-werror

echo "Building grub-$GRUB_VER with $NPROC threads ..."

make -j$NPROC
sudo make install -j$NPROC

popd

popd

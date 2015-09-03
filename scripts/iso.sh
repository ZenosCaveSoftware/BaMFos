#!/bin/sh
set -e
. ./build.sh

mkdir -p ../isodir
mkdir -p ../isodir/boot
mkdir -p ../isodir/boot/grub

cp ../sysroot/boot/BaMF.kernel ../isodir/boot/BaMF.kernel
cat > ../isodir/boot/grub/grub.cfg << EOF
menuentry "BaMF-OS" {
	multiboot /boot/BaMF.kernel
}
EOF
grub-mkrescue -o ../BaMF.iso ../isodir
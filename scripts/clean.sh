#!/bin/sh
set -e
. ./config.sh

$MAKE -C ../src clean

rm -rfv ../sysroot
rm -rfv ../isodir
rm -rfv ../BaMF.iso

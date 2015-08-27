#!/bin/sh
set -e
. ./headers.sh

DESTDIR="$PWD/../sysroot" $MAKE -C ../src install

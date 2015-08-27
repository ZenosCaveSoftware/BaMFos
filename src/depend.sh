#!/bin/sh
gcc -MM -MG "$@" | sed -e ’s@ˆ\(.*\)\.o:@\1.d \1.o:@’
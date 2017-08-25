BaMF. An OS.
==============================
Started as a Meaty Skeleton Kernel (found 
[here](https://gitlab.com/sortie/meaty-skeleton)).

I hope to grow this OS into a simple development environment. 
This is aimed at software development solely.


Setup
------------------------------
To compile the source you will need a i686-elf-gcc cross compiler.
Follow the instructions here for how to do so.
http://wiki.osdev.org/GCC_Cross-Compiler 

Compilation
------------------------------
A typical easy compilation can be done with ```./build```

Emulation
------------------------------
A typical easy emulation can be done with ```./qemu```


Provided scripts:
------------------------------
- [build](scripts/build) invokes [headers](scripts/headers) and runs the main make system.
- [clean](scripts/clean) cleans the source tree.
- [config](scripts/config) sets environment variables needed for compilation.
- [default](scripts/default)-host is used by config if HOST is not set should echo the arch to be built.
- [headers](scripts/headers) installs system headers into sysroot. Invokes [config](scripts/config).
- [iso](scripts/iso) builds the main iso for distrobution and installation.
- [qemu](scripts/qemu) runs qemu for the BaMF.iso. Invokes [iso](scripts/iso).
- [target](scripts/target)-triple-to-arch is used by [qemu](scripts/qemu) to set host triplet if needed.

Contributions:
-----------------------------
Please refer to [CONTRIB](CONTRIB.md)
for a list of contributors and contribution guides.

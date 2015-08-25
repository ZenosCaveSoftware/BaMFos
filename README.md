BaMF. An OS.
==============================
Started as a Meaty Skeleton Kernel. found [here](https://gitlab.com/sortie/meaty-skeleton)

I hope to grow this OS into a simple development environment aimed at software development solely.


Setup
------------------------------
To compile the source you will need a i686-elf-gcc cross compiler.
Follow the instructions here for how to do so.
http://wiki.osdev.org/GCC_Cross-Compiler 

Compilation
------------------------------
A typical easy compilation can be done with ```./build.sh```

Emulation
------------------------------
A typical easy emulation can be done with ```./qemu.sh```


Provided scripts:
------------------------------
- ./build.sh invokes ./headers.sh and runs the main make system.
- ./clean.sh cleans the source tree
- ./config.sh sets environment variables needed for compilation
- ./default-host.sh is used by config if HOST is not set. returns i686-elf.
- ./headers.sh installs system headers into sysroot. Invokes ./config.sh
- ./iso.sh builds the main iso for distrobution and installation.
- ./qemu.sh runs qemu for the BaMF.iso. Invokes ./iso.sh.
- ./target-triple-to-arch.sh is used by ./qemu.sh to set host triplet if needed.




Note:before running make, but after running ```../binutils-x.y.z ...```, running ```echo "MAKEINFO = :" >> Makefile``` 
takes care of the error you will possibly recieve other wise. 

**ONLY TRY THIS IF THE BUILD FAILS WITHOUT FIRST**



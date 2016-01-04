#define virt 0xffffffff80000000
#define phys 0x0000000000100000
#define KA2P(addr) (addr) - virt + phys
#ifndef _PANIC_H
#define _PANIC_H 1

#define PANIC(msg) panic(msg, __FILE__, __LINE__);
extern void panic(const char *message, const char *file, uint32_t line);

#endif

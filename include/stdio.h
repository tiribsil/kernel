#ifndef STDIO_H
#define STDIO_H

#include <stdarg.h>
#include <syscall.h>

void putc(char c);
int puts(const char* str);
char getc();
char* gets(char* buf);

void printf(const char *format, ...);

#endif // STDIO_H

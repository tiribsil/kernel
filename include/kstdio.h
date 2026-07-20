#ifndef KSTDIO_H
#define KSTDIO_H

#include <stdarg.h>

// Inicializacao
void kstdio_init(void);

// Output
void kprintf(const char *format, ...);
void kputs(const char *str);
void kputc(char c);

// Input
int kgetc(void);
void kgets(char *buf, int max_len);

// Utilidade
int kstdio_has_data(void);

#endif // KSTDIO_H

#include <stdio.h>

void putc(char c) {
  write(&c, 1);
}

int puts(const char *str) {
  int len = 0;
  while(*(str + len)) len++;
  write((char*)str, len);
  return len;
}

char getc() {
  char c;
  read(&c, 1);
  return c;
}

char* gets(char *buf) {
  int index = 0;
  char c;
  while (1) {
    read(&c, 1);
    if (c == '\r' || c == '\n') {
      write("\n", 1);
      buf[index] = '\0';
      break;
    } else if (c == '\b' || c == 0x7F) {
      if (index > 0) {
        index--;
	write("\b \b", 3);
      }
    } else if (c >= 32 && c <= 126) {
      buf[index++] = c;
      write(&c, 1);
    }
  }
  return buf;
}

// Printa um inteiro sem sinal
// Usa um buffer para inverter os digitos no final
static void print_uint_base(unsigned num, int base) {
  char buffer[65];
  int i = 0;

  if (num == 0) {
    putc('0');
    return;
  }

  while (num > 0) {
    unsigned long long remainder = num % (unsigned)base;
    if (remainder < 10) {
      buffer[i++] = (char)('0' + remainder);
    } else {
      buffer[i++] = (char)('a' + (remainder - 10));
    }
    num /= (unsigned)base;
  }

  while (i > 0) {
    putc(buffer[--i]);
  }
}

// Printa inteiro com sinal em base qualquer
static void print_int_base(int num, int base) {
  if (num < 0) {
    putc('-');
    num = -num;
  }

  print_uint_base((unsigned)num, base);
}

// printf simplificado
// Suporta %c %s %d %u %x
void printf(const char *format, ...) {
  va_list args;
  va_start(args, format);

  while (*format != '\0') {
    if (*format == '%') {
      format++;

      // Suporte simples a precisao (%.Nf) igual o printf nativo
      int precision = -1;

      if (*format == '.') {
        format++;
        precision = 0;

        while (*format >= '0' && *format <= '9') {
          precision = (precision * 10) + (*format - '0');
          format++;
        }
      }

      int length_mod = 0;
      if (*format == 'l') {
        length_mod++;
        format++;
      }

      switch (*format) {
      case 'c': {
        char c = (char)va_arg(args, int);
        putc(c);
        break;
      }
      case 's': {
        char *s = va_arg(args, char *);
        if (s) {
          puts(s);
        } else {
          puts("(null)");
        }
        break;
      }
      case 'd': {
	int n = va_arg(args, int);
        print_int_base(n, 10);
        break;
      }
      case 'u': {
	unsigned u = va_arg(args, unsigned);
        print_uint_base(u, 10);
        break;
      }
      case 'x': {
        unsigned int x = va_arg(args, unsigned int);
        puts("0x");
        print_uint_base(x, 16);
        break;
      }
      case '%': {
        putc('%');
        break;
      }
      default: {
        putc('%');
        putc(*format);
        break;
      }
      }
    } else {
      putc(*format);
    }
    format++;
  }

  va_end(args);
}

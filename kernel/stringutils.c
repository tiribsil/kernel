#include <stringutils.h>

// Implementacao basica do strcomp
int strcmp(const char *s1, const char *s2) {
  while (*s1 && (*s1 == *s2)) {
    s1++;
    s2++;
  }
  return *(const unsigned char *)s1 - *(const unsigned char *)s2;
}

int strlen(const char* s){
    int len = 0;
    while(s[len]) len++;
    return len;
}

int atoi(char* str) {
  int sign = 1;
  char* p = str;
  int result = 0;

  while (*p == ' ') p++;

  if (*p == '-') {
    sign = -1;
    p++;
  }
  else if (*p == '+') p++;

  while (*p >= '0' && *p <= '9') {
    result = result * 10 + (*p - '0');
    p++;
  }

  return sign * result;
}

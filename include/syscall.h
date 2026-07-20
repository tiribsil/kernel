#ifndef SYSCALL_H
#define SYSCALL_H

/*
Serve apenas como definição das funções que fazem chamadas de sistema,
permite o acesso delas a partir de código C.
*/

int printf(const char *s);
int abort(void);

int fork(void);
void exec(void (*programa)(void));
void exit(void);

#endif // SYSCALL_H

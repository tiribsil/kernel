#include <programas.h>
#include <syscall.h>

void programainicio(void) {
    printf("Processo rodando programa inicio\n");

    if(!fork()) exec(programa1);
    if(!fork()) exec(programa2);
    exit();
}

void programa1(void) {
    while(1) {
        printf("Processo rodando programa 1\n");
        for(volatile int i = 0; i < 50000000; i++); 
    }
}

void programa2(void) {
    while(1) {
        printf("Processo rodando programa 2\n");
        for(volatile int i = 0; i < 50000000; i++); 
    }
}

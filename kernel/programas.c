#include <programas.h>
#include <serial.h>
#include <syscall.h>

void programainicio(void) {
    serial_puts("Processo rodando programa inicio\n");

    if(!fork()) exec(programa1);
    if(!fork()) exec(programa2);
    exit();
}

void programa1(void) {
    while(1) {
        serial_puts("Processo rodando programa 1\n");
        for(volatile int i = 0; i < 50000000; i++); 
    }
}

void programa2(void) {
    while(1) {
        serial_puts("Processo rodando programa 2\n");
        for(volatile int i = 0; i < 50000000; i++); 
    }
}

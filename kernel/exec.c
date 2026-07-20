#include <process.h>
#include <serial.h>
#include <timer.h>

// Importa rótulo do linker script. Aqui é onde acaba o código do executável.
extern char __code_data_end[];

void sys_exec(void (*programa)(void)) {
    // copia as intrucoes das funcoes para a memoria do processo
    process p = current;
    
    // Aqui, a memória do processo vai ser todo o código entre o início
    // da função "programa" e o fim do código do executável.
    char* inicio = (char*)0x40000000;
    unsigned int tamanho_codigo = (unsigned int)__code_data_end - 0x40000000;
    for(unsigned int i = 0; i < tamanho_codigo; i++) p->mem[i] = inicio[i];
    p->msize = tamanho_codigo; // atualiza o tamanho da memoria

    unsigned int offset = ((unsigned int)programa) - 0x40000000;
    unsigned int entry_point = (unsigned int)p->mem + offset;

    unsigned int usrstack_top = (unsigned int) (p->mem + PROCESS_SIZE);

    p->tf = (struct trapframe*)((unsigned)p->kstack + SIZE_16KB - sizeof(struct trapframe));

    // zeramos os registradores apenas por questoes de seguranca e para facilitar a verificacao da execucao do exec
    p->tf->r0 = 0; 
    p->tf->r1 = 0; 
    p->tf->r2 = 0; 
    p->tf->r3 = 0;
    p->tf->r4 = 0; 
    p->tf->r5 = 0; 
    p->tf->r6 = 0; 
    p->tf->r7 = 0;
    p->tf->r8 = 0; 
    p->tf->r9 = 0; 
    p->tf->r10 = 0; 
    p->tf->r11 = 0;
    p->tf->r12 = 0;

    // atualiza os ponteiros de pilha e pc
    p->tf->pc_usr = entry_point;
    p->tf->sp_usr = usrstack_top;
    p->tf->lr_usr = 0;

    // coloca o resultado do exec para executar em modo usuario
    p->tf->cpsr_usr = 0x30;

    return; 
}

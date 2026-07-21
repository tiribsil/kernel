#include <process.h>
#include <scheduler.h>
#include <interrupts_handler.h>
#include <pmm.h>

#ifndef NULL
#define NULL ((void*)0)
#endif

process current = NULL;

pid_t used_pids[MAX_PROCESS_COUNT] = {0};
pid_t next_pid = 1;

pid_t create_pid(){
    for(int i = 0; i < MAX_PROCESS_COUNT; i++){
        pid_t pid = next_pid;
        next_pid++;

        if(next_pid >= MAX_PROCESS_COUNT){
            next_pid = 1;
        }

        if(!used_pids[pid]){
            used_pids[pid] = 1;
            return pid;
        }
    }
    return MAX_PROCESS_COUNT;
}

void free_pid(pid_t pid){
    used_pids[pid] = 0;
}

void sys_exit(){
    pll_node* current_pll_node = pcb_get_node_pid(current->pid);     // Atualiza list_location e retorna o no da fila
    pcb_remove(list_location, current_pll_node);                     // Remove o no da fila

    free_pid(current->pid);                                          // Libera o pid
    current->state = ZOMBIE;                                         // Muda o estado para Zombie
    pcb_elect();                                                     // Chama o escalonador
}

// salvar o contexto do processo
// chamar fork
// fork:
// criar uma struct de processo
// copiar tudo do current
// mudar o registrador de retorno do current para o pid do processo criado
// mudar o registrador de retorno do processo criado para 0
// inserir o processo no bloco de processos ou inserir em uma fila do escalonador

process process_table[MAX_PROCESS_COUNT] = {0};

// funcao de retorno para processos recém clonados
extern void irq_return(void);

void fork_return(void){
    irq_end_current();
    enable_cpu_interrupts();
    irq_return();
}

void sys_exec(void (*programa)(void)) {
    // copia as intrucoes das funcoes para a memoria do processo
    process p = current;

    unsigned int usrstack_top = (unsigned int)(p->usr_stack + PAGE_SIZE);

    p->tf = (struct trapframe*)((unsigned)p->kstack + PAGE_SIZE - sizeof(struct trapframe));

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
    p->tf->pc_usr = (unsigned int)programa | 1;
    p->tf->sp_usr = usrstack_top;
    p->tf->lr_usr = 0;

    // coloca o resultado do exec para executar em modo usuario
    p->tf->cpsr_usr = 0x30;

    return;
}

int sys_fork(){
    pid_t pid = create_pid(); // gera um pid

    process newprocess = (process)pmm_alloc_block(); // a ser substituido por malloc
    newprocess->usr_stack = pmm_alloc_block(); // a ser substituido por malloc
    newprocess->kstack = pmm_alloc_block(); // a ser substituido por malloc

    newprocess->parent = current;  // define o pai
    newprocess->state = READY; // coloca como pronto
    newprocess->pid = pid;
    newprocess->priority = current->priority;

    // copia toda as pilhas do pai para o filho
    for(int i = 0; i < PAGE_SIZE; i++){
        newprocess->kstack[i] = newprocess->parent->kstack[i];
	newprocess->usr_stack[i] = newprocess->parent->usr_stack[i];
    }

    newprocess->tf = (struct trapframe*)((char *)newprocess->kstack + ((char *)newprocess->parent->tf - (char *)newprocess->parent->kstack)); // calcula o topo da pilha do filho com base na do pai

    // calcula os offsets necessarios para o filho ficar na sua regiao de memoria

    newprocess->tf->pc_usr = newprocess->parent->tf->pc_usr;
    newprocess->tf->lr_usr = newprocess->parent->tf->lr_usr;

    unsigned int sp_offset = newprocess->parent->tf->sp_usr - (unsigned int)newprocess->parent->usr_stack;
    newprocess->tf->sp_usr = (unsigned int)newprocess->usr_stack + sp_offset;

    // prepara os retornos diferentes
    newprocess->parent->tf->r0 = newprocess->pid;
    newprocess->tf->r0 = 0;
    // configura os frame pointers
    unsigned int r11_offset = newprocess->parent->tf->r11 - (unsigned int)newprocess->parent->usr_stack;
    newprocess->tf->r11 = (unsigned int)newprocess->usr_stack + r11_offset;

    newprocess->context.sp = (unsigned int)newprocess->tf;
    newprocess->context.lr = (unsigned int)fork_return; // prepara o retorno da troca de contexto para fork return

    // inserir na fila de maior prioridade do escalonador
    pll_node* newprocess_block_node = pll_node_new(newprocess);
    pcb_insert(0, newprocess_block_node);

   return current->tf->r0;
}

void first_process(void (*programa)(void)){
    pid_t pid = create_pid(); // gera um pid

    process newprocess = (process)pmm_alloc_block();  // ira ser substituido por malloc
    newprocess->usr_stack = pmm_alloc_block();  // ira ser substituido por malloc
    newprocess->kstack = pmm_alloc_block();  // ira ser substituido por malloc

    newprocess->parent = NULL;
    newprocess->state = READY; // comeca pronto
    newprocess->pid = pid;
    newprocess->priority = 0;

    // inserir na fila de maior prioridade do escalonador
    pll_node* newprocess_block_node = pll_node_new(newprocess);
    pcb_insert(0, newprocess_block_node);

    current = newprocess;

    sys_exec(programa); //executa o programa inicial
    irq_return();
}

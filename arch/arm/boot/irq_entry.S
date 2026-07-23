.section .text
.equ TRAPFRAME_OFFSET, 8
.extern current
.global irq_entry 
.type irq_entry, %function

irq_entry:
   @ código considera que o hardware fez automaticamente:
   @ trocou para SP_irq, que é a stack do kernel nesse momento
   @ salvou o CPSR do processo em SPSR_irq
   @ salvou o PC do processo + 4 em LR_irq

   sub lr, lr, #4

   @ r0–r12 ainda pertencem ao contexto interrompido 

   str r0, [sp, #-4]             @ salva momentaneamente r0 e r1
   str r1, [sp, #-8]

   mov r1, sp                    @ usamos o r1 recém-liberado para guardar o SP_irq
   
   ldr r0, =current              @ carrega o endereço da variável global estática current
   ldr r0, [r0]                  @ carrega o valor contido em current (endereço do processo atual)

   ldr sp, [r0, #TRAPFRAME_OFFSET]  @ carrega o valor do campo tf do processo atual como nova pilha
   
   ldr r0,[r1, #-4]              @ restaura r0 e r1
   ldr r1,[r1, #-8]

   sub sp, sp, #(17 * 4)         @ temos 17 variáveis na struct
   stmia sp, {r0-r12}            @ salva R0–R12 na stack

   str lr, [sp, #(15 * 4)]       @ salva o PC na pilha PRIMEIRO!
   
   mrs r12, spsr                  @ pega o CPSR do processo
   str r12, [sp, #(16 * 4)]       @ salva o CPSR na pilha
   
   add r12, sp, #(13*4)     
   stmia r12, {sp, lr}^           @ salva sp_usr e lr_usr POR ÚLTIMO
   nop                            @ Esvazia o pipeline para garantir

   ldr r12, =current              @ carrega o endereço da variável global estática current
   ldr r12, [r12]                 @ carrega o valor contido em current (endereço do processo atual)

   str sp,[r12, #TRAPFRAME_OFFSET] @ atualiza o valor de tf

   mov r0, sp                     @ Salva nosso SP_irq temporariamente em r0
   
   mrs r1, cpsr
   bic r1, r1, #0x1F
   orr r1, r1, #0x13              @ Configura os bits para Modo SVC (0x13)
   msr cpsr_c, r1                 @ O GATILHO: A CPU agora está em SVC!
   
   mov sp, r0                     @ Entrega o ponteiro da pilha pro SP_svc
   
   blx irq_dispatcher_c           @ Chama o C (agora o Escalonador roda blindado)

   mrs r1, cpsr
   bic r1, r1, #0x1F
   orr r1, r1, #0x12              @ Devolve os bits para Modo IRQ (0x12)
   msr cpsr_c, r1                 @ A CPU volta para IRQ

   b irq_return                  @ código que vai retornar ao processo

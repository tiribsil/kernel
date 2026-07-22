.section .text
.equ TRAPFRAME_OFFSET, 8
.global irq_return
.type irq_return, %function

irq_return:
   ldr r0, =current              @ carrega o endereço da variável global estática current
   ldr r0, [r0]                  @ carrega o valor contido em current (endereço do processo atual)

   ldr sp, [r0, #TRAPFRAME_OFFSET]  @ carrega o valor do campo tf do processo atual como nova pilha

   add r1, sp, #(17 * 4)            @ Soma o tamanho exato do Trapframe (17 registradores)
   str r1, [r0, #TRAPFRAME_OFFSET]  @ Salva de volta no current->tf

   ldr r0, [sp, #(16 * 4)]          @ r0 = cpsr_usr (não usar lr aqui)
   msr spsr_cxsf, r0                @ spsr = cpsr_usr

   ldr lr, [sp, #(15 * 4)]          @ lr = pc_usr

   add r0, sp, #(13 * 4)            @ Usa r0 temporariamente como ponteiro
   ldmia r0, {sp, lr}^              @ restaura sp_usr e lr_usr usando r0
   nop                              @ Esvazia o pipeline
   
   ldmia sp, {r0-r12}               @ carrega os registradores r0-r12 finais

   movs pc, lr                      @ volta para pc_usr e copia SPSR = CPSR


.section .text
.equ TRAPFRAME_OFFSET, 8
.extern current
.global irq_return
.global restore_user_context
.global fork_return_asm

.type irq_return, %function
.type restore_user_context, %function
.type fork_return_asm, %function

@ Helper: Força a CPU a retornar para o contexto base do usuário
@ Útil para novos processos (criados por fork ou exec)
restore_user_context:
   ldr r0, =current
   ldr r0, [r0]
   ldr sp, [r0, #TRAPFRAME_OFFSET]
   b irq_return

@ Ponto de entrada totalmente em Assembly para clones
fork_return_asm:
   bl irq_end_current
   bl enable_cpu_interrupts
   b restore_user_context

irq_return:
   @ Presume-se estar no modo SVC e que o SP aponta para o Trapframe a ser ejetado
   ldr r0, [sp, #(16 * 4)]          @ r0 = SPSR salvo
   msr spsr_cxsf, r0                @ joga para SPSR_svc

   ldr lr, [sp, #(15 * 4)]          @ carrega o PC salvo para LR_svc

   add r0, sp, #(13 * 4)            
   ldmia r0, {sp, lr}^              @ restaura SP_usr e LR_usr
   nop                              
   
   ldmia sp, {r0-r12}               @ restaura R0-R12

   add sp, sp, #(17 * 4)            @ Ejetamos o trapframe da pilha!

   movs pc, lr                      @ Pula para o PC e restaura o CPSR nativamente

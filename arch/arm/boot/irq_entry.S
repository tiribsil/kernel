.section .text
.equ TRAPFRAME_OFFSET, 8
.extern current
.global irq_entry 
.type irq_entry, %function

irq_entry:
   @ Ajusta o LR para apontar para a instrução interrompida
   sub lr, lr, #4

   @ Salva R0-R3 e LR temporariamente na pilha do modo IRQ (SP_irq)
   stmfd sp!, {r0-r3, lr}

   @ Troca para o modo SVC para utilizarmos a pilha do kernel (SP_svc)
   mrs r0, cpsr
   bic r0, r0, #0x1F
   orr r0, r0, #0x13
   msr cpsr_c, r0

   @ Agora estamos em SVC. 
   @ Aloca o trapframe (17 * 4 = 68 bytes) empurrando a pilha
   sub sp, sp, #(17 * 4)

   @ Salva R4-R12 com segurança no Trapframe
   add r1, sp, #(4 * 4)
   stmia r1, {r4-r12}

   @ Volta para o modo IRQ para resgatar R0-R3 e o LR_irq original
   mrs r0, cpsr
   bic r0, r0, #0x1F
   orr r0, r0, #0x12
   msr cpsr_c, r0

   @ O SEGREDO: Puxamos o valor de LR_irq para dentro de R4!
   @ (R4 pode ser sobrescrito pois o original já está salvo no Trapframe)
   ldmfd sp!, {r0-r3, r4}
   mrs r1, spsr

   @ Retorna de vez ao modo SVC
   mrs r2, cpsr
   bic r2, r2, #0x1F
   orr r2, r2, #0x13
   msr cpsr_c, r2

   @ Salva R0-R3, PC (que agora está salvo em R4) e CPSR (em R1)
   stmia sp, {r0-r3}
   str r4, [sp, #(15 * 4)]
   str r1, [sp, #(16 * 4)]

   @ Salva SP_usr e LR_usr
   add r2, sp, #(13 * 4)
   stmia r2, {sp, lr}^
   nop

   @ ATENÇÃO: Só atualizamos o ponteiro current->tf se interrompemos
   @ o Modo Usuário (0x10). 
   and r2, r1, #0x1F
   cmp r2, #0x10
   bne 1f
   ldr r0, =current
   ldr r0, [r0]
   str sp, [r0, #TRAPFRAME_OFFSET]
1:

   @ PREVENÇÃO: Garante alinhamento de 8 bytes na pilha para o C (Norma AAPCS)
   mov r4, sp
   bic sp, sp, #7
   push {r4, lr}

   @ Chama o despachante em C
   bl irq_dispatcher_c

   @ Restaura a pilha e o Link Register do SVC
   pop {r4, lr}
   mov sp, r4

   @ Retorna ao processo/kernel interrompido
   b irq_return

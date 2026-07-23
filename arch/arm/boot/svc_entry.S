.section .text
.equ TRAPFRAME_OFFSET, 8
.extern current
.global svc_entry 
.type svc_entry, %function

svc_entry:
   @ Aloca o trapframe diretamente no SP_svc ativo
   sub sp, sp, #(17 * 4)
   stmia sp, {r0-r12}

   str lr, [sp, #(15 * 4)]       @ Salva o PC (LR_svc)
   
   mrs r12, spsr                 
   str r12, [sp, #(16 * 4)]      @ Salva o CPSR (SPSR_svc)
   
   add r12, sp, #(13*4)     
   stmia r12, {sp, lr}^          @ Salva SP_usr e LR_usr
   nop                            

   @ Atualiza o current->tf
   ldr r12, =current              
   ldr r12, [r12]                 
   str sp, [r12, #TRAPFRAME_OFFSET] 

   @ PREVENÇÃO: Garante alinhamento de 8 bytes na pilha para o C (Norma AAPCS)
   mov r4, sp
   bic sp, sp, #7
   push {r4, lr}

   @ Chama o tratador de Syscalls 
   bl syscall_Handler
   
   @ Restaura a pilha
   pop {r4, lr}
   mov sp, r4

   @ Grava o valor de retorno (R0) de volta no trapframe
   str r0, [sp, #0]
   
   @ Retorna restaurando o trapframe
   b irq_return

// adicionar aqui codigo da chamada de sistema, 
//a ordem deve ser igual a da tabela
.equ ABORT_CALL, 0
.equ SYS_FORK, 1
.equ SYS_EXEC, 2
.equ SYS_EXIT, 3
.equ SYS_READ, 4
.equ SYS_WRITE, 5
.equ SYS_WAITPID, 6
.text


.global abort
.type abort, %function
abort:
    push {lr}
    mov r7, #ABORT_CALL
    SVC #0
    pop {pc} 


.global fork
.type fork, %function
fork:
    push {lr}
    mov r7, #SYS_FORK
    SVC #0
    pop {pc} 


.global exec
.type exec, %function
exec:
    push {lr}
    mov r7, #SYS_EXEC
    SVC #0
    pop {pc} 


.global exit
.type exit, %function
exit:
    push {lr}
    mov r7, #SYS_EXIT
    SVC #0
    pop {pc}


.global read
.type read, %function
read:
    push {lr}
    mov r7, #SYS_READ
    SVC #0
    pop {pc}


.global write
.type write, %function
write:
    push {lr}
    mov r7, #SYS_WRITE
    SVC #0
    pop {pc}


.global waitpid
.type waitpid, %function
waitpid:
    push {lr}
    mov r7, #SYS_WAITPID
    SVC #0
    pop {pc}

/*

exemplo:
    push {lr} guardar o endereço da chamada

    swap de registradores para organizar parametros

    mov r0, #CHAMADA //r0 é definido como registrador com o codigo da chamada,
                     //e o resto do sistema so funciona seguindo esse padrão

    SVC #0 // É a instrução trap, 
           // necessária para ir ao modo supervisor e executar qualquer chamada

    pop {pc} retorna para quem chamou
    
 */



    


    

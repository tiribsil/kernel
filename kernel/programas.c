#include <programas.h>
#include <syscall.h>
#include <stringutils.h>

void programainicio(int argc, char** argv) {
    (void)argc; (void)argv;
    write("Processo rodando programa inicio\n", 50);

    if(!fork()) exec(programa1, 0, 0);
    if(!fork()) exec(programa2, 0, 0);
    exit();
}

void programa1(int argc, char** argv) {
    (void)argc; (void)argv;
    for(int i = 0; i < 10000; i++) {
        write("Processo rodando programa 1\n", 50);
    }
    //exit();
}

void programa2(int argc, char** argv) {
    (void)argc; (void)argv;
    for(int i = 0; i < 10; i++) {
        write("Processo rodando programa 2\n", 50);
    }
    exit();
}

void prog_shell(int argc, char** argv) {
    (void)argc; (void)argv;
    struct {
        const char *cmd_name;
        void (*func)(int, char**);
    } cmd_table[] = {
        {"programa1", programa1},
        {"programa2", programa2},
        //{"count", prog_count},
        //{"prime", prog_prime},
        //{"sysinfo", prog_sysinfo},
        //{"matrix", prog_matrix},
	//{"crash", prog_crash},
        {0, 0}
    };

    if(!fork()) while(1); // Idle process para a cpu ter algo pra fazer

    #define MAX_COMMAND 256
    #define MAX_WORD 64

    char command[MAX_COMMAND];
    char* child_argv[MAX_WORD];
    int pid, waitForChild = 1;

    while(1){
        write("> ", 3);
        read(command, MAX_COMMAND);

        int child_argc = 0;
	char* p = command;
	while(*p && *p != '\n'){
	    while(*p == ' ') p++;
            child_argv[child_argc] = p;
	    child_argc++;
	    while(*p && *p != ' ' && *p != '\n') p++;
	    if(*p == ' ') *p = 0;
	    p++;
	}
	child_argv[child_argc] = 0;
	if(!child_argc) continue;

        for(int i = 0; i < MAX_WORD; i++) 
            if(child_argv[child_argc - 1][i] == '\n') { child_argv[child_argc - 1][i] = 0; break; }

        if(!strcmp(child_argv[child_argc - 1], "&")){
            waitForChild = 0;
            child_argc--;
            child_argv[child_argc] = 0;
        }

        if(!strcmp(child_argv[0], "exit")) abort();

        pid = fork();
        if(pid){
            if(waitForChild) waitpid(pid); 
            else waitForChild = 1;
        } else {
	    for(int i = 0; cmd_table[i].cmd_name; i++){
                if(strcmp(*child_argv, cmd_table[i].cmd_name)) continue;
		exec(cmd_table[i].func, child_argc, child_argv);
	    }
            write("Erro ao executar comando!\n", 50);
            exit();
        }
    }
}

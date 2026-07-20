#include "shell.h"
#include "kstdio.h"

// Implementacao basica do strcomp
int strcmp(const char *s1, const char *s2) {
  while (*s1 && (*s1 == *s2)) {
    s1++;
    s2++;
  }
  return *(const unsigned char *)s1 - *(const unsigned char *)s2;
}

#define CMD_BUFFER_SIZE 128
#define MAX_ARGS 10

void shell_run(void) {
  char cmd_buffer[CMD_BUFFER_SIZE];

  kprintf("\n=== UFSKernel Shell ===\n");
  kprintf("Digite 'help'\n");

  while (1) {
    kprintf("$ ");

    kgets(cmd_buffer, CMD_BUFFER_SIZE);

    // Ignora comando vazio
    if (cmd_buffer[0] == '\0') {
      continue;
    }

    // === Tokenizador e Parser (continua o mesmo) ===
    char *argv[MAX_ARGS];
    int argc = 0;
    char *ptr = cmd_buffer;
    // Tokenizador
    while (*ptr != '\0') {
      // Separa por espacos
      while (*ptr == ' ') {
        *ptr = '\0';
        ptr++;
      }
      if (*ptr == '\0') {
        break;
      }

      // Achou inicio de uma palavra
      if (argc < MAX_ARGS) {
        argv[argc] = ptr;
        argc++;
      }

      while (*ptr != ' ' && *ptr != '\0') {
        ptr++;
      }
    }

    // Se o contador tiver vazio ignora
    if (argc == 0) {
      continue;
    }

    // Parser basico
    if (strcmp(argv[0], "help") == 0) {
      kprintf("\nComandos disponiveis:\n");
      kprintf("  help          - Mostra essa mensagem\n");
      kprintf("  clear         - Limpa a tela\n");
      kprintf("  kprintf       - Testa o kprintf\n");
      kprintf("  echo [texto]  - Repete caracteres digitados\n");
      kprintf("  quit          - Fecha o shell\n");
    } else if (strcmp(argv[0], "clear") == 0) {
      // Codigo ANSI pra limpar a tela
      kputs("\033[2J\033[H");
    } else if (strcmp(argv[0], "echo") == 0) {
      for (int i = 1; i < argc; i++) {
        kprintf("%s ", argv[i]);
      }
      kprintf("\n");
    } else if (strcmp(argv[0], "kprintf") == 0) {
      kprintf("+========PRINTF========+\n");
      kprintf("Teste printf sem parametros\n");
      kprintf("Teste String: %s\n", "Hello World");
      kprintf("Teste Char: %c\n", 'A');
      kprintf("Teste Decimal(int): %d, %d\n", 67, -451);
      kprintf("Teste float %.2f\n", 3.14f);
      kprintf("Teste Hex: %x\n", 0x09000000);
      kprintf("+======================+\n");
    } else if(strcmp(argv[0], "quit") == 0){
        return;
    } else {
        kprintf("Comando nao encontrado: %s\n", argv[0]);
    }
  }
}

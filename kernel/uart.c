#include "uart.h"

#include "interrupts_handler.h"

#define UART0_BASE 0x09000000U

// UART0_CR: Control Register (liga/desliga TX/RX e o dispositivo)
#define UART0_CR ((volatile uint32_t *)(UART0_BASE + 0x030))

// UART_DR: Data Register (leitura/escrita de bytes)
// UART_FR: Flag Register (estado da UART)
#define UART_DR ((volatile uint32_t *)(UART0_BASE + 0x000))
#define UART_FR ((volatile uint32_t *)(UART0_BASE + 0x018))

// Bits do Flag Register
#define UART_FR_BUSY (1 << 3) // UART ocupada transmitindo
#define UART_FR_RXFE (1 << 4) // RX FIFO vazio
#define UART_FR_TXFF (1 << 5) // TX FIFO cheio

// UART0_LCR_H: Line Control Register (configura formato e FIFO)
#define UART0_LCR_H ((volatile uint32_t *)(UART0_BASE + 0x02C))
#define UART_LCR_FEN (1 << 4) // Habilita FIFO

// UART0_IMSC: Interrupt Mask Set/Clear (liga interrupcoes da UART)
#define UART0_IMSC ((volatile uint32_t *)(UART0_BASE + 0x038))
#define UART_IMSC_RXIM (1 << 4) // Interrupcao de RX
#define UART_IMSC_TXIM (1 << 5) // Interrupcao de TX (nao usada)

// UART0_ICR: Interrupt Clear Register (limpa interrupcoes pendentes)
// UART0_MIS: Masked Interrupt Status (status das interrupcoes habilitadas)
#define UART0_ICR ((volatile uint32_t *)(UART0_BASE + 0x044))
#define UART0_MIS ((volatile uint32_t *)(UART0_BASE + 0x040))

// Baud rate (divisores inteiro e fracionario)
#define UART0_IBRD ((volatile uint32_t *)(UART0_BASE + 0x024))
#define UART0_FBRD ((volatile uint32_t *)(UART0_BASE + 0x028))

#define UART_BUFFER_SIZE 256

// Buffer circular simples para guardar bytes recebidos pela interrupcao
typedef struct {
  char buffer[UART_BUFFER_SIZE];
  volatile uint32_t head; // proxima posicao de escrita
  volatile uint32_t tail; // proxima posicao de leitura
} ring_buffer_t;

// Buffer de RX (recepcao)
static ring_buffer_t rx_buffer = {.head = 0, .tail = 0};

// Envia um caractere pela UART (modo polling)
void uart_putc(char c) {
  if (c == '\n') {
    uart_putc('\r');
  }

  // Espera o FIFO de TX ter espaco
  while ((*UART_FR) & UART_FR_TXFF) {
  }

  *UART_DR = c;
}

// Envia uma string inteira (ate '\0'.
void uart_puts(const char *str) {
  while (*str != '\0') {
    uart_putc(*str);
    str++;
  }
}

// Aguarda o fim da transmissao (TX vazio e UART parado)
void uart_flush(void) {
  while ((*UART_FR) & UART_FR_BUSY) {
  }
}

// Insere um byte no buffer circular de RX
// Retorna 1 se conseguiu ou 0 se o buffer ta cheio
static int uart_buffer_push(char c) {
  uint32_t next_head = (rx_buffer.head + 1) % UART_BUFFER_SIZE;

  if (next_head != rx_buffer.tail) {
    rx_buffer.buffer[rx_buffer.head] = c;
    rx_buffer.head = next_head;
    return 1;
  }

  return 0; // Overflow do buffer
}

// Remove um byte do buffer
// Retorna 1 se conseguiu ou 0 se nao havia dados
static int uart_buffer_pop(char *ptr) {
  if (rx_buffer.head == rx_buffer.tail) {
    return 0;
  }

  *ptr = rx_buffer.buffer[rx_buffer.tail];
  rx_buffer.tail = (rx_buffer.tail + 1) % UART_BUFFER_SIZE;

  return 1;
}

// Indica se tem pelo menos um byte no buffer
int uart_has_data(void) { return rx_buffer.head != rx_buffer.tail; }

// Configura o baud rate
static void uart_set_baud_rate(void) {
  *UART0_IBRD = 13;
  *UART0_FBRD = 1;
}

// Habilita FIFO e define 8N1
static void uart_enable_fifo(void) {

  *UART0_LCR_H = (0b11 << 5) | UART_LCR_FEN;
}

// Habilita interrupcao de RX na UART
static void uart_enable_interrupts(void) { *UART0_IMSC = UART_IMSC_RXIM; }

// API do UART: liga/desliga interrupcao de RX
void uart_enable_rx_irq(void) { uart_enable_interrupts(); }

void uart_disable_rx_irq(void) { *UART0_IMSC = 0; }

// Inicializa a UART:
// 1) desliga, 2) configura baud/FIFO, 3) habilita RX IRQ, 4) liga TX/RX
void uart_init(void) {
  uart_flush();
  *UART0_CR = 0; // desliga para configurar

  uart_set_baud_rate();
  uart_enable_fifo();

  uart_enable_interrupts();

  // INFO: NAO IRA FUNCIONAR SEM O INTERRUPTS_HANDLER

  // Registra o handler no array do GIC
  register_interrupt_handler(UART_IRQ, uart_irq_handler);

  // Configura a interrupcao como level-sensitive
  gic_config_interrupt(UART_IRQ, 0);

  // Habilita a IRQ 33 no distribuidor do GIC
  gic_enable_interrupt(UART_IRQ);

  // Liga UART
  *UART0_CR = (1 << 0) | (1 << 8) | (1 << 9);
}

// Handler de interrupcao da UART
// Deve ser chamado pelo gic quando irq_id == UART_IRQ
void uart_irq_handler(void) {
  uint32_t status = *UART0_MIS;

  if (status & UART_IMSC_RXIM) {
    // Limpa a interrupcao
    *UART0_ICR = UART_IMSC_RXIM;

    // Esvazia o FIFO de RX e guarda no buffer
    while (!(*UART_FR & UART_FR_RXFE)) {
      char c = (char)(*UART_DR);
      uart_buffer_push(c);
    }
  }
}

// Le um byte da UART
// Primeiro tenta o buffer
// Se o buffer estiver vazio faz polling direto no hardware
char uart_getc(void) {
  char c;

  while (!uart_buffer_pop(&c)) {
    // Fallback polling direto do hardware se nao tiver IRQs
    if (!(*UART_FR & UART_FR_RXFE)) {
      c = (char)(*UART_DR);
      return c;
    }
  }

  return c;
}

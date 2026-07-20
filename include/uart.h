#ifndef UART_H
#define UART_H

//typedef unsigned int uint32_t;

// IRQ da UART no QEMU (GIC ID 33)
#define UART_IRQ 33u

void uart_putc(char c);
void uart_puts(const char *str);
void uart_flush(void);

void uart_init(void);

// Controle local de interrupcoes da UART (RX)
void uart_enable_rx_irq(void);
void uart_disable_rx_irq(void);

// Handler chamado pelo gic
// Exemplo de uso:
// if (irq_id == UART_IRQ) uart_irq_handler();
void uart_irq_handler(void);

char uart_getc(void);
int uart_has_data(void);

#endif // UART_H

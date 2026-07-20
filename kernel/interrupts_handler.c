#include <interrupts_handler.h>
#include <vboard.h>
#include <bitutils.h>

static isr_t interrupt_handlers[MAX_INTERRUPTS] = {0};

uint32_t current_irq;

int register_interrupt_handler(uint32_t irq, isr_t handler) {
  if (irq >= MAX_INTERRUPTS) return -1; // ID inválido
  if (interrupt_handlers[irq]) return -2; // Já existe
  
  interrupt_handlers[irq] = handler; // Registra tratador
  return 0;
}

void gic_enable_interrupt(uint32_t irq) {
  if (irq >= MAX_INTERRUPTS) return; // ID inválido

  // Os registradores são vetores de números de 32 bits
  uint32_t reg_index = irq >> 5; // Divide por 32 -> número do vetor onde está o bit
  uint32_t bit_offset = irq & BIT_MASK(5); // Primeiros 5 bits -> bit certo dentro do número

  GICD_ISENABLER[reg_index] |= (1 << bit_offset); // Ativa o bit, liga a interrupção
}

void gic_disable_interrupt(uint32_t irq) {
  if (irq >= MAX_INTERRUPTS) return;

  uint32_t reg_index = irq >> 5;
  uint32_t bit_offset = irq & BIT_MASK(5);

  GICD_ICENABLER[reg_index] = (1 << bit_offset); // Ativa o bit, desliga a interrupção
}

void init_gic(void) {
  GICD_CTLR = 1; // Ativa distribuidor global de interrupções
  GICC_CTLR = 1; // Ativa interface de interrupções da CPU
  GICC_PMR = BIT_MASK(8); // Tratar interrupções de qualquer prioridade
}

void gic_config_interrupt(uint32_t irq, int edge_triggered) {
  // Cada IRQ ocupa 2 bits, acha o indice dividindo por 16
  uint32_t reg_index = irq >> 4;
  // Primeiros 4 bits são quantos pares tem que pular
  uint32_t shift = (irq & BIT_MASK(4)) << 1;

  // Primeiro, limpamos o par de bits da IRQ
  GICD_ICFGR[reg_index] &= ~(0b11 << shift);

  // Se for sensível à borda, aplicamos o valor 10
  if (edge_triggered) GICD_ICFGR[reg_index] |= (0b10 << shift);
}

void enable_cpu_interrupts(void) { __asm__ volatile("cpsie i"); }

void irq_dispatcher_c(void) {
  uint32_t irq_id = GICC_IAR & BIT_MASK(10); // Pega ID da interrupção do registrador
  current_irq = irq_id;

  if (irq_id < MAX_INTERRUPTS && interrupt_handlers[irq_id])
    interrupt_handlers[irq_id](); // Trata, se possível

  GICC_EOIR = irq_id; // Avisa que foi tratada
}

void irq_end_current(void){
    GICC_EOIR = current_irq; // Avisa que ultima interrupção foi tratada
}

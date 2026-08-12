#ifndef TYPES_H
#define TYPES_H

/* ========================================================================== */
/*   Tipos Inteiros Padrão (Faltantes nos seus erros)                         */
/* ========================================================================== */

// Tipos de 8 bits
typedef unsigned char          uint8_t;
typedef signed char            int8_t;

// Tipos de 16 bits
typedef unsigned short         uint16_t;
typedef signed short           int16_t;

// Tipos de 32 bits (Necessários para uint32_t)
typedef unsigned int           uint32_t;
typedef signed int             int32_t;

// Tipos de 64 bits (Úteis para suporte a arquivos grandes ou data/hora)
typedef unsigned long long     uint64_t;
typedef signed long long       int64_t;


/* ========================================================================== */
/*   Tipos de Ponteiros e Tamanhos (Necessários para uintptr_t)               */
/* ========================================================================== */

// Inteiro do tamanho de um ponteiro (Crucial para VMM e PMM em 32 bits)
typedef uint32_t               uintptr_t;
typedef int32_t                intptr_t;

// Tipos para tamanhos de estruturas e offsets
typedef uint32_t               size_t;
typedef int32_t                ssize_t;


/* ========================================================================== */
/*   Tipos Extras Muito Úteis para Desenvolvimento de OS                    */
/* ========================================================================== */

// Tipo Booleano nativo
typedef enum {
    false = 0,
    true = 1
} bool;

// Tipo nulo padrão
#ifndef NULL
#define NULL ((void*)0)
#endif

// Tipos específicos para gerenciamento de memória (Abstração de código)
typedef uintptr_t              vaddr_t;   // Endereço Virtual
typedef uintptr_t              paddr_t;   // Endereço Físico

#endif /* TYPES_H */


#ifndef BITUTILS_H
#define BITUTILS_H

#define BIT_MASK(n) ((1U << (n)) - 1)

static inline void enable_bit(volatile void* array, unsigned bit_index) {
    volatile unsigned char* ptr = (volatile unsigned char*)array;
    ptr[bit_index >> 3] |= (1 << (bit_index & 7));
}

static inline void disable_bit(volatile void* array, unsigned bit_index) {
    volatile unsigned char* ptr = (volatile unsigned char*)array;
    ptr[bit_index >> 3] &= ~(1 << (bit_index & 7));
}

#endif

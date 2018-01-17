#ifndef _SHIFT_H
#define _SHIFT_H
#include <stdbool.h>

void shift_init();

void shift_bit(bool bit);
void shift_byte(uint8_t byte);
void shift_word(uint16_t word);

void shift_clock();
void shift_latch();

void shift_set_addr(uint16_t addr);

#endif

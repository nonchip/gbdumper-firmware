#ifndef _PLAIN_RAM_H
#define _PLAIN_RAM_H

#include <stdint.h>
#include <stdbool.h>
#include "emulator.h"

void plain_ram_init(char* loadfile,bool randomize);

uint8_t plain_ram_read(uint16_t addr);
void plain_ram_write(uint16_t addr,uint8_t value);

#endif

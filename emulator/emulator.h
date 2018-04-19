#ifndef _EMULATOR_H
#define _EMULATOR_H
#include <stdint.h>
#include <stdbool.h>

// accessors
int emulator_init(int argc,char* argv[]);
void set_port_cmd(uint8_t v);
void set_port_data(uint8_t v);
uint8_t get_port_data();
void shift_set_addr(uint16_t a);

// there exists a private struct emulator_data_t that's used by those, but it's not exposed here on purpose.

// chipset implementations:

uint8_t (*emulator_read)(uint16_t);
void (*emulator_write)(uint16_t,uint8_t);

void plain_ram_init(char* loadfile,bool randomize);


#endif

#ifndef EMULATOR_H
#define EMULATOR_H
#include <stdint.h>

int emulator_init(int argc,char* argv[]);
void emulator_think();
void set_port_cmd(uint8_t v);
void set_port_data(uint8_t v);
uint8_t get_port_data();
void shift_set_addr(uint16_t a);

struct emulator_data_t{
  uint8_t rom[0xffff];
  uint8_t prevcmd;
  uint8_t cmd;
  uint8_t data;
  uint16_t addr;
};

struct emulator_data_t* emulator_data;

#endif

#include <stdlib.h>
#include "emulator.h"
#include "emupins.h"
#define _BV(bit) \
  (1 << (bit)) 


int emulator_init(int argc,char* argv[]){
  emulator_data = malloc(sizeof(struct emulator_data_t));
}

void set_port_cmd(uint8_t v){
  emulator_data->prevcmd=emulator_data->cmd;
  emulator_data->cmd=v;
  emulator_think();
}

void set_port_data(uint8_t v){
  emulator_data->data=v;
  emulator_think();
}

uint8_t get_port_data(){
  emulator_think();
  return emulator_data->data;
}

void shift_set_addr(uint16_t a){
  emulator_data->addr=a;
  emulator_think();
}

void emulator_think(){
  if( ( emulator_data->prevcmd & _BV(CMD_CLK) ) == ( emulator_data->cmd & _BV(CMD_CLK) ) )
    return;
}

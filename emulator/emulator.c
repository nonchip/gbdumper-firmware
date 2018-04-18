#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include "emulator.h"
#include "emupins.h"
#define _BV(bit) \
  (1 << (bit)) 

// private function
void emulator_think();

// private struct
struct emulator_data_t{
  uint8_t cmd;         // bus state
  uint8_t data;        // bus state
  uint16_t addr;       // bus state
  // those aren't accessible outside the emulator:
  uint8_t prevcmd;     // previous bus state (to detect edges)
  //uint8_t latchdata;   // internal state
  uint16_t latchaddr;  // internal state
  uint8_t ram[0xffff]; // internal state
};

struct emulator_data_t* emulator_data;

int emulator_init(int argc,char* argv[]){
  emulator_data = malloc(sizeof(struct emulator_data_t));
  char* loadfile=NULL;
  opterr = 0;
  char c;
  while((c = getopt (argc, argv, "rl:")) != -1)
    switch(c)
      {
      case 'r': // initialize everything with random values
        srand(time(NULL));
        for (int i = 0; i < sizeof(struct emulator_data_t); ++i)
          ((uint8_t*)emulator_data)[i]=(uint8_t)rand();
        break;
      case 'l': // load ram from file
        loadfile = optarg;
        break;
      case '?':
        if(optopt == 'l')
          fprintf(stderr, "Option -%c requires an argument.\n", optopt);
        else if(isprint (optopt))
          fprintf(stderr, "Unknown option `-%c'.\n", optopt);
        else
          fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);
        abort();
      default:
        abort();
      }
  if(loadfile){
    FILE* fp = fopen(loadfile,"r");
    if(!fp){
      fprintf(stderr, "Error opening '%s': %s\n",loadfile,strerror(errno));
      abort();
    }
    size_t r=fread(emulator_data->ram, 1, 0xffff, fp);
    fclose(fp);
    fprintf(stderr,"Read %#06x bytes from '%s' to ram.\n",r,loadfile);
  }
  fprintf(stderr,"emulator_init done.\n");
}

void set_port_cmd(uint8_t v){
  fprintf(stderr,"cmd  < %#04x.\n",v);
  emulator_data->prevcmd=emulator_data->cmd;
  emulator_data->cmd=v;
  emulator_think();
}

void set_port_data(uint8_t v){
  fprintf(stderr,"data < %#04x.\n",v);
  emulator_data->data=v;
  emulator_think();
}

uint8_t get_port_data(){
  emulator_think();
  uint8_t v = emulator_data->data;
  fprintf(stderr,"data > %#04x.\n",v);
  return v;
}

void shift_set_addr(uint16_t a){
  fprintf(stderr,"addr < %#06x.\n",a);
  emulator_data->addr=a;
  emulator_think();
}

bool edge_rising(uint8_t pin){
  return ( emulator_data->prevcmd & _BV(pin) ) !=0  // was high
      && ( emulator_data->cmd     & _BV(pin) ) ==0; // is low
}

bool edge_falling(uint8_t pin){
  return ( emulator_data->prevcmd & _BV(pin) ) ==0  // was low
      && ( emulator_data->cmd     & _BV(pin) ) ==0; // is high
}

bool low(uint8_t pin){
  return (emulator_data->cmd & _BV(pin)) == 0;
}

bool high(uint8_t pin){
  return !low(pin);
}

void emulator_think(){
  if( emulator_data->prevcmd == emulator_data->cmd ) // no cmd changes
    return;
  if( ( emulator_data->cmd & _BV(CMD__CS) ) != 0 ) // ~CS high, bus disabled
    return;

  //latch addr on \CLK
  if(edge_falling(CMD_CLK))
    emulator_data->latchaddr=emulator_data->addr;

  //write continuously while !WR low and !RD high:
  if(low(CMD__WR) && high(CMD__RD))
    emulator_data->ram[emulator_data->latchaddr] = emulator_data->data;

  //read continuously while !WR high and !RD low:
  if(high(CMD__WR) && low(CMD__RD))
    emulator_data->data = emulator_data->ram[emulator_data->latchaddr];

  //die if !WR and !RD low:
  if(low(CMD__RD) && edge_falling(CMD__WR) && !edge_falling(CMD__RD)){
    fprintf(stderr, "MAGIC SMOKE: ~WR falling while ~RD low.\n");
    abort();
  }
  if(low(CMD__WR) && edge_falling(CMD__RD) && !edge_falling(CMD__WR)){
    fprintf(stderr, "MAGIC SMOKE: ~RD falling while ~WR low.\n");
    abort();
  }
  if(low(CMD__WR) && low(CMD__RD)){
    fprintf(stderr, "MAGIC SMOKE: ~WR and ~RD low at the same time.\n");
    abort();
  }

}

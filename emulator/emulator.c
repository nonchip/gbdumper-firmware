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
};

struct emulator_data_t* emulator_data;

int emulator_init(int argc,char* argv[]){
  emulator_data = malloc(sizeof(struct emulator_data_t));
  char* loadfile=NULL;
  opterr = 0;
  char c;
  char* chipset="plain_ram";
  bool randomize=false;
  while((c = getopt (argc, argv, "rl:c:")) != -1)
    switch(c)
      {
      case 'r': // initialize everything with random values
        srand(time(NULL));
        randomize=true;
        for (int i = 0; i < sizeof(struct emulator_data_t); ++i)
          ((uint8_t*)emulator_data)[i]=(uint8_t)rand();
        break;
      case 'l': // load from file
        loadfile = optarg;
        break;
      case 'c':
        chipset = optarg;
        break;
      case '?':
        if(optopt == 'l' || optopt == 'c')
          fprintf(stderr, "Option -%c requires an argument.\n", optopt);
        else if(isprint (optopt))
          fprintf(stderr, "Unknown option `-%c'.\n", optopt);
        else
          fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);
        fflush(stderr);
        abort();
      default:
        abort();
      }
  
  if(!strcmp(chipset,"plain_ram"))
    plain_ram_init(loadfile,randomize);

  emulator_data->latchaddr=emulator_data->addr;
  emulator_data->prevcmd=emulator_data->cmd;
  fprintf(stderr,"emulator_init done.\n");
}

void set_port_cmd(uint8_t v){
  char* s=malloc(255);
  if(0==(v & _BV(CMD__WR)))
    strcat(s,"w");
  if(0==(v & _BV(CMD__RD)))
    strcat(s,"r");
  if(0==(v & _BV(CMD__CS)))
    strcat(s,"c");
  if(0==(v & _BV(CMD__RST)))
    strcat(s,"R");
  if(0!=(v & _BV(CMD_CLK)))
    strcat(s,"C");
  fprintf(stderr,"cmd  < %#04x     %s\n",v,s);
  free(s);
  emulator_data->prevcmd=emulator_data->cmd;
  emulator_data->cmd=v;
  emulator_think();
}

void set_port_data(uint8_t v){
  fprintf(stderr,"data < %#04x\n",v);
  emulator_data->data=v;
  emulator_think();
}

uint8_t get_port_data(){
  emulator_think();
  uint8_t v = emulator_data->data;
  fprintf(stderr,"data > %#04x\n",v);
  return v;
}

void shift_set_addr(uint16_t a){
  fprintf(stderr,"addr < %#06x\n",a);
  emulator_data->addr=a;
  emulator_think();
}

bool edge_falling(uint8_t pin){
  return ( emulator_data->prevcmd & _BV(pin) ) !=0  // was high
      && ( emulator_data->cmd     & _BV(pin) ) ==0; // is low
}

bool edge_rising(uint8_t pin){
  return ( emulator_data->prevcmd & _BV(pin) ) ==0  // was low
      && ( emulator_data->cmd     & _BV(pin) ) !=0; // is high
}

bool low(uint8_t pin){
  return (emulator_data->cmd & _BV(pin)) == 0;
}

bool high(uint8_t pin){
  return !low(pin);
}

void emulator_think(){
  //if( emulator_data->prevcmd == emulator_data->cmd ) // no cmd changes
  //  return;

  //clear data bus on /CS
  if(edge_rising(CMD__CS))
    emulator_data->data = 0;

  //latch addr on \CS
  if(edge_falling(CMD__CS))
    emulator_data->latchaddr=emulator_data->addr;

  if(high(CMD__CS)) // ~CS high, bus disabled
    return;

  //fprintf(stderr, "addr: %#06x .. %#06x\n",emulator_data->latchaddr,emulator_data->addr);

  //write continuously while !WR low and !RD high:
  if(low(CMD__WR) && high(CMD__RD))
    (*emulator_write)(emulator_data->latchaddr,emulator_data->data);

  //read on \CLK while !WR high and !RD low:
  if(high(CMD__WR) && low(CMD__RD) && edge_falling(CMD_CLK))
    emulator_data->data = (*emulator_read)(emulator_data->latchaddr);

  //die if !WR and !RD low:
  if(low(CMD__RD) && edge_falling(CMD__WR) && !edge_falling(CMD__RD)){
    fprintf(stderr, "MAGIC SMOKE: ~WR falling while ~RD low.\n");
    fflush(stderr);
    abort();
  }
  if(low(CMD__WR) && edge_falling(CMD__RD) && !edge_falling(CMD__WR)){
    fprintf(stderr, "MAGIC SMOKE: ~RD falling while ~WR low.\n");
    fflush(stderr);
    abort();
  }
  if(low(CMD__WR) && low(CMD__RD)){
    fprintf(stderr, "MAGIC SMOKE: ~WR and ~RD low at the same time.\n");
    fflush(stderr);
    abort();
  }

  //die if !CS or CLK is rising while !WR low
  if(low(CMD__WR) && (edge_rising(CMD__CS) || edge_rising(CMD_CLK))){
    fprintf(stderr, "MAGIC SMOKE: addr changed while ~WR low.\n");
    fflush(stderr);
    abort();
  }

  //die if addr is changed while !WR low
  if((emulator_data->addr != emulator_data->latchaddr) && low(CMD__WR) ){
    fprintf(stderr, "MAGIC SMOKE: addr changed while ~WR low.\n");
    fflush(stderr);
    abort();
  }
  
  //die if addr is changed while !CS low
  if((emulator_data->addr != emulator_data->latchaddr) && low(CMD__CS) ){
    fprintf(stderr, "MAGIC SMOKE: addr changed while ~CS low.\n");
    fflush(stderr);
    abort();
  }

}

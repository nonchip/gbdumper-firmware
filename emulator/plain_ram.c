#include "plain_ram.h"
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <string.h>

uint8_t plain_ram[0x10000];

void plain_ram_init(char* loadfile, bool randomize){
  if(randomize)
    for (int i = 0; i < 0x10000; ++i)
      plain_ram[i]=rand();
  if(loadfile){
    FILE* fp = fopen(loadfile,"r");
    if(!fp){
      fprintf(stderr, "Error opening '%s': %s\n",loadfile,strerror(errno));
      fflush(stderr);
      abort();
    }
    size_t r=fread(plain_ram, 1, 0xffff, fp);
    fclose(fp);
    fprintf(stderr,"Read %#06x bytes from '%s' to ram.\n",r,loadfile);
  }
  emulator_read=&plain_ram_read;
  emulator_write=&plain_ram_write;
}

uint8_t plain_ram_read(uint16_t addr){
  return plain_ram[addr];
}
void plain_ram_write(uint16_t addr,uint8_t value){
  plain_ram[addr]=value;
}
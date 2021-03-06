#ifdef EMULATOR
  #define _BV(bit) \
    (1 << (bit)) 
  #include "emulator/emulator.h"
  #include "emulator/emupins.h"
  #include <stdbool.h>
#else
  #include <avr/io.h>
  #include <util/delay.h>
  #include <avr/sleep.h>
  #include <avr/interrupt.h>
  #include "pins.h"
  #include "uart.h"
  #include "shift.h"
  #define set_port_cmd(v) PORT_CMD = v
  #define set_port_data(v) PORT_DATA = v
#endif
#include <stdlib.h>
#include <stdio.h>

#ifdef EMULATOR
int main (int argc,char* argv[]) {
  emulator_init(argc,argv);
#else
int main (void) {
  uart_init(true); // setup uart and bind to stdio
  shift_init();
  DDR_CMD |= _BV(CMD__RD) | _BV(CMD__CS) | _BV(CMD__WR) | _BV(CMD__RST) | _BV(CMD_CLK);
#endif
  set_port_cmd(_BV(CMD__RD) | _BV(CMD__CS) | _BV(CMD__WR) | _BV(CMD__RST));
  char cmd;
  while(true){
    cmd=getchar();
    if (cmd=='\r' || cmd=='\n' || cmd==EOF || cmd==0)
      continue;
    //fprintf(stderr,"[%c %#04x]",cmd,cmd);
    if (cmd!='R' && getchar()!=' ')
    {
      printf("ERR\r\n");
      continue;
    }
    switch(cmd){
      case 'C': // command pins
        /* dummy statement for label */;
        int v=_BV(CMD__RD) | _BV(CMD__CS) | _BV(CMD__WR) | _BV(CMD__RST);
        bool reading=true;
        char c;
        while(reading){
          c=getchar();
          //fprintf(stderr,"(%c %c %#04x)",cmd,c,c);
          switch(c){
            case 'w': // ~WE
              v &= ~ _BV(CMD__WR);
              break;
            case 'r': // ~RE
              v &= ~ _BV(CMD__RD);
              break;
            case 'c': // ~CS
              v &= ~ _BV(CMD__CS);
              break;
            case 'R': // ~RST
              v &= ~ _BV(CMD__RST);
              break;
            case 'C': // CLK
              v |= _BV(CMD_CLK);
              break;
            case '\r':
            case '\n':
            default:
              reading=false;
              break;
          }
        }
        set_port_cmd(v);
        break;
      case 'A': // set addr
        /* dummy statement for label */;
        uint16_t a;
        scanf("%x",&a);
        shift_set_addr(a);
        break;
      case 'R': // read byte
        /* dummy statement for label */;
        #ifndef EMULATOR
          DDR_DATA = 0;
        #endif
        uint8_t dr = PIN_DATA;
        printf("%02x\r\n",dr);
        break;
      case 'W': // write byte
        /* dummy statement for label */;
        #ifndef EMULATOR
          DDR_DATA = _BV(DATA_0) | _BV(DATA_1) | _BV(DATA_2) | _BV(DATA_3) | _BV(DATA_4) | _BV(DATA_5) | _BV(DATA_6) | _BV(DATA_7);
        #endif
        uint8_t dw;
        scanf("%hhx",&dw);
        set_port_data(dw);
        break;
      default:
        printf("ERR\r\n");
        break;
    }
  }
  return 0;
}

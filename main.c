#ifdef EMULATOR
  #include "emulator/emulator.h"
#else
  #include <avr/io.h>
  #include <util/delay.h>
  #include <avr/sleep.h>
  #include <avr/interrupt.h>
  #include "uart.h"
  #include "pins.h"
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
  PORT_CMD = _BV(CMD__RD) | _BV(CMD__CS) | _BV(CMD__WR) | _BV(CMD__RST);
#endif
  char cmd;
  while(true){
    cmd=getchar();
    if (cmd=='\r' || cmd=='\n')
      continue;
    if (getchar()!=' ')
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
          switch(c){
            case 'w': // ~WE
              v &= ~ _BV(CMD__WR);
            case 'r': // ~RE
              v &= ~ _BV(CMD__RD);
            case 'c': // ~CS
              v &= ~ _BV(CMD__CS);
            case 'R': // ~RST
              v &= ~ _BV(CMD__RST);
            case 'C': // CLK
              v |= _BV(CMD_CLK);
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
        printf("%x\r\n",dr);
        break;
      case 'W': // write byte
        /* dummy statement for label */;
        #ifdef EMULATOR
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

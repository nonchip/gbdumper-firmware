#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <stdio.h>

#include "uart.h"
#include "pins.h"
#include "shift.h"

int main (void) {
  char cmd;
  uart_init(true); // setup uart and bind to stdio
  shift_init();
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
      case 'C':
        /* dummy statement for label */;
        int v=0;
        bool reading=true;
        char c;
        while(reading){
          c=getchar();
          switch(c){
            case 'w':
              v |= _BV(CMD__WR);
            case 'r':
              v |= _BV(CMD__RD);
            case 'c':
              v |= _BV(CMD__CS);
            case 'R':
              v |= _BV(CMD__RST);
            case 'C':
              v |= _BV(CMD_CLK);
            default:
              reading=false;
              break;
          }
        }
        DDR_CMD |= _BV(CMD__RD) | _BV(CMD_CLK) | _BV(CMD__CS) | _BV(CMD__WR);
        PORT_CMD = v;
        break;
      case 'A':
        /* dummy statement for label */;
        uint16_t a;
        scanf("%x",&a);
        shift_set_addr(a);
        break;
      case 'R':
        /* dummy statement for label */;
        DDR_DATA = 0;
        uint8_t dr = PIN_DATA;
        printf("%x\r\n",dr);
        break;
      case 'W':
        /* dummy statement for label */;
        DDR_DATA = _BV(DATA_0) | _BV(DATA_1) | _BV(DATA_2) | _BV(DATA_3) | _BV(DATA_4) | _BV(DATA_5) | _BV(DATA_6) | _BV(DATA_7);
        uint8_t dw;
        scanf("%hhx",&dw);
        PORT_DATA=dw;
        break;
      default:
        printf("ERR\r\n");
        break;
    }
  }
  return 0;
}

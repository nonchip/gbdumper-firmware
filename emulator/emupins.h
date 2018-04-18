#ifndef _EMUPINS_H
#define _EMUPINS_H

#define PINON(port,mask) port |= mask
#define PINOFF(port,mask) port &= (~(mask))

#define CMD_CLK 0
#define CMD__WR 1
#define CMD__RD 2
#define CMD__CS 3
#define CMD__RST 4

#define PIN_DATA get_port_data()

#define DATA_0 0
#define DATA_1 1
#define DATA_2 2
#define DATA_3 3
#define DATA_4 4
#define DATA_5 5
#define DATA_6 6
#define DATA_7 7

#endif

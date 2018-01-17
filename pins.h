#ifndef _PINS_H
#define _PINS_H

#define PINON(port,mask) port |= mask
#define PINOFF(port,mask) port &= (~(mask))

#define PIN_CMD PINA
#define PORT_CMD PORTA
#define DDR_CMD DDRA

#define CMD_CLK PA0
#define CMD__WR PA1
#define CMD__RD PA2
#define CMD__CS PA3
#define CMD__RST PA4


#define PIN_SHIFT PINB
#define PORT_SHIFT PORTB
#define DDR_SHIFT DDRB

#define SHIFT_SER PB5
#define SHIFT_SRCLK PB7
#define SHIFT_RCLK PB2


#define PIN_DATA PINC
#define PORT_DATA PORTC
#define DDR_DATA DDRC

#define DATA_0 PC0
#define DATA_1 PC1
#define DATA_2 PC2
#define DATA_3 PC3
#define DATA_4 PC4
#define DATA_5 PC5
#define DATA_6 PC6
#define DATA_7 PC7

#endif

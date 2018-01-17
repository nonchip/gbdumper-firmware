// taken from https://appelsiini.net/2011/simple-usart-with-avr-libc/ and modified

#include <avr/io.h>
#include <stdio.h>

#ifndef BAUD
#define BAUD 9600
#endif
#include <util/setbaud.h>

#include "uart.h"
FILE uart_output = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);
FILE uart_input = FDEV_SETUP_STREAM(NULL, uart_getchar, _FDEV_SETUP_READ);

/* http://www.cs.mun.ca/~rod/Winter2007/4723/notes/serial/serial.html */

void uart_init(bool std) {
  UBRRH = UBRRH_VALUE;
  UBRRL = UBRRL_VALUE;
    
#if USE_2X
  UCSRA |= _BV(U2X);
#else
  UCSRA &= ~(_BV(U2X));
#endif

  UCSRC = _BV(URSEL) | _BV(UCSZ1) | _BV(UCSZ0); /* 8-bit data */ 
  UCSRB = _BV(RXEN) | _BV(TXEN);   /* Enable RX and TX */    
  if(std){
    stdout = &uart_output;
    stdin  = &uart_input;
  }
}

int uart_putchar(char c, FILE *stream) {
  loop_until_bit_is_set(UCSRA, UDRE);
  UDR = c;
  return 0;
}

int uart_getchar(FILE *stream) {
  loop_until_bit_is_set(UCSRA, RXC);
  return UDR;
}
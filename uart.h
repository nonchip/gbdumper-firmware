// taken from https://appelsiini.net/2011/simple-usart-with-avr-libc/ and modified
#ifndef _UART_H
#define _UART_H
#include <stdbool.h>
#include <stdio.h>

int uart_putchar(char c, FILE *stream);
int uart_getchar(FILE *stream);

void uart_init(bool std);

/* http://www.ermicro.com/blog/?p=325 */

FILE uart_output;
FILE uart_input;
#endif
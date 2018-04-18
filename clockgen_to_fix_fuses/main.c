#include <avr/io.h>
#include <util/delay.h>
#include <stdbool.h>

int main (void) {
  DDRB = 1<<DDB4;
  PORTB = 0;
  while(true){
    PORTB ^= 1<<PB4;
  }
  return 0;
}

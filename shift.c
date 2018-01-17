#include <avr/io.h>
#include <util/delay.h>
#include "shift.h"
#include "pins.h"

void shift_init(){
  DDR_SHIFT = _BV(SHIFT_SER) | _BV(SHIFT_SRCLK) | _BV(SHIFT_RCLK);
  PORT_SHIFT = 0;
  shift_word(0);
  shift_latch();
}

void shift_bit(bool bit){
  if(bit)
    PINON(PORT_SHIFT, _BV(SHIFT_SER));
  else
    PINOFF(PORT_SHIFT, _BV(SHIFT_SER));
  shift_clock();
}

void shift_byte(uint8_t byte){
  for (int i = 0; i < 8; ++i)
  {
    shift_bit(byte & _BV(8));
    byte=byte<<1;
  }
}

void shift_word(uint16_t word){
  shift_byte(word>>8);
  shift_byte(word&0xff);
}

void shift_clock(){
  PINON(PORT_SHIFT,_BV(SHIFT_SRCLK));
  PINOFF(PORT_SHIFT,_BV(SHIFT_SRCLK));
}

void shift_latch(){
  PINON(PORT_SHIFT, _BV(SHIFT_RCLK));
  _delay_loop_1(1);
  PINOFF(PORT_SHIFT, _BV(SHIFT_RCLK));
  _delay_loop_1(1);
}

void shift_set_addr(uint16_t addr){
  shift_word(addr);
  shift_latch();
}

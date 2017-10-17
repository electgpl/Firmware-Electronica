/* This RF transmitter is based on NEC protocol
   Internal oscillator used @ 8MHz
*/

#include <16F883.h>
#fuses NOMCLR, NOBROWNOUT, NOLVP, INTRC_IO
#use delay(clock = 8MHz)
#use fast_io(B)

void send_signal(unsigned int32 number){
  int8 i;
  // Send 9ms pulse
  output_high(PIN_B4);
  delay_ms(9);
  // Send 4.5ms space
  output_low(PIN_B4);
  delay_us(4500);
  // Send data (32 bits)
  for(i = 0; i < 32; i++){
    // If bit is 1 send 560us pulse and 1680us space
    if(bit_test(number, 31 - i)){
      output_high(PIN_B4);
      delay_us(560);
      output_low(PIN_B4);
      delay_us(1680);
    }
    // If bit is 0 send 560us pulse and 560us space
    else{
      output_high(PIN_B4);
      delay_us(560);
      output_low(PIN_B4);
      delay_us(560);
    }
  }
  // Send end bit
  output_high(PIN_B4);
  delay_us(560);
  output_low(PIN_B4);
  delay_us(560);
}
void main() {
  setup_oscillator(OSC_8MHZ);                    // Set internal oscillator to 8MHz
  output_b(0);
  set_tris_b(0x0F);                              // Configure RB0, RB1, RB2 and RB3 as inputs
  port_b_pullups(0x0F);                          // Enable internal pull-ups for pins RB0,RB1,RB2 and RB3
  while(TRUE){
    if(!input(PIN_B0)){                          // If RB0 button is pressed
      send_signal(0x00FF00FF);
      delay_ms(500);
    }
    if(!input(PIN_B1)){                          // If RB1 button is pressed
      send_signal(0x00FF807F);
      delay_ms(500);
    }
    if(!input(PIN_B2)){                          // If RB2 button is pressed
      send_signal(0x00FF40BF);
      delay_ms(500);
    }
    if(!input(PIN_B3)){                          // If RB3 button is pressed
      send_signal(0x00FF20DF);
      delay_ms(500);
    }
  }
}
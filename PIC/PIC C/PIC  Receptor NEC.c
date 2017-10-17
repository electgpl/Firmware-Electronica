/* This RF receiver is based on NEC protocol
   Internal oscillator used @ 8MHz
*/

#include <16F883.h>
#fuses NOMCLR, NOBROWNOUT, NOLVP, INTRC_IO
#use delay(clock = 8MHz)
#use fast_io(C)

short code_ok = 0;
unsigned int8 nec_state = 0, i;
unsigned int32 rf_code;
#INT_EXT                                         // External interrupt
void ext_isr(void){
  unsigned int16 time;
  if(nec_state != 0){
    time = get_timer1();                         // Store Timer1 value
    set_timer1(0);                               // Reset Timer1
  }
  switch(nec_state){
    case 0 :                                     // Start receiving IR data (we're at the beginning of 9ms pulse)
      setup_timer_1( T1_INTERNAL | T1_DIV_BY_2 );   // Enable Timer1 module with internal clock source and prescaler = 2
      set_timer1(0);                             // Reset Timer1 value
      nec_state = 1;                             // Next state: end of 9ms pulse (start of 4.5ms space)
      i = 0;
      ext_int_edge( H_TO_L );                    // Toggle external interrupt edge
      return;
    case 1 :                                     // End of 9ms pulse
      if((time > 9500) || (time < 8500)){        // Invalid interval ==> stop decoding and reset
        nec_state = 0;                           // Reset decoding process
        setup_timer_1(T1_DISABLED);              // Stop Timer1 module
      }
      else
        nec_state = 2;                           // Next state: end of 4.5ms space (start of 560µs pulse)
      ext_int_edge( L_TO_H );                    // Toggle external interrupt edge
      return;
    case 2 :                                     // End of 4.5ms space
      if((time > 5000) || (time < 4000)){        // Invalid interval ==> stop decoding and reset
        nec_state = 0;                           // Reset decoding process
        setup_timer_1(T1_DISABLED);              // Stop Timer1 module
        return;
      }
      nec_state = 3;                             // Next state: end of 560µs pulse (start of 560µs or 1680µs space)
      ext_int_edge( H_TO_L );                    // Toggle external interrupt edge
      return;
    case 3 :                                     // End of 560µs pulse
      if((time > 700) || (time < 400)){          // Invalid interval ==> stop decoding and reset
        nec_state = 0;                           // Reset decoding process
        setup_timer_1(T1_DISABLED);              // Disable Timer1 module
      }
      else
        nec_state = 4;                           // Next state: end of 560µs or 1680µs space
      ext_int_edge( L_TO_H );                    // Toggle external interrupt edge
      return;
    case 4 :                                     // End of 560µs or 1680µs space
      if((time > 1800) || (time < 400)){         // Invalid interval ==> stop decoding and reset
        nec_state = 0;                           // Reset decoding process
        setup_timer_1(T1_DISABLED);              // Disable Timer1 module
        return;
      }
      if( time > 1000)                           // If space width > 1ms (short space)
        bit_set(rf_code, (31 - i));              // Write 1 to bit (31 - i)
      else                                       // If space width < 1ms (long space)
        bit_clear(rf_code, (31 - i));            // Write 0 to bit (31 - i)
      i++;
      if(i > 31){                                // If all bits are received
        code_ok = 1;                             // Decoding process OK
        disable_interrupts(INT_EXT);             // Disable the external interrupt
      }
      nec_state = 3;                             // Next state: end of 560µs pulse (start of 560µs or 1680µs space)
      ext_int_edge( H_TO_L );                    // Toggle external interrupt edge
  }
}
#INT_TIMER1                                      // Timer1 interrupt (used for time out)
void timer1_isr(void){
  nec_state = 0;                                 // Reset decoding process
  ext_int_edge( L_TO_H );                        // External interrupt edge from high to low
  setup_timer_1(T1_DISABLED);                    // Disable Timer1 module
  clear_interrupt(INT_TIMER1);                   // Clear Timer1 interrupt flag bit
}
void main() {
  setup_oscillator(OSC_8MHZ);                    // Set internal oscillator to 8MHz
  output_c(0);                                   // PORTD initial state
  set_tris_c(0);                                 // Configure PORTD pins as outputs
  enable_interrupts(GLOBAL);                     // Enable global interrupts
  enable_interrupts(INT_EXT_L2H);                // Enable external interrupt
  clear_interrupt(INT_TIMER1);                   // Clear Timer1 interrupt flag bit
  enable_interrupts(INT_TIMER1);                 // Enable Timer1 interrupt
  while(TRUE){
    if(code_ok){                                 // If the mcu successfully receives NEC protocol message
      code_ok = 0;                               // Reset decoding process
      nec_state = 0;
      setup_timer_1(T1_DISABLED);                // Disable Timer1 module
      if(rf_code == 0x00FF00FF)
        output_toggle(PIN_C0);
      if(rf_code == 0x00FF807F)
        output_toggle(PIN_C1);
      if(rf_code == 0x00FF40BF)
        output_toggle(PIN_C2);
      if(rf_code == 0x00FF20DF)
        output_toggle(PIN_C3);
      enable_interrupts(INT_EXT_L2H);            // Enable external interrupt
    }
  }
}
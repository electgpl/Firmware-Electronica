#include <16F676.h>
#FUSES WDT
#FUSES INTRC_IO
#FUSES NOPROTECT
#FUSES NOBROWNOUT
#FUSES NOMCLR
#FUSES NOCPD
#FUSES NOPUT
#use delay(int=4000000,RESTART_WDT)
#define LED PIN_A0
void main(){
   setup_timer_0(RTCC_INTERNAL|RTCC_DIV_1);
   setup_wdt(WDT_2304MS);
   while(true){
      output_low(LED);
      delay_ms(200);
      output_high(LED);
      delay_ms(200);
      sleep();
   }
}
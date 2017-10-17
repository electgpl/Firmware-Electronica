#include <16f877A.h>
#fuses XT,NOWDT,NOPROTECT,NOLVP,PUT,BROWNOUT,NOCPD,NOWRT
#use delay (clock=4000000)
#BYTE PORTB=0x06
#BYTE TRISB=0x86
#include "lcd.c"
long millis;
long tiempo;
long cont;
#INT_TIMER0
void TIMER0_isr(){
   millis=millis+1;
   set_timer0(6);
}
void main(){
   TRISB=0x00;
   PORTB=0;
   output_high(PIN_B0);
   setup_timer_0(RTCC_INTERNAL|RTCC_DIV_4);
   set_timer0(6);
   enable_interrupts(INT_TIMER0);
   enable_interrupts(GLOBAL);
   lcd_init();
   tiempo=millis;
   while(TRUE){
      if(millis-tiempo>=1000){
         lcd_gotoxy(5,2);
         printf(lcd_putc,"\Conv=%lu ",millis-tiempo);
         lcd_gotoxy(1,1);
         printf(lcd_putc,"\Cont=%lu ",cont++);    
         PORTB=PORTB|0b00000001;
         PORTB=PORTB|0b00000100;
         tiempo=millis;
      }  
   }
}
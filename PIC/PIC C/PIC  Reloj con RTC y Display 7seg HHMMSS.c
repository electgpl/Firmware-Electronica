#include <16F883.h>
#FUSES NOWDT
#FUSES XT
#FUSES MCLR
#use delay(clock=4000000)
#define RTC_RST    PIN_C0
#define RTC_SCLK   PIN_C1
#define RTC_IO     PIN_C2
#include <DS1302.C>
#int_timer0 
void timer0_mux(int8 horD, int8 horU, int8 minD, int8 minU, int8 segD, int8 segU){ 
   output_a(0b00000001);
   output_b(horD);
   delay_ms(3);
   output_a(0b00000010);
   output_b(horU);
   delay_ms(3);
   output_a(0b00000100);
   output_b(minD);
   delay_ms(3);
   output_a(0b00001000);
   output_b(minU);
   delay_ms(3);
   output_a(0b00010000);
   output_b(segD);
   delay_ms(3);
   output_a(0b00100000);
   output_b(segU);
   delay_ms(3);   
   set_timer0(0);
} 
int valorDisp[10]={0b00111111,0b00000110,
                   0b01011011,0b01001111,
                   0b01100110,0b01101101,
                   0b01111101,0b00000111,
                   0b01111111,0b01101111};
int8 horU, horD, minU, minD, segU, segD, HHSet, MMSet, HH, MM, SS;
void main(){
   setup_timer_0(RTCC_INTERNAL|RTCC_DIV_64); 
   set_timer0(0); 
   enable_interrupts(INT_TIMER0); 
   enable_interrupts(GLOBAL); 
   while(TRUE){
      if(input(PIN_C5)==1){
         MMSet = MMSet + 1;
         if(MMSet > 59)
            MMSet = 0;
         minD = MMSet/10;
         minU = MMSet%10;
         timer0_mux(valorDisp[horD], valorDisp[horU], valorDisp[minD], 
                    valorDisp[minU], valorDisp[0], valorDisp[0]); 
         rtc_set_datetime(0,0,0,0,HHSet,MMSet);
         delay_ms(200);  
      }   
      if(input(PIN_C4)==1){
         HHSet = HHSet + 1;
         if(HHSet > 23)
            HHSet = 0;
         horD = HHSet/10;
         horU = HHSet%10;
         timer0_mux(valorDisp[horD], valorDisp[horU], valorDisp[minD], 
                    valorDisp[minU], valorDisp[0], valorDisp[0]); 
         rtc_set_datetime(0,0,0,0,HHSet,MMSet);
         delay_ms(200);
      }
      rtc_get_time(HH,MM,SS);
      horD = HH/10;
      horU = HH%10;
      minD = MM/10;
      minU = MM%10;
      segD = SS/10;
      segU = SS%10;
      timer0_mux(valorDisp[horD], valorDisp[horU], valorDisp[minD], 
                 valorDisp[minU], valorDisp[segD], valorDisp[segU]);
   }    
}
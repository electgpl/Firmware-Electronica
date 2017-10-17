#include <16F628A.h>
#FUSES NOWDT
#FUSES XT
#FUSES MCLR
#use delay(clock=4000000)
#int_timer0 
void timer0_mux(int8 minU, int8 segD, int8 segU){ 
   output_a(0b00000001);
   output_b(minU);
   delay_ms(5);
   output_a(0b00000010);
   output_b(segD);
   delay_ms(5);
   output_a(0b00000100);
   output_b(segU);
   delay_ms(5);
   set_timer0(0);
} 
int valorDisp[10]={0b00111111,0b00000110,
                   0b01011011,0b01001111,
                   0b01100110,0b01101101,
                   0b01111101,0b00000111,
                   0b01111111,0b01101111};
int8 segU, segD, minU, i, j, k, cont;
void main(){
   setup_timer_0(RTCC_INTERNAL|RTCC_DIV_64); 
   set_timer0(0); 
   enable_interrupts(INT_TIMER0); 
   enable_interrupts(GLOBAL); 
   while(TRUE){
      for(k=0;k<10;k++){ 
         for(j=0;j<6;j++){
            for(i=0;i<10;i++){
               while(cont<60){
                  if(input(PIN_A3)==1){
                     segU=valorDisp[i];
                     segD=valorDisp[j];
                     minU=valorDisp[k];
                     timer0_mux(minU, segD, segU);
                     delay_ms(1);
                     cont++;
                  }else{
                     while(input(PIN_A3)==0){
                        segU=valorDisp[i];
                        segD=valorDisp[j];
                        minU=valorDisp[k];
                        timer0_mux(minU, segD, segU);
                     }   
                  }
               }cont=0;   
            } 
         } 
      } 
   }     
}
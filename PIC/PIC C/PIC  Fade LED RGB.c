#include <16F628A.h>
#FUSES NOWDT, HS, NOMCLR 
#use delay(clock=4000000)
#int_timer0 
void pwm_duty(int pwm0, int pwm1, int pwm2){
   int cont;
   if(cont>15)
      cont=0;
   else
      cont++; 
   if(cont>0&&cont<pwm0)
      output_high(PIN_B0);
   else
      output_low(PIN_B0);
   if(cont>0&&cont<pwm1)
      output_high(PIN_B1);
   else
      output_low(PIN_B1);
   if(cont>0&&cont<pwm2)
      output_high(PIN_B2);
   else
      output_low(PIN_B2);
   set_timer0(0);      
}
void main(){
   setup_timer_0(RTCC_INTERNAL|RTCC_DIV_1); 
   set_timer0(0); 
   enable_interrupts(INT_TIMER0); 
   enable_interrupts(GLOBAL); 
   while(true){
      int r, g, b;
      for(b=0;b<16;b++){
         pwm_duty(16,0,b);
         delay_ms(50);
      }
      for(r=16;r>1;r--){
         pwm_duty(r,0,16);
         delay_ms(50);
      }
      for(g=0;g<16;g++){
         pwm_duty(0,g,16);
         delay_ms(50);
      }
      for(b=16;b>1;b--){
         pwm_duty(0,16,b);
         delay_ms(50);
      }      
      for(r=0;r<16;r++){
         pwm_duty(r,16,0);
         delay_ms(50);
      }
      for(g=16;g>1;g--){
         pwm_duty(16,g,0);
         delay_ms(50);
      }
   }
}
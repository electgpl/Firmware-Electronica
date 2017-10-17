#include <16F628a.h>
#FUSES NOWDT
#FUSES INTRC_IO
#FUSES NOMCLR
#use delay(int=4000000)
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
void fadeRGB(void){
   int r, g, b, i;
   for(b=0;b<16;b++){
      pwm_duty(16,0,b);
      for(i=0;i<8;i++){
         output_a(i); 
         delay_ms(1);
      }
   }
   for(r=16;r>1;r--){
      pwm_duty(r,0,16);
      for(i=0;i<8;i++){
         output_a(i); 
         delay_ms(1);
      }
   }
   for(g=0;g<16;g++){
      pwm_duty(0,g,16);
      for(i=0;i<8;i++){
         output_a(i); 
         delay_ms(1);
      }
   }
   for(b=16;b>1;b--){
      pwm_duty(0,16,b);
      for(i=0;i<8;i++){
         output_a(i); 
         delay_ms(1);
      }
   } 
   for(r=0;r<16;r++){
      pwm_duty(r,16,0);
      for(i=0;i<8;i++){
         output_a(i); 
         delay_ms(1);
      }
   }
   for(g=16;g>1;g--){
      pwm_duty(16,g,0);
      for(i=0;i<8;i++){
         output_a(i); 
         delay_ms(1);
      }
   }
}
void mixRGB(void){
   pwm_duty(8,8,8);
   output_a(0b00000000);
   delay_ms(1);
   pwm_duty(0,0,16);
   output_a(0b00000001);
   delay_ms(1);
   pwm_duty(0,16,0);
   output_a(0b00000010);
   delay_ms(1);
   pwm_duty(0,16,16);
   output_a(0b00000011);
   delay_ms(1);
   pwm_duty(16,0,0);
   output_a(0b00000100);
   delay_ms(1);
   pwm_duty(16,0,16);
   output_a(0b00000101);
   delay_ms(1);
   pwm_duty(16,16,0);
   output_a(0b00000110);
   delay_ms(1);
   pwm_duty(16,16,16);
   output_a(0b00000111);
   delay_ms(1);
}
void flashRGB(void){
   pwm_duty(8,8,8);
   output_a(0b00000000);
   delay_ms(100);
   pwm_duty(0,0,16);
   output_a(0b00000001);
   delay_ms(100);
   pwm_duty(0,16,0);
   output_a(0b00000010);
   delay_ms(100);
   pwm_duty(0,16,16);
   output_a(0b00000011);
   delay_ms(100);
   pwm_duty(16,0,0);
   output_a(0b00000100);
   delay_ms(100);
   pwm_duty(16,0,16);
   output_a(0b00000101);
   delay_ms(100);
   pwm_duty(16,16,0);
   output_a(0b00000110);
   delay_ms(100);
   pwm_duty(16,16,16);
   output_a(0b00000111);
   delay_ms(100);
}
void coloresRGB(int r, int g, int b){
   int i;
   for(i=0;i<8;i++){
      pwm_duty(r,g,b);
      for(i=0;i<8;i++){
         output_a(i); 
         delay_ms(1);
      }
   }
}
void main(){
   setup_timer_0(RTCC_INTERNAL|RTCC_DIV_1); 
   set_timer0(0); 
   enable_interrupts(INT_TIMER0); 
   enable_interrupts(GLOBAL); 
   while(true){
      int i, r, g, b;
      for(i=0;i<5;i++)
         fadeRGB();
      for(i=0;i<100;i++)
         mixRGB();
      for(i=0;i<10;i++)
         flashRGB();
      for(r=0;r<8;r++)
         for(i=0;i<100;i++)
            coloresRGB(b,g,r);
      for(g=0;g<8;g++)
         for(i=0;i<100;i++)
            coloresRGB(b,g,r);
      for(b=0;b<8;b++)
         for(i=0;i<100;i++)
            coloresRGB(b,g,r);
   }
}
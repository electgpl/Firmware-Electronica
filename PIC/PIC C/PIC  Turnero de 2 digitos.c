#include <16F883.h>
#FUSES NOWDT
#FUSES NOMCLR
#use delay(int=4000000)
void main(){
   int cuenta, 
       decenas, 
       unidades;
   int display[10]={0b00111111,0b00000110,
                    0b01011011,0b01001111,
                    0b01100110,0b01101101,
                    0b01111101,0b00000111,
                    0b01111111,0b01101111};
   while(true){
      if(input(PIN_A0)==1){
         delay_ms(200);
         if(input(PIN_A0)==1)
            if(cuenta<100)
               cuenta++;
            else
               cuenta=0;
      }   
      if(input(PIN_A1)==1){
         delay_ms(200);
         if(input(PIN_A1)==1)
            if(cuenta<1)
               cuenta=0;
            else
               cuenta--;
      }   
      decenas=cuenta/10;
      unidades=cuenta%10;
      output_c(display[decenas]);
      output_b(display[unidades]);
   }   
}
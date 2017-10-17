#include <16F883.h>
#fuses XT,NOWDT,NOPROTECT,PUT,NOLVP,NOMCLR,INTRC
#use delay(int=4000000)
#use rs232(baud=9600, xmit=pin_c6, rcv=pin_c7, bits=8, parity=N)
#define trigger PIN_A0
#define echo input(PIN_A1)
void main()
{
   unsigned int distancia;  
   while(true){
      output_high(trigger);
      delay_us(40);
      output_low(trigger); 
      while(echo==0);
      while(echo==1){
         distancia++;
         delay_us(58);
      } 
      delay_ms(200);
      if(distancia>70){
         printf("Fuera de rango 70cm maximo\r");
         delay_ms(500);
         printf("                          \r");
      }   
      else{
         printf("Distancia[cm]: %3d\r",distancia); 
         distancia = 0;
      }   
   }
}
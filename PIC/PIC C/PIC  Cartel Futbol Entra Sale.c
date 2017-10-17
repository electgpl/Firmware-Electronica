#include <16F628A.h>
#FUSES NOWDT                    //No Watch Dog Timer
#FUSES INTRC_IO                 //Internal RC Osc, no CLKOUT
#FUSES NOPUT                    //No Power Up Timer
#FUSES NOPROTECT                //Code not protected from reading
#FUSES NOBROWNOUT               //No brownout reset
#FUSES MCLR                     //Master Clear pin enabled
#FUSES NOLVP                    //No low voltage prgming, B3(PIC16) or B5(PIC18) used for I/O
#FUSES NOCPD                    //No EE protection
#use delay(int=4000000)
#int_timer0 
void timer0_mux(int8 minD, int8 minU, int8 segD, int8 segU){ 
   output_a(0b00001110);
   output_b(minD);
   delay_ms(10);
   output_a(0b00001101);
   output_b(minU);
   delay_ms(10);
   output_a(0b00001011);
   output_b(segD);
   delay_ms(10);
   output_a(0b00000111);
   output_b(segU);
   delay_ms(10);   
   set_timer0(0);
} 
void main(){
   setup_timer_0(RTCC_INTERNAL|RTCC_DIV_8); 
   set_timer0(0); 
   enable_interrupts(INT_TIMER0); 
   enable_interrupts(GLOBAL); 
   int cuentaRojo, 
       decenasRojo, 
       unidadesRojo,
       cuentaVerde, 
       decenasVerde, 
       unidadesVerde;
   timer0_mux(10,10,10,10);     
   while(TRUE){    
      if(input(PIN_B4)==1){
         delay_ms(50);
         if(input(PIN_B4)==1)
            if(cuentaVerde<100)
               cuentaVerde++;
            else
               cuentaVerde=0;
      }   
      if(input(PIN_B5)==1){
         delay_ms(50);
         if(input(PIN_B5)==1)
            if(cuentaVerde<1)
               cuentaVerde=0;
            else
               cuentaVerde--;
      }   
      decenasVerde=cuentaVerde/10;
      unidadesVerde=cuentaVerde%10;
      if(decenasVerde==0 && unidadesVerde==0){
         decenasVerde=10;
         unidadesVerde=10;
      }
      if(decenasVerde>9){
         decenasVerde=10;
         unidadesVerde=10;
      }      
      if(decenasVerde==0)
         decenasVerde=10;
      timer0_mux(decenasRojo,unidadesRojo,decenasVerde,unidadesVerde); 
      if(input(PIN_B6)==1){
         delay_ms(50);
         if(input(PIN_B6)==1)
            if(cuentaRojo<100)
               cuentaRojo++;
            else
               cuentaRojo=0;
      }   
      if(input(PIN_B7)==1){
         delay_ms(50);
         if(input(PIN_B7)==1)
            if(cuentaRojo<1)
               cuentaRojo=0;
            else
               cuentaRojo--;
      }   
      decenasRojo=cuentaRojo/10;
      unidadesRojo=cuentaRojo%10;
      if(decenasRojo==0 && unidadesRojo==0){
         decenasRojo=10;
         unidadesRojo=10;
      }
      if(decenasRojo>9){
         decenasRojo=10;
         unidadesRojo=10;
      }
      if(decenasRojo==0)
         decenasRojo=10;
      timer0_mux(decenasRojo,unidadesRojo,decenasVerde,unidadesVerde);      
   }
}
#include <16F883.h>
#device adc=10
#FUSES NOWDT, INTRC_IO, NOPUT, MCLR
#use delay(int=4000000)
#include <LCD.C>
void main(){
   setup_adc_ports(sAN0|VSS_VDD);
   setup_adc(ADC_CLOCK_DIV_32);
   lcd_init();
   int16 salida;
   char caracter;
   while(true){
      set_adc_channel(0);
      salida=read_adc();
      delay_us(30);
      lcd_gotoxy(1,1);
      printf(lcd_putc,"Valor: %4lu",salida);
      lcd_gotoxy(1,2);
      printf(lcd_putc,"Caracter: %c",caracter);
      if(salida<350)
         caracter='@';
      if(salida>360&&salida<384)   
         caracter='+';
      if(salida>386&&salida<401)
         caracter='=';
      if(salida>403&&salida<413)
         caracter='0';
      if(salida>415&&salida<435)
         caracter='C';
      if(salida>440&&salida<465)
         caracter='-';
      if(salida>469&&salida<485)
         caracter='3';
      if(salida>490&&salida<510)
         caracter='2';
      if(salida>515&&salida<540)
         caracter='1';
      if(salida>550&&salida<585)
         caracter='*';
      if(salida>595&&salida<625)
         caracter='6';
      if(salida>630&&salida<670)
         caracter='5';
      if(salida>680&&salida<720)
         caracter='4';
      if(salida>750&&salida<800)
         caracter='/';
      if(salida>820&&salida<880)
         caracter='9';
      if(salida>900&&salida<940)
         caracter='8';
      if(salida>950)
         caracter='7';        
   }
}
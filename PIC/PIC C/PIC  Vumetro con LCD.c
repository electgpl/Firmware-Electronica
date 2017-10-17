#include<16f883.h>
#device adc=8
#use delay(int=4000000)
#include<LCD.C> 
double map(float valor, float entradaMin, 
           float entradaMax, float salidaMin, 
           float salidaMax){
   return ((((valor-entradaMin)*
             (salidaMax-salidaMin))/
             (entradaMax-entradaMin))+salidaMin);
}  
void main(){
   setup_adc_ports(sAN0|sAN1|VSS_VDD);
   setup_adc(ADC_CLOCK_DIV_2);
   lcd_init();        
   int8 salida1, salida2, i, j, leeAdc1, leeAdc2;
   while(true){
      set_adc_channel(0);
      delay_us(10);
      leeAdc1 = read_adc();
      salida1 = map(leeADC1, 0, 255, 0, 16);
      set_adc_channel(1);
      delay_us(10);
      leeAdc2 = read_adc();
      salida2 = map(leeADC2, 0, 255, 0, 16);      
      lcd_init();
      for(i=1;i<salida1+1;i++){
         lcd_gotoxy(i,1);
         lcd_putc(0xFF);
      }
      for(j=1;j<salida2+1;j++){
         lcd_gotoxy(j,2);
         lcd_putc(0xFF);
      }      
   }
}
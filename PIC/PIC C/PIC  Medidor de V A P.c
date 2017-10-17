#include <16F883.h>
#device adc=8
#use delay(int=4000000)
#include <LCD.C>
void main(){
   setup_adc_ports(sAN0|sAN1|VSS_VDD);
   setup_adc(ADC_CLOCK_DIV_2);
   int16 i, adc1, adc2, adca1, adca2, tension, corriente, potencia;
   lcd_init();
   lcd_putc("Espere...");
   while(true){
      set_adc_channel(0);
      delay_us(20);
      adc1=read_adc();
      for(i=0;i<300;i++){
         adca1=adc1+adca1;
         delay_ms(2);
      }    
      tension=adca1/300;
      set_adc_channel(1);
      delay_us(20);
      adc2=read_adc();
      for(i=0;i<300;i++){
         adca2=adc2+adca2;
         delay_ms(2);
      }          
      corriente=adca2/300;
      potencia=corriente*tension;
      lcd_gotoxy(1,1);
      printf(lcd_putc,"V: %ld   A: %ld ",tension,corriente);
      lcd_gotoxy(1,2);
      printf(lcd_putc,"P: %ld   ",potencia);
      adca1=0; 
      adca2=0; 
   }
}
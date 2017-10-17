#include <16F883.h>
#device adc=10
#use delay(int=4000000)
#include <LCD.C>
#include <math.h>
void main(){
   setup_adc_ports(sAN0|VSS_VDD);
   setup_adc(ADC_CLOCK_DIV_2);
   int16 i;
   float adc1, valorMax=0;
   lcd_init();
   while(true){
	  do{
         set_adc_channel(0);
         adc1=read_adc()*5.0/1023.0;
         delay_us(20);
      }while(adc1!=0);
      for(i=0;i<300;i++){
         set_adc_channel(0);
         adc1=read_adc()*5.0/1023.0;
         delay_us(60);
         valorMax=adc1*adc1+valorMax;
      }  
      lcd_gotoxy(1,1);
      printf(lcd_putc,"Voltimetro Trms");
      lcd_gotoxy(1,2);
      printf(lcd_putc,"Val RMS: %3.0f ",(sqrt(valorMax/300)*75)+44);
      valorMax=0;
   }
}
#include <16F883.h>
#device adc=10
#use delay(int=4000000)
#include <LCD.C>
void main(){
   setup_adc_ports(sAN0|sAN1|VSS_VDD);
   setup_adc(ADC_CLOCK_DIV_2);
   lcd_init();
   int i;
   int16 currentADC, voltajeADC;
   float current, voltaje, power, currentHr, currentInc;
   while(TRUE){
      for(i=0;i<10;i++){
         set_adc_channel(0);
         delay_us(10);
         currentADC=read_adc(); 
         set_adc_channel(1);
         delay_us(10);
         voltajeADC=read_adc();      
         current=(5.0*currentADC/1023.0)/1.6665;
         voltaje=((5.0*voltajeADC/1023.0)/0.151)-(current/10);
         power=voltaje*current;
         currentHr=current/3600.0;
         delay_ms(100);
      }   
      lcd_gotoxy(1,1);       
      if(voltaje>9.999)
         printf(lcd_putc," %02.2fV %02.3fA ",voltaje,current);
      else               
         printf(lcd_putc,"  %02.2fV %02.3fA",voltaje,current);     
      lcd_gotoxy(1,2);
      currentInc = currentInc + currentHr;
      if(power>9.999)
         printf(lcd_putc," %02.2fW %02.3fAh ",power,currentInc);  
      else               
         printf(lcd_putc,"  %02.2fW %02.3fAh",power,currentInc);         
   }   
}
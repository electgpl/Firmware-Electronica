#include <16F883.h>
#device adc=8
#use delay(int=4000000)
#include <LCD.C>
void main(){
   setup_adc_ports(sAN0|sAN1|sAN2|VSS_VDD);
   setup_adc(ADC_CLOCK_DIV_2);
   setup_spi(SPI_SS_DISABLED);
   setup_timer_0(RTCC_INTERNAL|RTCC_DIV_1);
   setup_timer_1(T1_DISABLED);
   setup_timer_2(T2_DISABLED,0,1);
   setup_ccp1(CCP_OFF);
   setup_comparator(NC_NC_NC_NC);
   lcd_init();
   int8 currentADC, voltajeADC;
   float current, voltaje, power;
   while(TRUE){
      set_adc_channel(0);
      delay_us(10);
      currentADC = read_adc();
      current = (5.0 * currentADC / 255)/4.7;
      set_adc_channel(1);
      delay_us(10);
      voltajeADC = read_adc();      
      voltaje = ((5.0 * voltajeADC / 255)*3.2)-current*4.7;
      power = voltaje * current;
      lcd_gotoxy(1,1);      
      if(voltaje>9.99)
      printf(lcd_putc,"V:%02.2f Po:%01.2f ",voltaje,power);
      else
         printf(lcd_putc,"V:%02.2f  Po:%01.2f ",voltaje,power);
      lcd_gotoxy(1,2);
      printf(lcd_putc,"A:%01.3f         ",current);  
   }  
}
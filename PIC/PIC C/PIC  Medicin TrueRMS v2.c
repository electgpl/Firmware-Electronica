#include <16F883.h>
#device adc=10
#use delay(int=4000000)
#define LCD_ENABLE_PIN  PIN_B2
#define LCD_RS_PIN      PIN_B0
#define LCD_RW_PIN      PIN_B1
#define LCD_DATA4       PIN_B4
#define LCD_DATA5       PIN_B5
#define LCD_DATA6       PIN_B6
#define LCD_DATA7       PIN_B7
#include <LCD.C>
#include <math.h>
void main(){
   setup_adc_ports(sAN0|sAN1|VSS_VDD);
   setup_adc(ADC_CLOCK_DIV_2);
   int16 i;
   float adc1, valorMax1=0, adc2, valorMax2=0;
   float tension, corriente, potencia;
   lcd_init();
   while(true){
      for(i=0;i<300;i++){
         set_adc_channel(0);
         adc1=read_adc()*5.0/1023.0;
         delay_us(60);
         valorMax1=adc1*adc1+valorMax1;
      }  
      for(i=0;i<300;i++){
         set_adc_channel(1);
         adc2=read_adc()*5.0/1023.0;
         delay_us(60);
         valorMax2=adc2*adc2+valorMax2;
      }  
      tension=sqrt(valorMax1/300)*136;
      corriente=sqrt(valorMax2/300)*0.61;
      potencia=tension*corriente;
      lcd_gotoxy(1,1);
      printf(lcd_putc,"RMS    W: %3.0f ",potencia);
      lcd_gotoxy(1,2);
      printf(lcd_putc,"V: %3.0f A: %3.3f ",tension, corriente);
      valorMax1=0;
      valorMax2=0;
   }
}
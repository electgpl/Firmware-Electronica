#include <16F883.h>
#device adc=8
#use delay(clock=4000000)
#use rs232(baud=9600,parity=N,xmit=PIN_B7,rcv=PIN_B6,bits=8)
#define LCD_ENABLE_PIN  PIN_A0 
#define LCD_RS_PIN      PIN_B4
#define LCD_RW_PIN      PIN_B5
#define LCD_DATA4       PIN_B0
#define LCD_DATA5       PIN_B1
#define LCD_DATA6       PIN_B2
#define LCD_DATA7       PIN_B3 
#include <LCD.C>
void main(){
   setup_adc_ports(sAN1|VSS_VDD);
   setup_adc(ADC_CLOCK_DIV_2);
   setup_timer_0(RTCC_INTERNAL|RTCC_DIV_1);
   setup_timer_1(T1_DISABLED);
   setup_timer_2(T2_DIV_BY_16,255,1);
   setup_ccp1(CCP_PWM);
   set_pwm1_duty(512);
   setup_comparator(NC_NC_NC_NC);
   lcd_init();
   lcd_gotoxy(1,1);
   printf(lcd_putc,"boot...             ");
   delay_ms(1000);
   unsigned int8 varInt, adc1;
   while(TRUE){
      varInt=getc(); 
      set_pwm1_duty(varInt);
      lcd_gotoxy(1,1);
      printf(lcd_putc,"valor: %u         ",varInt);
      delay_ms(100);
      set_adc_channel(1);
      adc1=read_adc()+13;
      delay_us(60);
      lcd_gotoxy(1,2);
      printf(lcd_putc,"envia: %u         ",adc1);
      printf("%u",adc1);
      delay_ms(100);
   }
}
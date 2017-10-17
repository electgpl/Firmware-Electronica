#include <16F883.h>
#device adc=8
#use delay(clock=4000000)
byte CONST DISPLAY[10]={0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F};
void main(){
   setup_adc_ports(sAN0|VSS_VDD);
   setup_adc(ADC_CLOCK_DIV_2);
   int8 cuenta=0, dec=0, uni=0, i=0;  
   while(TRUE){  
      for(i=0;i<100;i++){
         output_c(0b00000001);
         output_b(DISPLAY[dec]);
         delay_ms(15);
         output_c(0b00000010);
         output_b(DISPLAY[uni]);
         delay_ms(15);
         dec=cuenta/10;
         uni=cuenta%10;   
      }
      set_adc_channel(0);
      delay_us(20);
      cuenta=read_adc()*2;
   }   
}
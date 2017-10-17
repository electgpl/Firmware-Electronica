#include <16F883.h>
#device adc=10
#use delay(int=4000000)
#define OUT_220 PIN_A2
#define LED_LO PIN_A3
#define LED_OK PIN_A4
#define LED_HI PIN_A5
void main(){
   int16 salida;
   setup_adc_ports(sAN0);
   setup_adc(ADC_CLOCK_DIV_2);
   output_low(OUT_220);
   output_high(LED_LO);
   output_high(LED_OK);
   output_high(LED_HI);  
   while(true){
      set_adc_channel(0);
      delay_us(10);
      salida = read_adc();
      switch(true){
      case (salida > 980):              //Si es > 4.1V
         output_low(OUT_220);
         output_low(LED_LO);
         output_low(LED_OK);
         output_high(LED_HI); 
         delay_ms(3000);
         break;
      case (salida > 800 && salida < 430):   //Si es > 800 y < 430
         output_high(OUT_220);
         output_low(LED_LO);
         output_high(LED_OK);
         output_low(LED_HI); 
         break;
      case (salida < 390):               //Si es < 390
         output_low(OUT_220);
         output_high(LED_LO);
         output_low(LED_OK);
         output_low(LED_HI);
         delay_ms(3000);
         break;      
      }  
   }
}
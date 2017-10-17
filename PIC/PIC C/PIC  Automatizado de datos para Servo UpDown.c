#include <16F883.h>
#device adc=8
#use delay(clock=4000000)
#use rs232(baud=9600,parity=N,xmit=PIN_C6,rcv=PIN_C7,bits=8)
#define MUESTRAS 50
void main(){
   setup_adc_ports(sAN0|VSS_VDD);
   setup_adc(ADC_CLOCK_DIV_2);
   unsigned int8 i=0, valor=0;
   printf("Cargando Memoria, esto puede tardar %f segundos.  \r",MUESTRAS*0.20003);
   for(i=0;i<MUESTRAS;i++){
      set_adc_channel(0);
      valor=read_adc();
      delay_us(30);
      write_eeprom(i,valor);
      delay_ms(200);
   }
   printf("Memoria completa, %d valores escritos.  \r",MUESTRAS);
   i=0;
   while(TRUE){   
      while(input(PIN_B0)==1&&i<MUESTRAS){
         printf("Valor %3d: %u  \r",i,read_eeprom(i));
         delay_ms(200);
         i++;
      }
      while(input(PIN_B1)==1&&i>0){
         printf("Valor %3d: %u  \r",i,read_eeprom(i));
         delay_ms(200);
         i--;
      }
   }
}
#include <16F883.h>
#device adc=8
#use delay(clock=4000000)
#use rs232(baud=9600,parity=N,xmit=PIN_C6,rcv=PIN_C7,bits=8)
void main(){
   setup_adc_ports(sAN0|VSS_VDD);
   setup_adc(ADC_CLOCK_DIV_2);
   unsigned int8 i, valor;
   int1 flag1=0, flag2=0;
   while(TRUE){
      if(input(PIN_B0)==1&&flag1==0){  
         printf("Cargando memoria  \r");
         for(i=0;i<20;i++){
            set_adc_channel(0);
            valor=read_adc();
            delay_us(30);
            write_eeprom(i,valor);
            delay_ms(50);
            printf("Valor %3d: %u  \r",i,valor);
         }
         printf("Memoria completa  \r");
         flag1=1;
      }else if(input(PIN_B0)==0&&flag2==0){
         printf("Mostrando memoria  \r");
         for(i=0;i<20;i++){
            printf("Valor %3d: %u  \r",i,read_eeprom(i));
            delay_ms(50);
         }
         printf("Fin de datos  \r");
         flag2=1;
      }
   }
}
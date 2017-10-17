#include <16F887.h>
#device adc=10
#fuses HS,NOWDT,NOPROTECT,NOLVP
#use delay(clock=20000000)
#include <HDM64GS12.c>
#include <graphics.c>
void main(){
   int16 voltajeADC, voltajeADC_ant, 
         corrienteADC, corrienteADC_ant;
   setup_adc_ports(sAN0|sAN1|VSS_VDD);
   setup_adc(ADC_CLOCK_DIV_2);
   char nombre[]="Electgpl";
   char parametro[]="   [V]       [A]   ";
   char voltaje[9];
   char corriente[9];
   glcd_init(ON);
   glcd_rect(1, 1, 127, 63, NO, ON);
   glcd_rect(3, 3, 125, 13, NO, ON);
   glcd_text57(41, 5, nombre, 1, ON);
   glcd_rect(3, 15, 125, 25, NO, ON);
   glcd_text57(8, 17, parametro, 1, ON);
   glcd_rect(3, 27, 125, 61, NO, ON);
   while(true){
      set_adc_channel(0);
      delay_us(10);
      voltajeADC=read_adc();
      if(voltajeADC_ant != voltajeADC){
         glcd_text57(13, 38, voltaje, 2, OFF); 
         sprintf(voltaje, "%1.2f", (float)voltajeADC*5/1023.0);
         voltaje[4] = '\0';
         glcd_text57(13, 38, voltaje, 2, ON);  
         voltajeADC_ant=voltajeADC;
      }
      set_adc_channel(1);
      delay_us(10);
      corrienteADC=read_adc();
      if(corrienteADC_ant != corrienteADC){
         glcd_text57(70, 38, corriente, 2, OFF); 
         sprintf(corriente, "%1.2f", (float)corrienteADC*5/1023.0);
         corriente[4] = '\0';
         glcd_text57(70, 38, corriente, 2, ON);  
         corrienteADC_ant=corrienteADC;
      }  
   }
}
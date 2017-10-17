#include <16F887.h>
#device adc=10
#fuses HS,NOWDT,NOPROTECT,NOLVP
#use delay(clock=20000000)
#include <HDM64GS12.c>
#include <graphics.c>
void main(){
   int16 voltajeADC, voltajeADC_ant,
         cpsADC,cpsADC_ant,
         corrienteADC, corrienteADC_ant;
   setup_adc_ports(sAN0|sAN1|sAN2|VSS_VDD);
   setup_adc(ADC_CLOCK_DIV_2);
   char cps[]="CPS";
   char volt[]="V";
   char amper[]="A";
   char voltaje[9];
   char minuto[9];
   char corriente[9];
   glcd_init(ON);
   glcd_line(0, 25, 125, 25, ON);
   glcd_text57(45,12,volt,1,ON);
   glcd_text57(105,12,amper,1,ON);
   glcd_text57(105,55,cps,1,ON);
   while(true){
      set_adc_channel(0);
      delay_us(10);
      voltajeADC=read_adc();
      if(voltajeADC_ant != voltajeADC){
         glcd_text57(5, 3, voltaje, 2, OFF);
         sprintf(voltaje, "%1.1f", (float)voltajeADC*5/1023.0);
         voltaje[3] = '\0';
         glcd_text57(5, 3, voltaje, 2, ON);  
         voltajeADC_ant=voltajeADC;
      }
      set_adc_channel(1);
      delay_us(10);
      corrienteADC=read_adc();
      if(corrienteADC_ant != corrienteADC){
         glcd_text57(65, 3, corriente, 2, OFF);
         sprintf(corriente, "%1.1f", (float)corrienteADC*5/1023.0);
         corriente[3] = '\0';
         glcd_text57(65, 3, corriente, 2, ON);        
         corrienteADC_ant=corrienteADC;
      }
      set_adc_channel(2);
      delay_us(10);
      cpsADC=read_adc();
      if(cpsADC_ant != cpsADC){
         glcd_text57(30, 40, minuto, 3, OFF);
         sprintf(minuto, "%02.2f", (float)cpsADC*5/1023.0);
         minuto[5] = '\0';
         glcd_text57(30, 40, minuto, 3, ON);
         cpsADC_ant=cpsADC;        
      }
   }
}
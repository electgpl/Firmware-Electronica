#include <16F883.h>
#device adc=8
#use delay(int=4000000)
#include <LCD.C>
#define LED PIN_A1
void main(){
   setup_adc_ports(sAN0|VSS_VDD);
   setup_adc(ADC_CLOCK_DIV_2);
   lcd_init();
   int i;
   int8 salida;
   char caracter;
   const int keyValueMinor[17]={0  ,54 , 56, 59, 62, 68, 73, 77, 83, 92,102,112,124,150,170,200,240};
   const int keyValueMajor[17]={53 ,56 , 58, 61, 67, 72, 76, 82, 90,100,110,120,132,160,190,220,255};
   const char   KeySimbols[17]={'_','D','#','0','*','C','9','8','7','B','6','5','4','A','3','2','1'};  
   while(true){
      set_adc_channel(0);
      salida=read_adc();
      delay_us(30);
      for(i=0;i<18;i++){
         if(salida>keyValueMinor[i]&&salida<keyValueMajor[i])
            caracter=KeySimbols[i];
         delay_ms(1);
      }
      lcd_gotoxy(1,1);
      printf(lcd_putc,"Valor: %3u",salida);
      lcd_gotoxy(1,2);
      printf(lcd_putc,"Caracter: %c",caracter);
      output_toggle(LED); 
   }
}
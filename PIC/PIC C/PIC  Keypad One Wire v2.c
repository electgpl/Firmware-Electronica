#include <16F883.h>
#device adc=10
#FUSES NOWDT, INTRC_IO, NOPUT, MCLR
#use delay(int=4000000)
#include <LCD.C>
void main(){
   setup_adc_ports(sAN0|VSS_VDD);
   setup_adc(ADC_CLOCK_DIV_32);
   lcd_init();
   int i;
   int16 salida;
   char caracter;
   const int keyValueMinor[17]={100,360,386,403,415,440,469,490,515,550,595,630,680,750,820,900,950};
   const int keyValueMajor[17]={350,384,401,413,435,465,485,510,540,585,625,670,720,800,880,940,1024};
   const char   KeySimbols[17]={'@','+','=','0','C','-','3','2','1','*','6','5','4','/','9','8','7'};
   while(true){
      set_adc_channel(0);
      salida=read_adc();
      delay_us(30);
      lcd_gotoxy(1,1);
      printf(lcd_putc,"Valor: %4lu",salida);
      lcd_gotoxy(1,2);
      printf(lcd_putc,"Caracter: %c",caracter);
      for(i=0;i<18;i++){
         if(salida>keyValueMinor[i]&&salida<keyValueMajor[i])
            caracter=KeySimbols[i];
      }
   }
}
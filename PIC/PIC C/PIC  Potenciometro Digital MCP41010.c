#include <16F883.h>
#FUSES HS
#FUSES MCLR
#use delay(clock=4000000)
#use rs232(baud=9600, xmit=pin_c6, rcv=pin_c7, bits=8, parity=N)
#define CS   PIN_B2
#define SCLK PIN_B1
#define SI   PIN_B0
void setPote(int valor) 
{
   int8 i;
   int8 palabra[2];
   palabra[0]=valor;
   palabra[1]=0x11;                             //Posicion 0x11 asigna el valor, 0x21 guarda el valor
   output_low(SCLK);
   output_low(CS);
   for(i=1;i<=16;++i) {
      output_bit(SI, shift_left(palabra,2,0));
      output_high(SCLK);
      output_low(SCLK);
   }
   output_high(CS);
}
int8 valPote=0;
void main()
{
   printf("Listo!\r");
   while(true){
      if(input(PIN_C0)==1)
      {
         if(valPote<255)
         {
            valPote++;
            setPote(valPote);
            printf("valPote: %3u \r",valPote);
         }
      }
      if(input(PIN_C1)==1)
      {
         if(valPote>0)
         {  
            valPote--;
            setPote(valPote);
            printf("valPote: %3u \r",valPote);
         }
      }
   }
}
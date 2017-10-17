#include <16F876A.h>
#use delay(clock=4000000)
#use rs232(baud=9600,parity=N,xmit=PIN_C6,rcv=PIN_C7,bits=8)
void main(){
   char charIn;
   printf("---------------------------\r");
   printf("RELE BLUETOOTH DE 4 CANALES\r");
   printf("  electgpl.blogspot.com.ar \r");
   printf("---------------------------\r");
   printf("-  q: B0=1  ---  a: B0=0  -\r");
   printf("-  w: B1=1  ---  s: B1=0  -\r");
   printf("-  e: B2=1  ---  d: B2=0  -\r");
   printf("-  r: B3=1  ---  f: B3=0  -\r");
   printf("---------------------------\r");
   printf("\r");
   while(true){
      if(kbhit()){
         charIn=getch();
         switch(charIn){
         case 'q':
            output_high(PIN_B0);
            printf("B0 ENCENDIDO\r");
            break;
         case 'a':
            output_low(PIN_B0);
            printf("B0 APAGADO\r");
            break;
         case 'w':
            output_high(PIN_B1);
            printf("B1 ENCENDIDO\r");
            break;
         case 's':
            output_low(PIN_B1);
            printf("B1 APAGADO\r");
            break;
         case 'e':
            output_high(PIN_B2);
            printf("B2 ENCENDIDO\r");
            break;
         case 'd':
            output_low(PIN_B2);
            printf("B2 APAGADO\r");
            break;
         case 'r':
            output_high(PIN_B3);
            printf("B3 ENCENDIDO\r");
            break;
         case 'f':
            output_low(PIN_B3);
            printf("B3 APAGADO\r");
            break;
         default:
            output_low(PIN_B0);
            output_low(PIN_B1);
            output_low(PIN_B2);
            output_low(PIN_B3);
            break;
         }
      }
   }
}
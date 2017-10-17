#include <16F628A.h>
#use delay(int=4000000)
#use rs232(baud=9600,parity=N,xmit=PIN_B2,rcv=PIN_B1,bits=8)
void main(){
   char charIn;
   while(true){
      if(kbhit())
         charIn=getch();
      switch(charIn){
      case ‘a’:
         output_high(PIN_A0);
         break;
      case ‘w’:
         output_low(PIN_A0);
         break;
      }
   }
}
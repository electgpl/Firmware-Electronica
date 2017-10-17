#include <16F883.h>
#use delay(clock=20000000)
#use rs232(baud=9600,parity=N,xmit=PIN_C6,rcv=PIN_C7,bits=8)
#include <lcd420.c>
void main(){
   lcd_init();
   char varInt;
   int8 contChar=0, contReng=1;
   while(TRUE){
      if(kbhit()){
         varInt=getch();        
         contChar++;
      }
      if(contChar>20){
         contChar=1;
         contReng++;
      } 
      if(contReng>4)
         contReng=1;
      lcd_gotoxy(contChar,contReng);   
      printf(lcd_putc,"%c",varInt);
   }
}
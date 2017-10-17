#include <16F676.h>
#FUSES NOWDT
#FUSES INTRC_IO
#FUSES NOPROTECT
#FUSES NOBROWNOUT
#FUSES NOMCLR
#FUSES NOCPD
#FUSES NOPUT
#use delay(int=4000000)
#use rs232(baud=1200,parity=N,xmit=PIN_C4,rcv=PIN_C5,bits=8)
//Manchester IEEE 802.3
void main(){
   char data[2];
   int8 i=0, armado=0;
   while(true){
      if(!kbhit()){
         data[i++]=getchar();
      } 
      if(i>1){
         i=0;
         if(bit_test(data[1],7)==1&&bit_test(data[1],6)==0) bit_clear(armado,7);
         if(bit_test(data[1],7)==0&&bit_test(data[1],6)==1) bit_set(armado,7);
         if(bit_test(data[1],5)==1&&bit_test(data[1],4)==0) bit_clear(armado,6);
         if(bit_test(data[1],5)==0&&bit_test(data[1],4)==1) bit_set(armado,6);
         if(bit_test(data[1],3)==1&&bit_test(data[1],2)==0) bit_clear(armado,5);
         if(bit_test(data[1],3)==0&&bit_test(data[1],2)==1) bit_set(armado,5);
         if(bit_test(data[1],1)==1&&bit_test(data[1],0)==0) bit_clear(armado,4);
         if(bit_test(data[1],1)==0&&bit_test(data[1],0)==1) bit_set(armado,4);
         if(bit_test(data[0],7)==1&&bit_test(data[0],6)==0) bit_clear(armado,3);
         if(bit_test(data[0],7)==0&&bit_test(data[0],6)==1) bit_set(armado,3);
         if(bit_test(data[0],5)==1&&bit_test(data[0],4)==0) bit_clear(armado,2);
         if(bit_test(data[0],5)==0&&bit_test(data[0],4)==1) bit_set(armado,2);
         if(bit_test(data[0],3)==1&&bit_test(data[0],2)==0) bit_clear(armado,1);
         if(bit_test(data[0],3)==0&&bit_test(data[0],2)==1) bit_set(armado,1);
         if(bit_test(data[0],1)==1&&bit_test(data[0],0)==0) bit_clear(armado,0);
         if(bit_test(data[0],1)==0&&bit_test(data[0],0)==1) bit_set(armado,0);
         printf("valor: %x \n\r", armado);
      }
   }
}
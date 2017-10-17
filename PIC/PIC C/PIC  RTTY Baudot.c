#include <16F84A.h>
#fuses XT
#use delay(clock=4000000)
#define AUDIO PIN_A2
freq1(){        //625us + 625us = 1250us = 800Hz
   int q=0;
   while(q<17){ //22ms / 1250us = 17 veces.
      output_low(AUDIO);
      delay_us(625);
      output_high(AUDIO);
      delay_us(625);
      q++;
   }  
}
freq0(){        //515us + 515us = 1030us = 970Hz
   int p=0;
   while(p<21){ //22ms / 1030us = 21 veces.
      output_low(AUDIO);
      delay_us(515);
      output_high(AUDIO); 
      delay_us(515); 
      p++;
   }  
}
void main(){
   int i,j;     
   byte valor;
   byte const bitMask[8]={1,2,4,8,16,32,64,128};
   byte const ByteDatosSalida[18]={
      0b11001000, //11 00100 0, STOP STOP ESPACIO START
      0b11001000, //11 00100 0, STOP STOP ESPACIO START
      0b11000010, //11 00001 0, STOP STOP "E" START
      0b11100100, //11 10010 0, STOP STOP "L" START
      0b11000010, //11 00001 0, STOP STOP "E" START
      0b11011100, //11 01110 0, STOP STOP "C" START
      0b11100000, //11 10000 0, STOP STOP "T" START
      0b11110100, //11 11010 0, STOP STOP "G" START
      0b11101100, //11 10110 0, STOP STOP "P" START
      0b11100100, //11 10010 0, STOP STOP "L" START
      0b11001000, //11 00100 0, STOP STOP ESPACIO START
      0b11001000, //11 00100 0, STOP STOP ESPACIO START
      0b11010100, //11 01010 0, STOP STOP "R" START
      0b11100000, //11 10000 0, STOP STOP "T" START
      0b11100000, //11 10000 0, STOP STOP "T" START
      0b11101010, //11 10101 0, STOP STOP "Y" START
      0b11001000, //11 00100 0, STOP STOP ESPACIO START
      0b11001000};//11 00100 0, STOP STOP ESPACIO START
   while(true){
      delay_ms(3000);              //delay de repeticion 3s
      i=0;
      while(i<18){                 //cantidad de caracteres 18
         valor=ByteDatosSalida[i]; //carga valor con dato de salida
         for(j=0;j<8;j++){         //recorre los 8 bit del baudot
            if(valor&bitMask[j])   //aplica la mascara
               freq0();            //si es verdadero envia 970Hz
            else
               freq1();            //si es falso envia 800Hz
         }i++; 
      }
   }
}
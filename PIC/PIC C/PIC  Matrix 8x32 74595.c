#include <16F628A.h>
#FUSES NOWDT, HS, NOPUT, MCLR
#use delay(clock=20000000)
#define LARGO  56
#define DATA   PIN_A0
#define CLOCK  PIN_A1
#define LATCH  PIN_A2
void f74595_init(){
   output_bit(DATA,1);
   output_bit(CLOCK,1);
   output_bit(DATA,0);
}
void f74595_clock(){
   output_bit(CLOCK,1);
   output_bit(CLOCK,0);
   output_bit(LATCH,1);
   output_bit(LATCH,0);
}
int const tabla[LARGO+64]={
0,0,0,0,0,0,0,0,  //8 espacios
0,0,0,0,0,0,0,0,  //8 espacios
0,0,0,0,0,0,0,0,  //8 espacios
0,0,0,0,0,0,0,0,  //8 espacios
255,137,129,0,    //Letra E
255,128,128,0,    //Letra L
255,137,129,0,    //Letra E
126,129,129,0,    //Letra C
1,255,1,0,        //Letra T
126,129,241,0,    //Letra G
254,17,14,0,      //Letra P
255,128,128,0,    //Letra L
0,0,              //2 espacios
255,6,255,0,      //Letra M
254,17,254,0      //Letra A
1,255,1,0,        //Letra T
255,25,230,0,     //Letra R
255,0,            //Letra I
231,24,231,0,     //Letra X
0,0,0,0,0,0,0,0,  //8 espacios
0,0,0,0,0,0,0,0,  //8 espacios
0,0,0,0,0,0,0,0,  //8 espacios
0,0,0,0,0,0,0,0   //8 espacios
};
void main(){
   int i,j,k;                          //Variables contadores
   while(TRUE){                        //Ciclo infinito
      for(i=0;i<LARGO+32;i++){         //Recorrido de tabla
         for(k=0;k<24;k++){            //Refresco de matriz
            f74595_init();             //Inicializa 74595
            for(j=0;j<32;j++){         //Barrido de Columnas
               f74595_clock();         //desplaza las columnas
               output_b(~tabla[j+i]);  //Desplazamiento de tabla
               delay_us(20);           //Demora de multiplexado
            }
         }
      }
   }
}
#include <16F883.h>
#use delay(clock=4000000)
#use rs232(baud=9600, xmit=pin_c6, rcv=pin_c7, bits=8, parity=N)
#define entradaA input(PIN_A0)                  //Macro de entrada A
#define entradaB input(PIN_A1)                  //Macro de entrada B
unsigned int f_encoder(){			            //Funcion de llamada al encoder
   int entradaAnterior=0;                       //Variable de intercambio
   unsigned int encoder=0;					    //Variable sin signo de encoder
   if((entradaAnterior==0)&&(entradaA==1)){     //Si Anterior 0 y A 1
      if(entradaB==0)                           //Si B esta activo
         encoder--;                             //Decremento el valor de encoder
      else                                      //Caso contrario
         encoder++;                             //Incrementa encoder
      return(encoder);							//Retorna valor
   }
   entradaAnterior=entradaA;                    //Intercambio de Variables	
}
void main(){                                    //Función principal
   while(true){                                 //Bucle infinito
      printf("SALIDA %3u\r",f_encoder());       //Muestra el valor de encoder
   }
}
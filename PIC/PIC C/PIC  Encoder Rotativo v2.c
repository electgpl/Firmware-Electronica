#include <16F883.h>
#use delay(clock=4000000)
#use rs232(baud=9600, xmit=pin_c6, rcv=pin_c7, bits=8, parity=N)
#define entradaA input(PIN_A0)					//Macro de entrada A
#define entradaB input(PIN_A1)					//Macro de entrada B
unsigned int leeEncoder(){						//Función del encoder
   int auxiliar=0, entradaAnterior=0;			//Variables de trabajo
   auxiliar=entradaA;							//Mueve A a auxiliar
   if((entradaAnterior==0)&&(auxiliar==1)){		//Si Anterior 0 y Auxiliar 1
      if(entradaB==0)							//Si B esta activo
         encoder--;								//Decremento el valor de encoder
      else										//Caso contrario
         encoder++;								//Incrementa encoder
   }
   entradaAnterior=auxiliar;					//Intercambio de Variables
}
void main(){									//Función principal
   while(true){									//Bucle infinito
      printf("SALIDA %3u\r",leeEncoder());	    //Muestra el valor de encoder
   }
}
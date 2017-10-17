#include <16F84.h>				//Microcontrolador		
#use delay(clock=4000000)		//Cristal de 4MHz
int const palabra[39]={			//Vector de Frase
0b00000001,						//Espacio
0b00000001,						//Espacio
0b00000001,						//Espacio
0b00000001,						//Espacio
0b00000001,						//Espacio
0b00000001,						//Espacio
0b00000001,						//Espacio
0b00000001,						//Espacio
0b00000011,						//Letra L	
0b00000011,						//Letra L	
0b11111111,						//Letra L	
0b00000001,						//Espacio
0b11110001,						//Letra P	
0b10010001,						//Letra P	
0b11111111,						//Letra P	
0b00000001,						//Espacio
0b10011111,						//Letra G	
0b10000011,						//Letra G	
0b11111111,						//Letra G	
0b00000001,						//Espacio
0b10000001,						//Letra T	
0b11111111,						//Letra T	
0b10000001,						//Letra T	
0b00000001,						//Espacio
0b10000011,						//Letra C
0b10000011,						//Letra C
0b11111111,						//Letra C
0b00000001,						//Espacio
0b10000011,						//Letra E
0b10010011,						//Letra E
0b11111111,						//Letra E
0b00000001,						//Espacio
0b00000011,						//Letra L
0b00000011,						//Letra L
0b11111111,						//Letra L
0b00000001,						//Espacio
0b10000011,						//Letra E
0b10010011,						//Letra E
0b11111111};				  	//Letra E
void main(){   					//Función Principal
   int i;     					//Variable de incremento de secuencia 
   while(TRUE){             	//Función Repetitiva
      for(i=0;i<39;i++){   		//Lazo de iteración para generar secuencia
         output_b(palabra[i]);	//Se mueve al puerto B el valor del vector según indice
		 delay_ms(1);			//Delay de 1ms entre cada valor del vector
      } 
   }
}
#include <12F675.h>
#FUSES NOWDT
#FUSES INTRC_IO
#FUSES NOCPD
#FUSES NOPROTECT
#FUSES NOMCLR
#FUSES NOPUT
#FUSES NOBROWNOUT
#use delay(int=4000000)
#define DATO PIN_A1
#define CLK  PIN_A0
void main(){
   int i,j;      
   byte valor;										
   byte const bitMask[8]={1,2,4,8,16,32,64,128}; 	//Variables para mascara   
   byte const ByteDatosSalida[3]={0b11110010, 		//Datos a mostrar en cada 74164   
                                  0b11011010,    
                                  0b01100000}; 
   while(true){										
      while(i<3){ 									//Ciclo de recorrido de los tres 74164
         valor=ByteDatosSalida[i];  				//Intercambio de variables
         for(j=0;j<8;j++){ 							//Rrecorrido de cada bit del 74164
            output_low(CLK);						//Clock a nivel bajo  
            output_bit(DATO,valor&bitMask[j]); 		//Salida de dato para 74164 con aplicación de mascara "&" 
            delay_ms(30);  							//Demora de carga de datos
            output_high(CLK); 						//Clock a nivel alto
         }i++;   									//Incremento del contador del while 
      }
   }
}
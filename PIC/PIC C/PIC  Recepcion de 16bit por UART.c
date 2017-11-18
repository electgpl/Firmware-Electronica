//*********************************************************************************************************
// El programa recibe 16bit por UART y los muestra por puertos
// El enlace se realiza mediante una comunicación ASK OOK UHF sobre UART Invertido a 600bps
//*********************************************************************************************************
#include <16F883.h>                                //Biblioteca del microcontrolador
#fuses NOMCLR, NOWDT, INTRC_IO                     //Configuración de fuses
#use delay(int=4000000)                            //Configuración del clock interno a 4MHz
#use rs232(baud=600,parity=N,xmit=PIN_C6,rcv=PIN_C7,bits=8) //Configuración de UART 600bps 8 N 1
//*********************************************************************************************************
// Variables
//*********************************************************************************************************
#define HEADER 200                                 //Definición de valor Header para el payload
#define STATUS PIN_C0                              //Pin para analizar el status de la interrupt
static int8 trama[4];                              //Variable donde se aloja el trama
static int8 valor1=0, valor2=0;                    //Variable donde se alojan los datos externos
//*********************************************************************************************************
// Función Interrupción UART que realiza el parse de datos, validación de Header y Checksum
// Si validación y Checksum son validos, carga el vector DATO para ser utilizado
//*********************************************************************************************************
int8 i=0;                                          //Variable para el contador de bytes de entrada
#int_RDA                                           //Interrupción por dato en UART
void intser(){                                     //Función de servicio de interrupción
   trama[i++]=getc();                              //Guarda byte de entrada en trama. incrementa indice
   if(i>4){                                        //Si el indice es mayor que 4 se asume que se completa el trama
      if(trama[0]==HEADER){                        //Validación que el Header sea 200 (seteado en el transmisor)
         if(trama[1]+trama[2]-trama[3]==0){        //Validación de checksum, si datos leídos son igual a checksum
            valor1=trama[1];                       //Cargamos los datos en el vector
            valor2=trama[2];                       //Cargamos los datos en el vector
            output_high(STATUS);                   //Ponemos a 1 el LED de estado de dato presente
            delay_ms(100);                         //Delay de LED encendido 
            output_low(STATUS);                    //Ponemos a 0 el LED
         }
      }
      i=0;                                         //Una vez que se completan los 5 bytes, se reinicia el contador
   }
}
//*********************************************************************************************************
// Programa principal, recibe los datos por UART y los muestra en los puertos
//*********************************************************************************************************
void main(){                                       //Función principal
   enable_interrupts(global);                      //Habilita interrupciones globales
   enable_interrupts(int_rda);                     //Habilita interrupción de dato en UART
   while(true){                                    //Loop principal infinito
      output_a(valor1);                            //Cargamos puerto A 
      delay_us(100);                               //Delay de refresco
      output_b(valor2);                            //Cargamos puerto B
      delay_us(100);                               //Delay de refresco
   }
}

//*********************************************************************************************************
// Programa para enviar 16 estados por UART con CRC
// El enlace se realiza mediante una comunicación ASK OOK UHF sobre UART Invertido a 600bps
//*********************************************************************************************************
#include <16F883.h>                                  //Biblioteca del microcontrolador
#fuses NOMCLR, NOWDT, INTRC_IO                       //Configuración de fuses
#use delay(int=4000000)                              //Configuración del clock interno a 4MHz
#use rs232(baud=600,parity=N,xmit=PIN_C6,rcv=PIN_C7,bits=8) //Configuración de UART 600bps 8 N 1
//*********************************************************************************************************
// Función que realiza la lectura de puertos
//*********************************************************************************************************
int8 valor1=0, valor2=0, valor1Ant=0, valor2Ant=0;   //Declaracion de variables de 8bit
int1 cambio=FALSE;                                   //Declaracion de variable boole
int1 leePuertos(void){                               //Declaración de función para enviar datos
   valor1=input_a();                                 //Lectura de puerto A
   delay_ms(50);                                     //Delay de 50ms
   valor2=input_b();                                 //Lectura de puerto B   
   delay_ms(50);                                     //Delay de 50ms
   if(valor1!=valor1Ant || valor2!=valor2Ant)        //Si hay cambio en entrada se actualiza
      cambio=TRUE;                                   //Setea flag a TRUE
   else                                              //Si no hay cambio en la entrada no actualiza
      cambio=FALSE;                                  //Setea flag a FALSE
   valor1Ant=valor1;                                 //Intercambio de valores
   valor2Ant=valor2;                                 //Intercambio de valores
   return(cambio);                                   //Retorno de funcion
}
//*********************************************************************************************************
// Función que realiza el envio de datos por UART
//*********************************************************************************************************
#define HEADER 200                                   //Definición de valor Header para el payload
static char trama[4];                                //Variable donde se aloja el trama
void enivaRF(void){                                  //Declaración de función para enviar datos
   trama[0]=HEADER;                                  //Carga el Header en byte 0 del trama
   trama[3]=trama[1]+trama[2];                       //Realiza suma de los dos datos y lo carga en el byte 3 del trama
   putc(trama[0]);                                   //Envía el byte 0 del trama por UART
   delay_ms(50);                                     //Delay de espera entre bytes enviados por UART
   putc(trama[1]);                                   //Envía el byte 1 del trama por UART
   delay_ms(50);                                     //Delay de espera entre bytes enviados por UART
   putc(trama[2]);                                   //Envía el byte 2 del trama por UART
   delay_ms(50);                                     //Delay de espera entre bytes enviados por UART
   putc(trama[3]);                                   //Envía el byte 3 del trama por UART
   delay_ms(50);                                     //Delay de espera entre bytes enviados por UART
   printf("\r");                                     //Envía caracter de retorno de linea como final de trama
}
//*********************************************************************************************************
// Programa principal, Realiza el envio de datos por UART
//*********************************************************************************************************
void main(){                                         //Función principal
   while(true){                                      //Loop principal repetitivo
      if(leePuertos()){                              //Funcion que lee los puertos para enviar datos
         trama[1]=valor1;                            //Carga el valor 1 en byte 1 del trama
         trama[2]=valor2;                            //Carga el valor 2 en byte 2 del trama
         enivaRF();                                  //Llamado a la función que envía datos
      }
   }
}

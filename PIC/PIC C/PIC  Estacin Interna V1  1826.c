//*********************************************************************************************************
// Programa para la estación meteorológica Interna inalámbrica
// El programa recibe Temperatura, Humedad y Luminosidad por interrupción
// El sistema mide Temperatura y Humedad interior
// El enlace se realiza mediante una comunicación ASK OOK UHF sobre UART Invertido a 600bps
// Se muestra de en un LCD de 2x16 medición interna y externa
//*********************************************************************************************************
#include <16F1826.h>                                           //Biblioteca del microcontrolador
#fuses NOMCLR, NOWDT, INTRC_IO                                 //Configuración de fuses
#use delay(int=4000000)                                        //Configuración del clock interno a 4MHz
#use rs232(baud=600,parity=N,xmit=PIN_B5,rcv=PIN_B1,bits=8)    //Configuración de UART 600bps 8 N 1
#define LCD_ENABLE_PIN  PIN_A7                                 //Redefine pin de LCD
#define LCD_RS_PIN      PIN_A4                                 //Redefine pin de LCD
#define LCD_RW_PIN      PIN_A6                                 //Redefine pin de LCD
#define LCD_DATA4       PIN_B4                                 //Redefine pin de LCD
#define LCD_DATA5       PIN_B5                                 //Redefine pin de LCD
#define LCD_DATA6       PIN_B6                                 //Redefine pin de LCD
#define LCD_DATA7       PIN_B7                                 //Redefine pin de LCD
#include <LCD.c>                                               //bilioteca del LCD 2x16
//*********************************************************************************************************
// Variables
//*********************************************************************************************************
#define HEADER 200                                             //Definición de valor Header para el payload
#define DATA PIN_A3                                            //Pin del bus de un hilo para el DHT11
static int8 datoInt[2];                                        //Variable donde se alojan los datos internos
static int8 payload[5];                                        //Variable donde se aloja el payload
static int8 datoExt[3];                                        //Variable donde se alojan los datos externos
//*********************************************************************************************************
// Función Interrupción UART que realiza el parse de datos, validación de Header y Checksum
// Si validación y Checksum son validos, carga el vector DATO para ser utilizado
//*********************************************************************************************************
int8 i=0;                                                      //Variable para el contador de bytes de entrada
#int_RDA                                                       //Interrupción por dato en UART
void intser(){                                                 //Función de servicio de interrupción
   payload[i++]=getc();                                        //Guarda byte de entrada en payload. incrementa indice
   if(i>5){                                                    //Si el indice es mayor que 5 se asume que se completa el payload
      if(payload[0]==HEADER){                                  //Validación que el Header sea 200 (seteado en el transmisor)
         if(payload[1]+payload[2]+payload[3]-payload[4]==0){   //Validación de checksum, si datos leídos son igual a checksum
            datoExt[0]=payload[1];                             //Cargamos los datos en el vector
            datoExt[1]=payload[2];                             //Cargamos los datos en el vector
            datoExt[2]=payload[3];                             //Cargamos los datos en el vector
         }
      }
      i=0;                                                     //Una vez que se completan los 5 bytes, se reinicia el contador
   }
}
//*********************************************************************************************************
// Trama DHT11 - Segun Datasheet
// ____             ____      ____                                       ___
//     |___________|    |____|    |.....................................|
//     |   18ms    |    |80us|80us|               5x8 bit               |
//     |    PIC    |    |                  DHT11 respuesta              | Fin de
//     |  request  |    |  2x80us, intRH, decRH, intT, decT, checksum   | Trama
// ____          ________
//     |________|        |...                        0 bit
//      <-50us-> <-27us->
// ____          _________________________
//     |________|                         |...       1 bit
//      <-50us-> <---------70us---------->
//*********************************************************************************************************
unsigned int trama[5];                                         //Vector donde se alojan los datos
//*********************************************************************************************************
// Funcion de recepcio de Byte
// Lee el valor leído en la trama y lo separa realizando shift
// Retorna el valor en forma de byte, es utilizado en la función recibeDato()
//*********************************************************************************************************
unsigned int recibeByte(){                                     //Función que recibe un Byte
   unsigned int8 valorLeido = 0;                               //Valor de retorno de la función
   int8 i=0;                                                   //Inicialización del indice
   for(i=0; i<8; i++){                                         //Iteración para recepción de bits
      valorLeido <<= 1;                                        //Registro de desplazamiento de bits
      while(!input(DATA));                                     //Espera a DATA = 0
      delay_us(30);                                            //Demora de 30us (Del Datasheet)
      if(input(DATA)){                                         //Pregunta si DATA = 1
          valorLeido |= 1;                                     //Realiza toggle del valor leido
      }
      while(input(DATA));                                      //Espera a DATA = 1
   }
   return valorLeido;                                          //Retorna el valor leido
}
//*********************************************************************************************************
// Función de recepción de dato para el DHT11
// Recibe los valores de temperatura y humedad (parte entera y decimales por separado)
// Recibe el checksum enviado por el DHT11 y lo compara con el leído en el programa
//*********************************************************************************************************
unsigned int recibeDato(){                                     //Función que recibe el Dato
   int validacion = 0;                                         //Variable de Validación
   int checksum = 0;                                           //Variable de deteccion de cambios de secuencia
   int8 j=0;                                                   //Variable para el lazo for
   output_high(DATA);                                          //Set DATA = 1  
   output_low(DATA);                                           //Set DATA = 0
   delay_ms(18);                                               //Demora de 18ms (Del Datasheet)
   output_high(DATA);                                          //Set DATA = 1
   delay_us(25);                                               //Demora de 25ms (Del Datasheet)
   validacion = input(DATA);                                   //Mueve valor de DATA a Validacion
   delay_us(80);                                               //Espera 80us (Del Datasheet)
   validacion = input(DATA);                                   //Mueve valor de DATA a Validacion
   if(!validacion){                                            //Si Validación = 0, Error de secuencia
      printf( "Error en Checksum \r");                         //Muestra leyenda de error
   }
   delay_us(80);                                               //Espera 80us (Del Datasheet)
   for(j=0; j<5; j++){                                         //Lazo de carga de bytes de datos
       trama[j] = recibeByte();                                //Carga del vector de datos
   }
   output_high(DATA);                                          //Set DATA = 1
   for(j=0; j<4; j++){                                         //Lazo de carga de bytes de verificacion
       checksum += trama[j];                                   //Carga de bytes de verificación
   }
   if(checksum == trama[4]){                                   //Si la secuencia de verificación es correcta
      return 0;                                                //Se retorna 0 y se realiza la lectura
   }
}
//*********************************************************************************************************
// Lee DHT11
//*********************************************************************************************************
void leeDHT11(void){                                           //Función que lee el DHT11
   if(recibeDato()==0){                                        //Consulta si hay dato en la entrada del DHT11
      datoInt[0]=trama[2];                                     //Carga el valor de temperatura DHT en byte 1 del payload
      datoInt[1]=trama[0];                                     //Carga el valor de humedad DHT en byte 2 del payload
   }
}
//*********************************************************************************************************
// Impresion de Pantalla
//*********************************************************************************************************
void impPantalla(void){                                        //Función que imprime pantalla
   lcd_gotoxy(1,2);                                            //Posiciona el cursor en la pantalla
   printf(lcd_putc,"%2u",datoExt[0]);                          //Imprime Temperatura Exterior
   lcd_gotoxy(3,2);                                            //Posiciona el cursor en la pantalla
   lcd_putc(0xDF);                                             //Imprime unidad de medida
   lcd_gotoxy(4,2);                                            //Posiciona el cursor en la pantalla
   lcd_putc("C");                                              //Imprime unidad de medida
   lcd_gotoxy(6,2);                                            //Posiciona el cursor en la pantalla
   printf(lcd_putc,"%2u",datoExt[1]);                          //Imprime Humedad Exterior
   lcd_gotoxy(8,2);                                            //Posiciona el cursor en la pantalla
   lcd_putc("%RH");                                            //Imprime unidad de medida
   lcd_gotoxy(12,2);                                           //Posiciona el cursor en la pantalla
   printf(lcd_putc,"%3u",datoExt[2]);                          //Imprime Luminosidad Exterior
   lcd_gotoxy(15,2);                                           //Posiciona el cursor en la pantalla
   lcd_putc("%L");                                             //Imprime unidad de medida
   lcd_gotoxy(3,1);                                            //Posiciona el cursor en la pantalla
   printf(lcd_putc,"%2u",datoInt[0]);                          //Imprime Temperatura Interior
   lcd_gotoxy(5,1);                                            //Posiciona el cursor en la pantalla
   lcd_putc(0xDF);                                             //Imprime unidad de medida
   lcd_gotoxy(6,1);                                            //Posiciona el cursor en la pantalla
   lcd_putc("C");                                              //Imprime unidad de medida
   lcd_gotoxy(10,1);                                           //Posiciona el cursor en la pantalla
   printf(lcd_putc,"%2u",datoInt[1]);                          //Imprime Humedad Exterior
   lcd_gotoxy(12,1);                                           //Posiciona el cursor en la pantalla
   lcd_putc("%RH");                                            //Imprime unidad de medida
}
//*********************************************************************************************************
// Programa principal, Realiza la lectura de DHT11, lectura de batería y recibe los datos por UART
// Lee la temperatura y humedad interna, realiza un pronóstico aproximado
//*********************************************************************************************************
void main(){                                                   //Función principal
   enable_interrupts(global);                                  //Habilita interrupciones globales
   enable_interrupts(int_rda);                                 //Habilita interrupción de dato en UART
   lcd_init();                                                 //Inicializa LCD
   while(true){                                                //Loop principal infinito
      leeDHT11();                                              //Función que lee DHT11
      delay_ms(2000);                                          //Delay de Actualizacion
      impPantalla();                                           //Imprime pantalla LCD 
      delay_ms(2000);                                          //Delay de Actualizacion
   }
}
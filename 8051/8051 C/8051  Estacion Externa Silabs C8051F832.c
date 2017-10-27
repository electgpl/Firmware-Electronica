//*********************************************************************************************************
// Programa para la estacion meteorologica Interna inalámbrica
// El programa recibe Temperatura, Humedad y Luminosidad por interrupcion
// El sistema mide Temperatura y Humedad interior
// El enlace se realiza mediante una comunicacion ASK OOK UHF sobre UART Invertido a 600bps
// Se muestra medicion interna y externa en un LCD de 2x16
// Microcontrolador Silabs C8051F832
//*********************************************************************************************************
#include <reg52.h>
#include <REG51F800.H>
#include <intrins.h>
#include <stdio.h>
//*********************************************************************************************************
// Variables
//*********************************************************************************************************
#define HEADER 200                                           //Definicion de valor Header para el payload
sbit DATA = P1^0;                                            //Pin del bus de un hilo para el DHT11
static int datoInt[2];                                       //Variable donde se alojan los datos internos
static int payload[5];                                       //Variable donde se aloja el payload
static int datoExt[3];                                       //Variable donde se alojan los datos externos
sbit P0MDIN = 0xF1;                                          //Port 0 Input Mode
sbit P0SKIP = 0xD4;                                          //Port 0 Skip
sbit ADC0CF = 0xBC;                                          //ADC0 Configuration
sbit ADC0CN = 0xE8;                                          //ADC0 Control 0
sbit AD0BUSY = 0xF8;                                         //ADC0 Busy
sbit ADC0L = 0xBD;                                           //ADC0 Data Word Low Byte 
sbit ADC0H = 0xBE;                                           //ADC0 Data Word High Byte
//*********************************************************************************************************
// delay_us Bloqueante Micro Segundos
//*********************************************************************************************************
void delay_us(unsigned int us_count){                        //Funcion para delay_ms de micro-segundos
   int t=0;
   while(us_count!=0){                                       //Mientras que el contador es distinto de cero
      for(t=0;t<16;t++){                                     //16MIPS dividido en 16 para 1us
         _nop_();                                            //Ejecuta funcion NOP de ensamblador
      }
      us_count--;                                            //Decremento del valor de delay_ms
   }
}
//*********************************************************************************************************
// delay_ms Bloqueante Mili Segundos
//*********************************************************************************************************
void delay_ms(unsigned int us_count){                        //Funcion para delay_ms de micro-segundos
   while(us_count!=0){                                       //Mientras que el contador es distinto de cero
      delay_us(1000);                                        //Ejecuta funcion delay_ms micro segundos
      us_count--;                                            //Decremento del valor de delay_ms
   }
}
//*********************************************************************************************************
// Configuracion ADC pin P0.0
//*********************************************************************************************************
void configuraADC(void){
   #define VREF 3                                            //Tensión de referencia interna
   P0MDIN=0xFE;                                              //Selecciona pin P0.0 como canal ADC 
   P0SKIP=0x01;                                              //Decodificacion Crossbar
   ADC0CF=0xF8;                                              //Habilita ADC 0
   ADC0CN=0x80;                                              //Habilita conversion en registro
}
//*********************************************************************************************************
// Funcion para inicializar el puerto serie 9600 @ 11.059MHz
//*********************************************************************************************************
void serialInit(void){                                       //Funcion para configurar UART
    TMOD=0x20;                                               //Timer 1 en modo 2 - Auto recarga para generar Baudrate
    SCON=0x50;                                               //Serial modo 1, 8bit de dato, 1bit de start, 1bit de stop
    TH1=0xFD;                                                //Carga el baudrate en el timer a 9600bps
    TR1=1;                                                   //Dispara el timer
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
unsigned int trama[5];                                       //Vector donde se alojan los datos
//*********************************************************************************************************
// Funcion de recepcio de Byte
// Lee el valor leido en la trama y lo separa realizando shift
// Retorna el valor en forma de byte, es utilizado en la funcion recibeDato()
//*********************************************************************************************************
unsigned int recibeByte(){                                   //Funcion que recibe un Byte
   unsigned int valorLeido = 0;                              //Valor de retorno de la funcion
   int i=0;                                                  //Inicializacion del indice
   for(i=0; i<8; i++){                                       //Iteracion para recepcion de bits
      valorLeido <<= 1;                                      //Registro de desplazamiento de bits
      while(DATA==0);                                        //Espera a DATA = 0
      delay_us(30);                                          //Demora de 30us (Del Datasheet)
      if(DATA==1){                                           //Pregunta si DATA = 1
          valorLeido |= 1;                                   //Realiza toggle del valor leido
      }
      while(DATA==1);                                        //Espera a DATA = 1
   }
   return valorLeido;                                        //Retorna el valor leido
}
//*********************************************************************************************************
// Funcion de recepcion de dato para el DHT11
// Recive los valores de temperatura y humedad (parte entera y decimales por separado)
// Recive el checksum enviado por el DHT11 y lo compara con el leido en el programa
//*********************************************************************************************************
unsigned int recibeDato(){                                   //Funcion que recibe el Dato
   int validacion = 0;                                       //Variable de Validacion
   int checksum = 0;                                         //Variable de deteccion de cambios de secuencia
   int j=0;                                                  //Variable para el lazo for
   DATA = 1;                                                 //Set DATA = 1  
   DATA = 0;                                                 //Set DATA = 0
   delay_ms(18);                                             //Demora de 18ms (Del Datasheet)
   DATA = 1;                                                 //Set DATA = 1
   delay_us(25);                                             //Demora de 25ms (Del Datasheet)
   validacion = DATA;                                        //Mueve valor de DATA a Validacion
   delay_us(80);                                             //Espera 80us (Del Datasheet)
   validacion = DATA;                                        //Mueve valor de DATA a Validacion
   if(!validacion){                                          //Si Validacion = 0, Error de secuencia
      printf( "Error en Checksum \r");                       //Muestra leyenda de error
   }
   delay_us(80);                                             //Espera 80us (Del Datasheet)
   for(j=0; j<5; j++){                                       //Lazo de carga de bytes de datos
       trama[j] = recibeByte();                              //Carga del vector de datos
   }
   DATA = 1;                                                 //Set DATA = 1
   for(j=0; j<4; j++){                                       //Lazo de carga de bytes de verificacion
       checksum += trama[j];                                 //Carga de bytes de verificacion
   }
   if(checksum == trama[4]){                                 //Si la secuencia de verificacion es correcta
      return 0;                                              //Se retorna 0 y se realiza la lectura
   }
}
//*********************************************************************************************************
// Lee DHT11
//*********************************************************************************************************
void leeDHT11(void){                                         //Funcion que lee el DHT11
   if(recibeDato()==0){                                      //Consulta si hay dato en la entrada del DHT11
      datoInt[0]=trama[2];                                   //Carga el valor de temperatura DHT en byte 1 del payload
      datoInt[1]=trama[0];                                   //Carga el valor de humedad DHT en byte 2 del payload
   }
}
//*********************************************************************************************************
// Funcion que realiza la lectura del sensor LDR y acondiciona el valor
//*********************************************************************************************************
int lecturaLDR(void){                                        //Funcion que realiza la lectura del ADC para el LDR
   unsigned int adval;                                       //Variable donde se guarda el valor del ADC
   AD0BUSY=1;                                                //Inicia conversion de ADC
   while(AD0BUSY);                                           //Espera que finalice la conversion
   adval=ADC0L;                                              //Lee el ADC de 0 a 511
   return(adval);                                            //Retorna el valor del ADC canal 0
} 
//*********************************************************************************************************
// Funcion para enviar byte por puerto serie
//*********************************************************************************************************
void sendByte(unsigned char serialdata){
   SBUF=serialdata;                                          //Carga el dato a enviar por uart
   while(TI==0);                                             //Espera a la transmicion completa
   TI=0;                                                     //Borra el flag de transmision
}
//*********************************************************************************************************
// Funcion que realiza el parse de datos y armado del payload para enviar por UART
//*********************************************************************************************************
void enivaRF(void){                                          //Declaración de función para enviar datos
   payload[0]=HEADER;                                        //Carga el Header en byte 0 del payload
   payload[4]=payload[1]+payload[2]+payload[3];              //Realiza suma de los tres datos y lo carga en el byte 4 del payload
   sendByte(payload[0]);                                     //Envía el byte 0 del payload por UART
   delay_ms(50);                                             //Delay de espera entre bytes enviados por UART
   sendByte(payload[1]);                                     //Envía el byte 1 del payload por UART
   delay_ms(50);                                             //Delay de espera entre bytes enviados por UART
   sendByte(payload[2]);                                     //Envía el byte 2 del payload por UART
   delay_ms(50);                                             //Delay de espera entre bytes enviados por UART
   sendByte(payload[3]);                                     //Envía el byte 3 del payload por UART
   delay_ms(50);                                             //Delay de espera entre bytes enviados por UART
   sendByte(payload[4]);                                     //Envía el byte 4 del payload por UART
   delay_ms(50);                                             //Delay de espera entre bytes enviados por UART
   printf("\r");                                             //Envía caracter de retorno de linea como final de payload
}
//*********************************************************************************************************
// Programa principal, Realiza la lectura de DHT11, lectura de bateria y recibe los datos por UART
// Lee la temperatura y humedad interna, realiza un pronóstico aproximado
//*********************************************************************************************************
void main(){                                                 //Funcion principal
   serialInit();    
   configuraADC();                                           //Función que configura ADC
   while(1){                                                 //Loop principal infinito
      if(recibeDato()==0){                                   //Consulta si hay dato en la entrada del DHT11
         payload[1]=trama[2];                                //Carga el valor de temperatura DHT en byte 1 del payload
         payload[2]=trama[0];                                //Carga el valor de humedad DHT en byte 2 del payload
         payload[3]=lecturaLDR();                            //Carga el valor de luminosidad en el byte 3 del payload
         enivaRF();                                          //Llamado a la función que envía datos
      }      
      delay_ms(2304);                                        //Delay del timer de sleep*
   }
}

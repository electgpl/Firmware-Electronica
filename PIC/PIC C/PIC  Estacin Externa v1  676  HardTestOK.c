//*********************************************************************************************************
// Programa para la estación meteorológica externa inalámbrica
// El programa envía Temperatura, Humedad y Luminosidad cada 30 segundos
// El sistema entra en bajo consumo cuando mientras no realiza mediciones
// El enlace se realiza mediante una comunicación ASK OOK UHF sobre UART Invertido a 600bps
//********************************************************************************************************* 
#include <16F676.h>                                            //Biblioteca del microcontrolador
#fuses NOMCLR, NOWDT, INTRC_IO                                 //Configuración de fuses
#device adc=8                                                  //Configuración del ADC a 8bit
#use delay(int=4000000)                                        //Configuración del clock interno a 4MHz
#use rs232(baud=600,parity=N,xmit=PIN_C5,rcv=PIN_C4,bits=8,invert) //Configuración de UART 600bps 8 N 1
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
#define DATA PIN_A1                                            //Pin del bus de un hilo para el DHT11
unsigned int trama[5];                                         //Vector donde se alojan los datos
//*********************************************************************************************************
// Función de recepción de Byte
// Lee el valor leído en la trama y lo separa realizando shift
// Retorna el valor en forma de byte, es utilizado en la función recibeDato()
//*********************************************************************************************************
unsigned int recibeByte(){                                     //Función que recibe un Byte
   unsigned int8 valorLeido=0;                                 //Valor de retorno de la función
   int8 i=0;                                                   //Inicialización del indice
   int16 timeout=0;
   for(i=0;i<8;i++){                                           //Iteracion para recepción de bits
      valorLeido<<=1;                                          //Registro de desplazamiento de bits
      while(input(DATA)==0);                                   //Espera a DATA = 0
      delay_us(30);                                            //Demora de 30us (Del Datasheet)
      if(input(DATA))                                          //Pregunta si DATA = 1
         valorLeido|=1;                                        //Realiza toggle del valor leido
      timeout=0;
      while((input(DATA))&&(timeout++<1000));                  //Espera a DATA = 1 y timeout
   }
   return valorLeido;                                          //Retorna el valor leído
}
//*********************************************************************************************************
// Función de recepción de dato para el DHT11
// Recibe los valores de temperatura y humedad (parte entera y decimales por separado)
// Recibe el checksum enviado por el DHT11 y lo compara con el leído en el programa
//*********************************************************************************************************
unsigned int recibeDato(){                                     //Función que recibe el Dato
   int validacion=0;                                           //Variable de Validación
   int checksum=0;                                             //Variable de detección de cambios de secuencia
   int8 j=0;                                                   //Variable para el lazo for
   output_high(DATA);                                          //Set DATA = 1  
   output_low(DATA);                                           //Set DATA = 0
   delay_ms(18);                                               //Demora de 18ms (Del Datasheet)
   output_high(DATA);                                          //Set DATA = 1
   delay_us(25);                                               //Demora de 25ms (Del Datasheet)
   validacion=input(DATA);                                     //Mueve valor de DATA a Validación
   delay_us(80);                                               //Espera 80us (Del Datasheet)
   validacion=input(DATA);                                     //Mueve valor de DATA a Validación
   if(!validacion){                                            //Si Validación = 0, Error de secuencia
      printf("Error en Checksum \r");                          //Muestra leyenda de error
   }
   delay_us(80);                                               //Espera 80us (Del Datasheet)
   for(j=0;j<5;j++){                                           //Lazo de carga de bytes de datos
       trama[j]=recibeByte();                                  //Carga del vector de datos
   }
   output_high(DATA);                                          //Set DATA = 1
   for(j=0;j<4;j++){                                           //Lazo de carga de bytes de verificación
       checksum+=trama[j];                                     //Carga de bytes de verificación
   }
   if(checksum==trama[4]){                                     //Si la secuencia de verificación es correcta
      return 0;                                                //Se retorna 0 y se realiza la lectura
   }
}
//********************************************************************************************************* 
// Función que realiza la lectura del sensor LDR y acondiciona el valor
//*********************************************************************************************************
static int8 lecturaLDR(void){                                  //Función que realiza la lectura del ADC para el LDR
   set_adc_channel(0);                                         //Selecciona canal ADC 0 y comienza la medición
   delay_us(10);                                               //Delay propuesto por datasheet para conversión ADC
   return(read_adc());                                         //Retorna el valor del ADC canal 0
}
//*********************************************************************************************************
// Función que realiza el parse de datos y armado del payload para enviar por UART
//*********************************************************************************************************
#define HEADER 200                                             //Definición de valor Header para el payload
static char payload[5];                                        //Variable donde se aloja el payload
void enivaRF(void){                                            //Declaración de función para enviar datos
   payload[0]=HEADER;                                          //Carga el Header en byte 0 del payload
   payload[4]=payload[1]+payload[2]+payload[3];                //Realiza suma de los tres datos y lo carga en el byte 4 del payload
   putc(payload[0]);                                           //Envía el byte 0 del payload por UART
   delay_ms(50);                                               //Delay de espera entre bytes enviados por UART
   putc(payload[1]);                                           //Envía el byte 1 del payload por UART
   delay_ms(50);                                               //Delay de espera entre bytes enviados por UART
   putc(payload[2]);                                           //Envía el byte 2 del payload por UART
   delay_ms(50);                                               //Delay de espera entre bytes enviados por UART
   putc(payload[3]);                                           //Envía el byte 3 del payload por UART
   delay_ms(50);                                               //Delay de espera entre bytes enviados por UART
   putc(payload[4]);                                           //Envía el byte 4 del payload por UART
   delay_ms(50);                                               //Delay de espera entre bytes enviados por UART
   printf("\r");                                               //Envía caracter de retorno de linea como final de payload
}
//*********************************************************************************************************
// Programa principal, Realiza la lectura de DHT11, lectura del LDR y Envía los datos por UART 
// Realiza el timer de Sleep y WakeUp
//*********************************************************************************************************
void main(){                                                   //Función principal
   static int8 timer=0;                                        //Definición de variable para el timer
   setup_adc_ports(sAN0|VSS_VDD);                              //Configuración de canales ADC
   setup_adc(ADC_CLOCK_DIV_2);                                 //Configuración de velocidad de conversión de ADC
   while(true){                                                //Loop principal repetitivo
      if(timer>2){                                             //Cantidad de iteraciones del timer
         if(recibeDato()==0){                                  //Consulta si hay dato en la entrada del DHT11 
            payload[1]=trama[2];                               //Carga el valor de temperatura DHT en byte 1 del payload
            payload[2]=trama[0];                               //Carga el valor de humedad DHT en byte 2 del payload
            payload[3]=lecturaLDR();                           //Carga el valor de luminosidad en el byte 3 del payload
            enivaRF();                                         //Llamado a la función que envía datos
         }
         timer=0;                                              //Inicializa timer
      }else{                                                   //Mientras que el timer sea menor que limite
         delay_ms(2304);                                       //Delay del timer de sleep*
         timer++;                                              //Incrementa el timer
      }
   }
}
//
// Programa que realiza el envió de 3 canales ADC de 0 a 255, mediante puerto UART a 600bps
// El objetivo es realizar un enlace simplex mediante módulos ASK OOK UHF
// 
#include <16F883.h>												//Biblioteca del microcontrolador
#fuses NOMCLR, NOWDT, INTRC_IO									//Configuración de fuses
#device adc=8													//Configuración del ADC a 8bit
#use delay(int=4000000)											//Configuración del clock interno a 4MHz
#use rs232(baud=600,parity=N,xmit=PIN_C6,rcv=PIN_C7,bits=8)		//Configuración de UART 600bps 8 N 1
//
// Funcion que realiza el parse de datos y armado del payload para enviar por UART
//
#define HEADER 200												//Definición de valor Header para el payload
static char payload[5];											//Variable donde se aloja el payload
void enivaRF(void){												//Declaración de función para enviar datos
   payload[0]=HEADER;											//Carga el Header en byte 0 del payload
   set_adc_channel(0);											//Selecciona canal ADC 0 y comienza la medición
   delay_us(10);												//Delay propuesto por datasheet para conversión ADC
   payload[1]=read_adc();										//Carga el valor del ADC canal 0 en byte 1 del payload
   set_adc_channel(1);											//Selecciona canal ADC 1 y comienza la medicion
   delay_us(10);												//Delay propuesto por datasheet para conversión ADC
   payload[2]=read_adc();										//Carga el valor del ADC canal 1 en byte 2 del payload
   set_adc_channel(2);											//Selecciona canal ADC 2 y comienza la medición
   delay_us(10);												//Delay propuesto por datasheet para conversión ADC
   payload[3]=read_adc();										//Carga el valor del ADC canal 2 en byte 3 del payload
   payload[4]=payload[1]+payload[2]+payload[3];					//Realiza suma de los tres datos y lo carga en el byte 4 del payload
   putc(payload[0]);											//Envía el byte 0 del payload por UART
   delay_ms(50);												//Delay de espera entre bytes enviados por UART
   putc(payload[1]);											//Envía el byte 1 del payload por UART
   delay_ms(50);												//Delay de espera entre bytes enviados por UART
   putc(payload[2]);											//Envía el byte 2 del payload por UART
   delay_ms(50);												//Delay de espera entre bytes enviados por UART
   putc(payload[3]);											//Envía el byte 3 del payload por UART
   delay_ms(50);												//Delay de espera entre bytes enviados por UART
   putc(payload[4]);											//Envía el byte 4 del payload por UART
   delay_ms(50);												//Delay de espera entre bytes enviados por UART
   printf("\r");												//Envía carácter de retorno de linea como final de payload
}
//
//Programa principal, realiza el envió de los tres canales del ADC mediante UART por cada llamado a la función enviaRF()
//
void main(){													//Función principal
   setup_adc_ports(sAN0|sAN1|sAN2|VSS_VDD);						//Configuración de canales ADC
   setup_adc(ADC_CLOCK_DIV_2);									//Configuración de velocidad de conversión de ADC
   while(true){													//Loop principal repetitivo
      enivaRF();												//Llamado a la función que envía datos
      delay_ms(5000);											//Delay de 5s para cada envió de datos
   }
}
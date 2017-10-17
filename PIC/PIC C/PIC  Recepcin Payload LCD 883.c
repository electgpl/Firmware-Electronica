//
// Programa que realiza la recepcion de 3 canales ADC de 0 a 255, mediante puerto UART a 600bps
// El objetivo es realizar un enlace simplex mediante módulos ASK OOK UHF
//
#include <16F883.h>                                             //Biblioteca del microcontrolador
#fuses NOMCLR, NOWDT, INTRC_IO                                  //Configuración de fuses
#use delay(int=4000000)                                         //Configuración del clock interno a 4MHz
#use rs232(baud=600,parity=N,xmit=PIN_C6,rcv=PIN_C7,bits=8)     //Configuración de UART 600bps 8 N 1
#include <LCD.C>												//Biblioteca del display LCD
//
// Función Interrupción UART que realiza el parse de datos, validación de Header y Checksum
// Si validación y Checksum son validos, carga el vector DATO para ser utilizado
//
#define HEADER 200                                              //Definición de valor Header para el payload
static char payload[5];											//Variable donde se aloja el payload
static char dato[3];											//Variable donde se alojan los datos
int8 i=0;														//Variable para el contador de bytes de entrada
#int_RDA														//Interrupción por dato en UART
void intser(){													//Función de servicio de interrupción
   payload[i++]=getc();											//Guarda byte de entrada en payload. incrementa indice
   if(i>5){														//Si el indice es mayor que 5 se asume que se completa el payload
      if(payload[0]==HEADER){									//Validación que el Header sea 200 (seteado en el transmisor)
         if(payload[1]+payload[2]+payload[3]-payload[4]==0){	//Validación de checksum, si datos leídos son igual a checksum
            dato[0]=payload[1];									//Cargamos los datos en el vector
            dato[1]=payload[2];									//Cargamos los datos en el vector
            dato[2]=payload[3];									//Cargamos los datos en el vector
         }
      }
      i=0;														//Una vez que se completan los 5 bytes, se reinicia el contador
   }
   output_toggle(PIN_C0);										//Salida de monitoreo de ingreso a interrupción
}
//
//Programa principal, realiza la recepción de datos mediante interrupción y muestra los valores en el display LCD
//
void main(){													//Función principal
   enable_interrupts(global);									//Habilita interrupciones globales
   enable_interrupts(int_rda);									//Habilita interrupción de dato en UART
   lcd_init();													//Inicializa LCD
   lcd_gotoxy(1,1);												//Posiciona el cursor en la pantalla
   printf(lcd_putc,"START.");									//Muestra la leyenda START con 1 punto
   delay_ms(500);												//Delay para el efecto del punto en el LCD
   lcd_gotoxy(1,1);												//Posiciona el cursor en la pantalla
   printf(lcd_putc,"START..");									//Muestra la leyenda START con 2 puntos
   delay_ms(500);												//Delay para el efecto del punto en el LCD
   lcd_init();													//Inicializa LCD
   while(true){													//Loop principal infinito
      lcd_gotoxy(1,1);											//Posiciona el cursor en la pantalla
      printf(lcd_putc,"T:%3u H:%3u",dato[0],dato[1]);			//Formatea los datos y los muestra en el LCD
      lcd_gotoxy(1,2);											//Posiciona el cursor en la pantalla
      printf(lcd_putc,"L:%3u ",dato[2]);						//Formatea los datos y los muestra en el LCD
      delay_ms(1000);											//Delay de refresco de datos
   }
}
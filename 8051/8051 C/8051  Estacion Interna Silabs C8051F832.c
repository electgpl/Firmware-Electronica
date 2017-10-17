//*********************************************************************************************************
// Programa para la estacion meteorologica Interna inalámbrica
// El programa recibe Temperatura, Humedad y Luminosidad por interrupcion
// El sistema mide Temperatura y Humedad interior
// El enlace se realiza mediante una comunicacion ASK OOK UHF sobre UART Invertido a 600bps
// Se muestra medicion interna y externa en un LCD de 2x16
// Microcontrolador Silabs C8051F832
//*********************************************************************************************************
#include <REG51F800.H>                                       //Header de Silabs C8051F832
#include <intrins.h>                                         //Biblioteca para funciones de assembler para el Delay
#include <stdio.h>                                           //Biblioteca standar de entrada y salida
//*********************************************************************************************************
// Delay Bloqueante Micro Segundos
//*********************************************************************************************************
void delay_us(unsigned int us_count){                        //Funcion para delay de micro-segundos
     int t=0;
   while(us_count!=0){                                       //Mientras que el contador es distinto de cero
      for(t=0;t<16;t++){                                     //16MIPS dividido en 16 para 1us
         _nop_();                                            //Ejecuta funcion NOP de ensamblador
      }
      us_count--;                                            //Decremento del valor de delay
   }
}
//*********************************************************************************************************
// Delay Bloqueante Mili Segundos
//*********************************************************************************************************
void delay_ms(unsigned int us_count){                        //Funcion para delay de micro-segundos
   while(us_count!=0){                                       //Mientras que el contador es distinto de cero
      delay_us(1000);                                        //Ejecuta funcion delay micro segundos
      us_count--;                                            //Decremento del valor de delay
   }
}
//*********************************************************************************************************
// Variables
//*********************************************************************************************************
#define HEADER 200                                           //Definicion de valor Header para el payload
sbit DATA = P2^0;                                            //Pin del bus de un hilo para el DHT11
sbit STATUS = P1^0;                                          //Pin para analizar el status de la interrupt
static int datoInt[2];                                       //Variable donde se alojan los datos internos
static int payload[5];                                       //Variable donde se aloja el payload
static int datoExt[3];                                       //Variable donde se alojan los datos externos
//*********************************************************************************************************
// Configuracion de pines del LCD
//*********************************************************************************************************
sbit LCD_RS = P0^0;									         //Register select
sbit LCD_EN = P0^1;										     //Enable
sbit LCD_D4 = P0^2;										     //Data bits
sbit LCD_D5 = P0^3;											 //Data bits
sbit LCD_D6 = P0^4;											 //Data bits
sbit LCD_D7 = P0^6;											 //Data bits
//*********************************************************************************************************
// Funcion de escritura LCD
//*********************************************************************************************************
void lcd_write(unsigned char c){
   if(c & 0x80) LCD_D7 = 1; else LCD_D7 = 0;
   if(c & 0x40) LCD_D6 = 1; else LCD_D6 = 0;
   if(c & 0x20) LCD_D5 = 1; else LCD_D5 = 0;
   if(c & 0x10) LCD_D4 = 1; else LCD_D4 = 0;
   LCD_EN = 1;
   delay_us(1);
   LCD_EN = 0;
   delay_us(1);
   if(c & 0x08) LCD_D7 = 1; else LCD_D7 = 0;
   if(c & 0x04) LCD_D6 = 1; else LCD_D6 = 0;
   if(c & 0x02) LCD_D5 = 1; else LCD_D5 = 0;
   if(c & 0x01) LCD_D4 = 1; else LCD_D4 = 0;  
   LCD_EN = 1;
   delay_us(1);
   LCD_EN = 0;
   delay_us(40);
}
//*********************************************************************************************************
// Funcion que Limpia y enciende el LCD
//*********************************************************************************************************
void lcd_clear(void){
   LCD_RS = 0;                                               //Escribir bytes de control
   lcd_write(0x01);
   delay_ms(2);
}
//*********************************************************************************************************
// Funcion que escribe un caracter en pantalla
//*********************************************************************************************************
void lcd_putch(unsigned char c){
   LCD_RS = 1;                                               //Escribir bytes de control
   lcd_write(c);
}
//*********************************************************************************************************
// Escribir una cadena de caracteres en la pantalla LCD
//*********************************************************************************************************
void lcd_puts(char *s){
   LCD_RS = 1;                                               //Escribir bytes de control
   while(*s) 
      lcd_write(*s++);
}
//*********************************************************************************************************
// Funcion para ir a la posicion especifica
//*********************************************************************************************************
void lcd_goto(unsigned char pos,unsigned char line){
   LCD_RS = 0;                                               //Escribir bytes de control
   if(line==0)
      lcd_write(0x80 + pos);
   else
      lcd_write(0x80 + pos+ 0x40);
}
//*********************************************************************************************************
// Funcion que Inicializar la pantalla LCD en modo de 4 bits
//*********************************************************************************************************
void lcd_init(void){                                      
   LCD_RS = 0;                                               //Escribir bytes de control
   delay_ms(15);                                             //Retraso de encendido
   LCD_D4 = 1;                                               //Inicializacion segun datasheet    
   LCD_D5 = 1;                                               //Inicializacion segun datasheet  
   LCD_EN = 1;
   delay_us(1);
   LCD_EN = 0;
   delay_us(1);                                              //Habilitacion / Deshabilitacion del LCD a demanda
   delay_ms(5);                                              //Delay de configuracion de datasheet
   LCD_EN = 1;
   delay_us(1);
   LCD_EN = 0;
   delay_us(1);                                              //Habilitacion / Deshabilitacion del LCD a demanda   
   delay_us(100);                                            //Delay de configuracion de datasheet
   LCD_EN = 1;
   delay_us(1);
   LCD_EN = 0;
   delay_us(1);                                              //Habilitacion / Deshabilitacion del LCD a demanda  
   delay_ms(5);                                              //Delay de configuracion de datasheet
   LCD_D4 = 0;                                               //Establecer el modo de 4 bits
   LCD_EN = 1;
   delay_us(1);
   LCD_EN = 0;
   delay_us(1);                                              //Habilitacion / Deshabilitacion del LCD a demanda 
   delay_us(40);                                             //Delay de configuracion de datasheet
   lcd_write(0x28);                                          //Modo de 4 bits, 1/16 de servicio, fuente 5x8, 2 líneas
   lcd_write(0x0C);                                          //Enciende Display
   lcd_write(0x06);                                          //Cursor de avance del modo de entrada
   lcd_write(0x01);                                          //Borrar la visualizaci󮠹 restablecer el cursor
}
//*********************************************************************************************************
// Funcion ITOA para convertir entero en ascii
//*********************************************************************************************************
char *itoa(long int num, char *s){                           //Funcion ITOA (Entero to ASCII)
   unsigned long int temp=1;                                 //Declaración de valor temporal
   unsigned int i, cnt=0;                                    //Declaración de indices y contadores
   char c;                                                   //Declaración de variable de carácter de salida
   while(temp>0){                                            //Rutina de Conversión (Queda invertida)
      temp=(num/10);                                         //Conversión de carácter a carácter
      s[cnt]=(num%10)+'0';                                   //utilizando divisiones y resto
      if(s[cnt]>0x39)                                        //sumando el offset de la tabla ASCII
         s[cnt]+=0x7;
      cnt++;
      num=temp;
   }
   for(i=0;i<(int)(cnt/2);i++){                              //Rutina para invertir el numero convertido
      c=s[i];                                                //Intercambio de variables
      s[i]=s[cnt-i-1];
      s[cnt-i-1]=c;
   }
   s[cnt]='\0';                                              //Carácter nulo, fin de la conversión ITOA
   return s;                                                 //Retorno del valor ASCII
}
//*********************************************************************************************************
// Funcion Interrupcion UART que realiza el parse de datos, validacion de Header y Checksum
// Si validacion y Checksum son validos, carga el vector DATO para ser utilizado
//*********************************************************************************************************
int i=0;                                                     //Variable para el contador de bytes de entrada
void UART0_ISR(){                                            //Funcion de servicio de interrupcion
   payload[i++]=SBUF0;                                       //Guarda byte de entrada en payload. incrementa indice
   if(i>5){                                                  //Si el indice es mayor que 5 se asume que se completa el payload
      if(payload[0]==HEADER){                                //Validacion que el Header sea 200 (seteado en el transmisor)
         if(payload[1]+payload[2]+payload[3]-payload[4]==0){ //Validacion de checksum, si datos leidos son igual a checksum
            datoExt[0]=payload[1];                           //Cargamos los datos en el vector
            datoExt[1]=payload[2];                           //Cargamos los datos en el vector
            datoExt[2]=payload[3];                           //Cargamos los datos en el vector
            STATUS ^= 1;                                     //Status de dato recibido OK
         }
      }
      i=0;                                                   //Una vez que se completan los 5 bytes, se reinicia el contador
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
// Saludo de Pantalla
//*********************************************************************************************************
void saludoPantalla(void){                                   //Funcion que realiza un saludo inicial
   lcd_goto(1,1);                                            //Posiciona el cursor en la pantalla
   lcd_putch(" Est.Inalambrica");                            //Imprime mensaje renglon 1
   lcd_goto(1,2);                                            //Posiciona el cursor en la pantalla
   lcd_putch(" v1.1  UTN INSPT");                            //Imprime mensaje renglon 2
   delay_ms(2000);                                           //Delay de saludo
   lcd_init();                                               //Inicializa LCD
   lcd_clear();                                              //Borra LCD 
}
//*********************************************************************************************************
// Impresion de Pantalla
//*********************************************************************************************************
void impPantalla(void){                                      //Funcion que imprime pantalla
   char string[4];                                           //Declaración de vector para mostrar en LCD
   lcd_goto(1,2);                                            //Posiciona el cursor en la pantalla
   itoa(datoExt[0],string);                                  //Funcion que convierte entero en ascii
   lcd_puts(string);                                         //Muestra en el LCD
   lcd_goto(3,2);                                            //Posiciona el cursor en la pantalla
   lcd_putch(0xDF);                                          //Imprime unidad de medida
   lcd_goto(4,2);                                            //Posiciona el cursor en la pantalla
   lcd_putch("C");                                           //Imprime unidad de medida
   lcd_goto(6,2);                                            //Posiciona el cursor en la pantalla
   itoa(datoExt[1],string);                                  //Funcion que convierte entero en ascii
   lcd_puts(string);                                         //Muestra en el LCD
   lcd_goto(8,2);                                            //Posiciona el cursor en la pantalla
   lcd_putch("%RH");                                         //Imprime unidad de medida
   lcd_goto(12,2);                                           //Posiciona el cursor en la pantalla
   itoa(datoExt[2],string);                                  //Funcion que convierte entero en ascii
   lcd_puts(string);                                         //Muestra en el LCD
   lcd_goto(15,2);                                           //Posiciona el cursor en la pantalla
   lcd_putch("L");                                           //Imprime unidad de medida
   lcd_goto(3,1);                                            //Posiciona el cursor en la pantalla
   itoa(datoInt[0],string);                                  //Funcion que convierte entero en ascii
   lcd_puts(string);                                         //Muestra en el LCD
   lcd_goto(5,1);                                            //Posiciona el cursor en la pantalla
   lcd_putch(0xDF);                                          //Imprime unidad de medida
   lcd_goto(6,1);                                            //Posiciona el cursor en la pantalla
   lcd_putch("C");                                           //Imprime unidad de medida
   lcd_goto(10,1);                                           //Posiciona el cursor en la pantalla
   itoa(datoInt[1],string);                                  //Funcion que convierte entero en ascii
   lcd_puts(string);                                         //Muestra en el LCD
   lcd_goto(12,1);                                           //Posiciona el cursor en la pantalla
   lcd_putch("%RH");                                         //Imprime unidad de medida
}
//*********************************************************************************************************
// Configuracion puerto serie a 9600 baud con cristal externo de 16MHz.
//*********************************************************************************************************
void configuraUART(void){
   SCON0  = 0x50;                                            //SCON0: mode 1, 8-bit UART, enable rcvr
   TMOD  |= 0x20;                                            //TMOD:  timer 1, mode 2, 8-bit reload
   TH1    = 76;                                              //TH1:   reload value for 600 baud @ 16MHz
   TR1    = 1;                                               //TR1:   timer 1 run
   TI0    = 1;                                               //TI0:   set TI to send first char of UART
}
//*********************************************************************************************************
// Programa principal, Realiza la lectura de DHT11, lectura de bateria y recibe los datos por UART
// Lee la temperatura y humedad interna, realiza un pronóstico aproximado
//*********************************************************************************************************
void main(){                                                 //Funcion principal
   configuraUART();                                          //Funcion que configura UART
   lcd_init();                                               //Inicializa LCD
   lcd_clear();                                              //Borra LCD 
   saludoPantalla();                                         //Funcion de saludo de pantalla
   while(1){                                                 //Loop principal infinito
      leeDHT11();                                            //Funcion que lee DHT11
      delay_ms(2000);                                        //Delay de Actualizacion
      impPantalla();                                         //Imprime pantalla LCD
      delay_ms(2000);                                        //Delay de Actualizacion
   }
}
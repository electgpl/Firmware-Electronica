//*********************************************************************************************************
// Programa para la estacion meteorologica Interna inalámbrica
// El programa recibe Temperatura, Humedad y Luminosidad por interrupcion
// El sistema mide Temperatura y Humedad interior
// El enlace se realiza mediante una comunicacion ASK OOK UHF sobre UART Invertido a 600bps
// Se muestra medicion interna y externa en un LCD de 2x16
// Microcontrolador Silabs C8051F832
//*********************************************************************************************************
#include <REG51F800.H>
#include <reg52.h>
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
//*********************************************************************************************************
// Configuracion de pines del LCD
//*********************************************************************************************************
extern bit RS;                                               //Definicion de pines                                                                   
extern bit EN;                                               //Definicion de pines                            
extern bit D4;                                               //Definicion de pines 
extern bit D5;                                               //Definicion de pines 
extern bit D6;                                               //Definicion de pines 
extern bit D7;                                               //Definicion de pines 
sbit RS = P2^0;                                              //Definicion de pines                                                                    
sbit EN = P2^1;                                              //Definicion de pines                            
sbit D4 = P2^4;                                              //Definicion de pines
sbit D5 = P2^5;                                              //Definicion de pines
sbit D6 = P2^6;                                              //Definicion de pines
sbit D7 = P2^7;                                              //Definicion de pines
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
// Funcion que escribe en puerto
//*********************************************************************************************************
void lcdPort(char a){                                        //Funcion para escribir el puerto en 4bit
    if(a&1) D4=1;                                            //Evalua si a AND 0001, D4 a HIGH
      else  D4=0;                                            //Caso contrario D4 a LOW
    if(a&2) D5=1;                                            //Evalua si a AND 0010, D5 a HIGH 
      else  D5=0;                                            //Caso contrario D5 a LOW
    if(a&4) D6=1;                                            //Evalua si a AND 0100, D6 a HIGH 
      else  D6=0;                                            //Caso contrario D6 a LOW
    if(a&8) D7=1;                                            //Evalua si a AND 1000, D7 a HIGH 
      else  D7=0;                                            //Caso contrario D7 a LOW
 }
 //*********************************************************************************************************
 // Funcion que envia comando
 //*********************************************************************************************************
 void lcdCmd(char a){                                        //Funcion para realizar comandos en LCD
    RS=0;                                                    //Control RS a LOW
    lcdPort(a);                                              //Llamado a lcdPort
    EN=1;                                                    //Control EN a HIGH
    delay_ms(5);                                             //Espera de 5ms
    EN=0;                                                    //Control EN a LOW
 }
 //*********************************************************************************************************
 // Borrado de LCD
 //*********************************************************************************************************
 lcdClear(){                                                 //Funcion para borrar el LCD
    lcdCmd(0);                                               //Se envia comando LOW
    lcdCmd(1);                                               //Se envia comando HIGH
 }
 //*********************************************************************************************************
 // Funcion para ir a la posicion especifica
 //*********************************************************************************************************
 void lcdGotoxy(char b, char a){                             //Funcion para posicionar el cursor
    char temp,z,y;                                           //Declaracion de variables
    if(a==1){                                                //Si se encuentra en renglon 1
      temp=0x80+b;                                           //Se incremena la columna
       z=temp>>4;                                            //Se realiza el desplazamiento
       y=temp&0x0F;                                          //Se aplica la mascara de bits
       lcdCmd(z);                                            //Envia comandos
       lcdCmd(y);                                            //Envia comandos
    }else{                                                   //Si no esta en renglon 1
       if(a==2){                                             //Si se encuentra en renglon 2
          temp=0xC0+b;                                       //Se incrementa la columna
          z=temp>>4;                                         //Se realiza el desplazamiento
          y=temp&0x0F;                                       //Se realiza la mascara
          lcdCmd(z);                                         //Envia comandos
          lcdCmd(y);                                         //Envia comandos
       }
    }		
 }
 //*********************************************************************************************************
 // Inicializa LCD
 //*********************************************************************************************************
 void lcdInit(){                                             //Funcion para inicializar el LCD
    lcdPort(0x00);                                           //Se envia 0000 0000
    delay_ms(200);                                           //Delay de 200ms
    lcdCmd(0x03);                                            //Se envia 0000 0011
    delay_ms(50);                                            //Delay de 50ms
    lcdCmd(0x03);                                            //Se envia 0000 0011
    delay_ms(110);                                           //Delay de 110ms
    lcdCmd(0x03);                                            //Se envia 0000 0011
    lcdCmd(0x02);                                            //Se envia 0000 0010
    lcdCmd(0x02);                                            //Se envia 0000 0010
    lcdCmd(0x08);                                            //Se envia 0000 1000
    lcdCmd(0x00);                                            //Se envia 0000 0000 
    lcdCmd(0x0C);                                            //Se envia 0000 1100
    lcdCmd(0x00);                                            //Se envia 0000 0000
    lcdCmd(0x06);                                            //Se envia 0000 0110
 }
 //*********************************************************************************************************
 // Funcion para escribir caracter
 //*********************************************************************************************************
 void lcdWriteChar(char a){                                  //Funcion para escribir un caracter
    char temp,y;                                             //Declaracion de variables
    temp=a&0x0F;                                             //Se realiza la mascara de bits 0000 1111
    y=a&0xF0;                                                //Se realiza la mascara de bits 0000 1111
    RS=1;                                                    //Control RS a HIGH
    lcdPort(y>>4);                                           //Se desplaza 4 lugares a la derecha
    EN=1;                                                    //Control EN a HIGH
    delay_ms(5);                                             //Delay de 5ms
    EN=0;                                                    //Control EN a LOW
    lcdPort(temp);                                           //Se envia el valor
    EN=1;                                                    //Control EN a HIGH
    delay_ms(5);                                             //Delay de 5ms
    EN=0;                                                    //Control EN a LOW
 }
 //*********************************************************************************************************
 // Funcion para escribir string
 //*********************************************************************************************************
 void lcdWriteString(char *a){                               //Funcion para escribir string
    int i;                                                   //Declaracion de variables
    for(i=0;a[i]!='\0';i++)                                  //Iteracion de caracteres hasta el NULL
       lcdWriteChar(a[i]);                                   //Se carga cada caracter para formar string
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
// Funcion para inicializar el puerto serie 9600 @ 11.059MHz
//*********************************************************************************************************
void serialInit(void){                                       //Funcion para configurar UART
    TMOD=0x20;                                               //Timer 1 en modo 2 - Auto recarga para generar Baudrate
    SCON=0x50;                                               //Serial modo 1, 8bit de dato, 1bit de start, 1bit de stop
    TH1=0xFD;                                                //Carga el baudrate en el timer a 9600bps
    TR1=1;                                                   //Dispara el timer
 }
 //*********************************************************************************************************
 // Función Interrupción UART que realiza el parse de datos, validación de Header y Checksum
 // Si validación y Checksum son validos, carga el vector DATO para ser utilizado
 //*********************************************************************************************************
 char i=0;                                                   //Variable para el contador de bytes de entrada
 void serial_ISR(void) interrupt 4{                          //Función de servicio de interrupción
      if(RI==1){                                             //Si hay dato pendiente en UART
       payload[i++]=SBUF;                                    //Guarda byte de entrada en payload. incrementa indice
       RI=0;                                                 //Pone a cero el flag
    }                                      
    if(i>5){                                                 //Si el indice es mayor que 5 se asume que se completa el payload
       if(payload[0]==HEADER){                               //Validación que el Header sea 200 (seteado en el transmisor)
          if(payload[1]+payload[2]+payload[3]-payload[4]==0){//Validación de checksum, si datos leídos son igual a checksum
             datoExt[0]=payload[1];                          //Cargamos los datos en el vector
             datoExt[1]=payload[2];                          //Cargamos los datos en el vector
             datoExt[2]=payload[3];                          //Cargamos los datos en el vector
          }
       }
       i=0;                                                  //Una vez que se completan los 5 bytes, se reinicia el contador
    }	 
    RI=0;                                                    //Pone a cero el flag 
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
// Saludo de Pantalla
//*********************************************************************************************************
void saludoPantalla(void){                                   //Funcion que realiza un saludo inicial
   lcdGotoxy(1,1);                                           //Posiciona el cursor en la pantalla
   lcdWriteString(" Est.Inalambrica");                       //Imprime mensaje renglon 1
   lcdGotoxy(1,2);                                           //Posiciona el cursor en la pantalla
   lcdWriteString(" v1.1  UTN INSPT");                       //Imprime mensaje renglon 2
   delay_ms(2000);                                           //delay_ms de saludo
   lcdInit();                                                //Inicializa LCD
   lcdClear();                                               //Borra LCD 
}
//*********************************************************************************************************
// Impresion de Pantalla
//*********************************************************************************************************
void impPantalla(void){                                      //Funcion que imprime pantalla
   char string[4];                                           //Declaración de vector para mostrar en LCD
   lcdGotoxy(1,2);                                           //Posiciona el cursor en la pantalla
   itoa(datoExt[0],string);                                  //Funcion que convierte entero en ascii
   lcdWriteString(string);                                   //Muestra en el LCD
   lcdGotoxy(3,2);                                           //Posiciona el cursor en la pantalla
   lcdWriteString(0xDF);                                     //Imprime unidad de medida
   lcdGotoxy(4,2);                                           //Posiciona el cursor en la pantalla
   lcdWriteString("C");                                      //Imprime unidad de medida
   lcdGotoxy(6,2);                                           //Posiciona el cursor en la pantalla
   itoa(datoExt[1],string);                                  //Funcion que convierte entero en ascii
   lcdWriteString(string);                                   //Muestra en el LCD
   lcdGotoxy(8,2);                                           //Posiciona el cursor en la pantalla
   lcdWriteString("%RH");                                    //Imprime unidad de medida
   lcdGotoxy(12,2);                                          //Posiciona el cursor en la pantalla
   itoa(datoExt[2],string);                                  //Funcion que convierte entero en ascii
   lcdWriteString(string);                                   //Muestra en el LCD
   lcdGotoxy(15,2);                                          //Posiciona el cursor en la pantalla
   lcdWriteString("L");                                      //Imprime unidad de medida
   lcdGotoxy(3,1);                                           //Posiciona el cursor en la pantalla
   itoa(datoInt[0],string);                                  //Funcion que convierte entero en ascii
   lcdWriteString(string);                                   //Muestra en el LCD
   lcdGotoxy(5,1);                                           //Posiciona el cursor en la pantalla
   lcdWriteString(0xDF);                                     //Imprime unidad de medida
   lcdGotoxy(6,1);                                           //Posiciona el cursor en la pantalla
   lcdWriteString("C");                                      //Imprime unidad de medida
   lcdGotoxy(10,1);                                          //Posiciona el cursor en la pantalla
   itoa(datoInt[1],string);                                  //Funcion que convierte entero en ascii
   lcdWriteString(string);                                   //Muestra en el LCD
   lcdGotoxy(12,1);                                          //Posiciona el cursor en la pantalla
   lcdWriteString("%RH");                                    //Imprime unidad de medida
}
//*********************************************************************************************************
// Programa principal, Realiza la lectura de DHT11, lectura de bateria y recibe los datos por UART
// Lee la temperatura y humedad interna, realiza un pronóstico aproximado
//*********************************************************************************************************
void main(){                                                 //Funcion principal
   saludoPantalla();                                         //Función de saludo de pantalla
   P1=0x00;                                                  //Usado para aplicacion
   P3=0x03;                                                  //Usado para el serie
   serialInit();                                             //Inicializa puerto serie
   EA=1;                                                     //Habilitacion de interrupcion Global
   ES=1;                                                     //Habilitacion de interrupcion Serie
   while(1){                                                 //Loop principal infinito
      leeDHT11();                                            //Funcion que lee DHT11
      delay_ms(2000);                                        //delay_ms de Actualizacion
      impPantalla();                                         //Imprime pantalla LCD
      delay_ms(2000);                                        //delay_ms de Actualizacion
   }
}

#include <16F883.h>
#FUSES NOWDT
#FUSES HS
#FUSES MCLR
#use delay(clock=4000000)
#use rs232(baud=9600,parity=N,xmit=PIN_C6,rcv=PIN_C7,bits=8)
#define DATA PIN_B0                    //Pin del bus de un hilo para el DHT11
unsigned int trama[5];                 //Vector donde se alojan los datos
unsigned int recibeByte()              //Funcion que recibe un Byte
{ 
   unsigned int8 valorLeido = 0;       //Valor de retorno de la funcion
   int8 i=0;
   for(i=0; i<8; i++)                  //Iteracion para recepcion de bits
   { 
      valorLeido <<= 1;                //Registro de desplazamiento de bits
      while(!input(DATA));             //Espera a DATA = 0
      delay_us(30);                    //Demora de 30us (Del Datasheet)
      if(input(DATA))                  //Pregunta si DATA = 1
      { 
          valorLeido |= 1;             //Realiza toggle del valor leido
      } 
      while(input(DATA));              //Espera a DATA = 1
   } 
   return valorLeido;                  //Retorna el valor leido
} 
unsigned int recibeDato()              //Funcion que recibe el Dato
{ 
   int validacion = 0;                 //Variable de Validacion
   int checksum = 0;                   //Variable de deteccion de cambios de secuencia
   int8 j=0;                           //Variable para el lazo for
   output_high(DATA);                  //Set DATA = 1   
   output_low(DATA);                   //Set DATA = 0
   delay_ms(18);                       //Demora de 18ms (Del Datasheet)
   output_high(DATA);                  //Set DATA = 1
   delay_us(25);                       //Demora de 25ms (Del Datasheet)
   validacion = input(DATA);           //Mueve valor de DATA a Validacion
   if(validacion)                      //Si Validacion = 1, Sensor no encontrado
   { 
      printf( "Sensor no encontrado. \r");                      
   } 
   delay_us(80);                       //Espera 80us (Del Datasheet)
   validacion = input(DATA);           //Mueve valor de DATA a Validacion
   if(!validacion)                     //Si Validacion = 0, Error de secuencia
   { 
      printf( "Error en secuencia (Checksum) \r");   
   } 
   delay_us(80);                       //Espera 80us (Del Datasheet)
   for(j=0; j<5; j++)                  //Lazo de carga de bytes de datos
   { 
       trama[j] = recibeByte();        //Carga del vector de datos
   } 
   output_high(DATA);                  //Set DATA = 1
   for(j=0; j<4; j++)                  //Lazo de carga de bytes de verificacion
   { 
       checksum += trama[j];           //Carga de bytes de verificacion
   } 
   if(checksum == trama[4])            //Si la secuencia de verificacion es correcta
   { 
      return 0;                        //Se retorna 0 y se realiza la lectura
   }
} 
void main()
{
   setup_timer_0(RTCC_INTERNAL|RTCC_DIV_1);
   setup_timer_1(T1_DISABLED);
   setup_timer_2(T2_DISABLED,0,1);
   while(TRUE)
   {
      if(recibeDato()==0)              //Si el retorno es 0, se imprime en terminal
         printf( "Temp: %2u - R.H: %2u \r", trama[2], trama[0]);
      delay_ms(500);
   }
}
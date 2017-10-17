// Programa que recibe tramas RC5 del sensor IRM8601 - S las decodifica
// y muestra el comando y la direcci󮠰or un display LCD 2 x 16.

#include <16f88.h>              				// Definiciones de registros internos del micro.     
#fuses HS,NOWDT,NOPROTECT,PUT   				// Fuses del programador.
#use delay (clock=20000000)     				// Utilizamos un cristal de 20Mhz.
#use fast_io(b)

// Definiciones de hardware.
#define IR_RECEPTOR PIN_B0              		// Pin donde estarᠣonectado el receptor IR.
#define INDICADOR_TRAMA_RECIBIDA PIN_B1 		// Led que indica cuando se recibe una trama RC5.

// Constantes del programa.
const int16    cuartodebit=440;   				// tiempo de 1/4 bit
const int16    mediobit=880;      				// tiempo de 1/2 bit

// Librer� de hardware externo:
#include <74HC595.c>            				// Expansor de 8 salidas.
int16 decodifica_RC5();          				// Declaramos la variable.
								
#int_EXT										// Interrupci󮠰or RB0 en la que recibimos los datos del sensor.
EXT_isr() {
  decodifica_RC5(); 							// Llama a la funci󮠤e decodificaci󮠤e trama de datos RC5.
  output_high(INDICADOR_TRAMA_RECIBIDA);
}
// Funcion decodificacion RC5:
int16 decodifica_RC5()
{ 
   int16 D0=0, D1=65535; 						// Variables que contienen los datos.
   int i;                						// Auxiliar.
      while (input(IR_RECEPTOR)); 				// Espera la transici󮠱 a 0 para empezar e leer.  
      delay_us(cuartodebit);   					// Espera el bit de start de 440uS.  
      for(i=0;i<13;i++){       					// Recoje los 13 bits de datos y los guarda...
         delay_us(mediobit);   					// espera el tiempo de medio bit 880uS.
         D1=D1*2+input(IR_RECEPTOR);   			// lee los datos...
         delay_us(mediobit);   					// espera el tiempo de medio bit 880uS.
         D0=D0*2+input(IR_RECEPTOR);   			// lee los datos pero invertidos...        
      }
      if ((D0^D1)==65535) return (D1 & 0x3FFF); else return (0);
      											// Acᠣhequea que el valor que devuelva la funci󮠏R_EXCLUSIVA entre 
      											// D1 (c󤩧o original) ^ D0 (c󤩧o invertido) sea igual a 3FFF (por ser 
      											// valores coindidentes pero invertidos) ...si ese es el caso entonces devuelve
      											// el valor de D1 ( datos originales ) para ser procesados...caso contrario retorna 0.
}
void main() {
   set_tris_b(0x01);           					// RB0 como entrada , las dem᳠como salidas.
   output_low(INDICADOR_TRAMA_RECIBIDA); 		// Limpiamos el indicador de trama recibida.
   enable_interrupts(int_ext); 					// Habilitamos la interrupci󮠥xterna por RB0.
   ext_int_edge(H_TO_L);  						// Activa la interrupci󮠰or flanco de bajada.
   enable_interrupts(global);  					// Habilitamos las interrupciones globales.
   output_74HC595(0x00); 						// Limpiamos las salidas.
   while(1){
      long data;
      data=decodifica_RC5();       	 			// Leemos los datos RC5.
      data=data & 0x007F;     					// Limpiamos los bits de start y el de togle.
     											// Determinamos cual es el comando recibido y actuamos en consecuencia.
      if (data==1) output_74HC595(0x01); 
      if (data==2) output_74HC595(0x02);
      if (data==3) output_74HC595(0x04);
      if (data==4) output_74HC595(0x08); 
      if (data==5) output_74HC595(0x10);
      if (data==6) output_74HC595(0x20);
      if (data==7) output_74HC595(0x40);
      if (data==8) output_74HC595(0x80);
      if (data==12)                
      {
       output_74HC595(0x00); 					// Limpiamos las salidas.         
      }
       output_low(INDICADOR_TRAMA_RECIBIDA); 	// Limpiamos el indicador de trama recibida.
   }
}
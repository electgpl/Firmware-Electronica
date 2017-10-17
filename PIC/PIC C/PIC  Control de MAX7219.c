#include <16F883.h>
#use delay(clock=4000000)
//Pines a utilizar
#define CLK  PIN_B6
#define LOAD PIN_B5
#define DATA PIN_B4
//Variables del MAX7219 segun Datasheet
#define decodifica 0x09
#define brillo 0x0A
#define cantDigitos 0x0B
#define bajoConsumo 0x0C
//Variable de formateo de salida
long datoSerial=0;
//Funcion de Clock
void clock7219(){
   output_low(CLK);
   delay_ms(1);
   output_high(CLK);
}
//Funcion para cargar el valor
void load7219(){
   output_low(LOAD);
   delay_ms(1);
   output_high(LOAD);
}
//Funcion para enviar el dato de 16bit
void envia7219(long dato){
   int cont;
   for(cont=0;cont<16;++cont){
      output_bit(DATA,shift_left(&dato,2,0));
      clock7219();
   }
   load7219();
}
//Funcion para construir el dato a enviar
void creaDato(byte modo, int datoEntrante){
   datoSerial=0x00;
   datoSerial<<=4;
   datoSerial|=modo;
   datoSerial<<=8;
   datoSerial|=datoEntrante;
   envia7219(datoSerial);
}
//Funcion para escibir, Digito y Valor (1xxx xxxx DP)
void escribe7219(byte digito, int dato){
   creaDato(digito, dato);
}
//Modo bajo consumo 1=WakeUp, 0=Sleep
void bajoConsumo7219(int modoOperacion){
   creaDato(bajoConsumo,modoOperacion);
}
//Modo de codificacion Binario(15) o Decada
void decodifica7219(int modoDecodifica){
   creaDato(decodifica, modoDecodifica);
}
//Configuracion del nivel de brillo 1 a 15
void brillo7219(int nivelBrillo){
   creaDato(brillo, nivelBrillo);
}
//Configuracion de la cantidad de digitos 1 a 8
void cantDigitos7219(int totalDigitos){
   creaDato(cantDigitos, totalDigitos);
}
void inicia7219(){
   creaDato(bajoConsumo,1);
   //Deco 7seg, 255 para 8dig, 15 para 4dig
   creaDato(decodifica,255);
   //Cant 7seg, 0 al 7 
   creaDato(cantDigitos,7);
   //Brillo minimo 0 y maximo 15
   creaDato(brillo,15);
}
void main(){
   inicia7219();
   while(true){
      escribe7219(1,0);
      escribe7219(2,0);
      escribe7219(3,0);
      escribe7219(4,1);
      escribe7219(5,2);
      escribe7219(6,1);
      escribe7219(7,7);
      escribe7219(8,0);
   }
}
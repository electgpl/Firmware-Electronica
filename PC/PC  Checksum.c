//Simulador ONLINE
//https://www.onlinegdb.com/online_c_compiler
#include <stdio.h>
#include <stdint.h>
//Seteo de valores de ejemplo, esto proviene de los sensores en la realidad.
uint16_t temp  = 30;
uint16_t hum   = 60;
uint16_t luz   = 180;
//Variable Checksum en el transmisor
uint8_t checksum_tx = 0;
//Variable a alojar valores leídos y checksum en el receptor
uint16_t valor[4];
uint8_t checksum_rx = 0;
//Funcion Principal
int main(){
   //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<  EN EL TRANSMISOR  >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
   //Simulamos el programa trnasmisor, calculando el checksum
   //Sumamos todos los valores, y como son variables de 8bit los bit que exceden este byte se pierden
   //Ejemplo: Temp=30, Hum=60, Luz=180: 30+60+180=270, 0001 0000 1110, toma solo 8bit 0000 1110 = 14
   checksum_tx = temp + hum + luz;
   //Simulamos el envió de datos por UART  
   //Se envía decimal 30 60 180 14, se pasa a hexadecimal 0x1E 0x3C 0xB4 0x0E
   printf( "envió.: %02x%02x%02x%02x\n", temp, hum, luz, checksum_tx);
   //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<  EN EL RECEPTOR  >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
   //Entraran 4 byte por UART, cada valor que entra lo alojamos en un indice del vector valor
   //quedando así los 4 indices del vector cada uno con cada variable y el último con el checksum
   valor[0] = temp;
   valor[1] = hum;
   valor[2] = luz;
   valor[3] = checksum_tx;
   //Realizamos el calculo construyendo nuevamente la suma que se ha enviado por el transmisor
   //dentro del receptor, sumamos los tres valores temp, hum y luz, a esto le restamos el checksum
   //que ha llegado, si el valor es 0 entonces es correcta la transmisión.
   checksum_rx = valor[0] + valor[1] + valor[2] - valor[3];
   //Si el checksum es 0, entonces mostramos los valores temp, hum y luz.
   //Si el checksum no es 0, entonces el calculo es erróneo y algún valor se perdió o llego mal
   //entonces se muestra el mensaje de "Error en Checksum"
   if(checksum_rx == 0){
      printf( "temp.: %d\n", valor[0] );
      printf( "hum..: %d\n", valor[1] );
      printf( "luz..: %d\n", valor[2] );
   }else{
      printf( "Error en Checksum!!!\n" );
   }
   return 0;
}
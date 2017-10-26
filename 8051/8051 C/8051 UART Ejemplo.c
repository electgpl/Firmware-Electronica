#include<reg52.h>
#define Baud_rate 0xFD  												//BAUD RATE 9600                     
sbit P1_0 = P1^0;
sbit P1_1 = P1^1;
sbit P1_2 = P1^2;
sbit P1_3 = P1^3;
//*********************************************************************************************************
// Funcion para inicializar el puerto serie 9600 @ 11.059MHz
//*********************************************************************************************************
void serialInit(void){                   	
   TMOD = 0x20;                           			//Timer 1 en modo 2 - Auto recarga para generar Baudrate
   SCON = 0x50;                           			//Serial modo 1, 8bit de dato, 1bit de start, 1bit de stop
   TH1 = Baud_rate;                       			//Carga el baudrate en el timer
   TR1 = 1;                               			//Dispara el timer
}
//*********************************************************************************************************
// Funcion para enviar byte por puerto serie
//*********************************************************************************************************
void sendByte(unsigned char serialdata){
   SBUF = serialdata;                        		//Carga el dato a enviar por uart
   while(TI == 0);                            	//Espera a la transmicion completa
   TI = 0;                                    	//Borra el flag de transmision
}
//*********************************************************************************************************
// Interrupcion de byte presente en puerto serie
//*********************************************************************************************************
void serial_ISR (void) interrupt 4{
   char valor;
   if(RI==1){
      valor = SBUF;
      RI = 0;
   }
   switch(valor){
   case '1': P1_0 = 1; sendByte('1'); break;
   case '2': P1_1 = 1; sendByte('1'); break;
   case '3': P1_2 = 1; sendByte('1'); break;
   case '4': P1_3 = 1; sendByte('1'); break;
   case '5': P1_0 = 0; sendByte('0'); break;
   case '6': P1_1 = 0; sendByte('0'); break;
   case '7': P1_2 = 0; sendByte('0'); break;
   case '8': P1_3 = 0; sendByte('0'); break; 
   default: ; break;
   }
   RI = 0;
}
//*********************************************************************************************************
// Funcion principal
//*********************************************************************************************************
void main(){
   P1 = 0x00; 																	//Usado para aplicacion
   P3 = 0x03; 																	//Usado para el serie
   serialInit();    
   EA = 1;
   ES = 1;
   while(1) {;}
}

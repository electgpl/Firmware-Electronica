//NRF24l01 - Codigo Transmisor.
#include <Spi.h>
#include <mirf.h>
//Funcion de configuracion
void setup(){
    Serial.begin(9600);						//Configuracion del UART a 9600bps
    Mirf.init();							//Inicializacion
    Mirf.setRADDR((byte *)"enlace");	    //Nombre del canal Transmisor
    Mirf.payload = 1;				   	 	//Se transmitira 1 solo byte
    Mirf.channel = 10;						//Se utilizara el canal 10
    Mirf.config();							//Se configura el sistema
	Mirf.configRegister(RF_SETUP,0x06);		//Set a 1MHz de ancho de banda
}
//Funcion de iteracion infinita
void loop(){
    byte var=100; 							//Definicion de variable y asignacion de valor
	Mirf.send(&var);						//Enviamos el valor asignado
    while(Mirf.isSending());				//Esperamos hasta que el dato sea enviado
    delay(100);								//Demora de 100ms
}
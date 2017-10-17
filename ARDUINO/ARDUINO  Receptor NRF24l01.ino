//NRF24l01 - Codigo Receptor.
#include <SPI.h>
#include <Mirf.h>
#include <nRF24L01.h>
//Funcion de configuracion
void setup(){
    Serial.begin(9600);						//Configuracion del UART a 9600bps
    Mirf.init();							//Inicializacion
    Mirf.setRADDR((byte *)"enlace");	    //Nombre del canal Transmisor
    Mirf.payload = 1;				   	 	//Se transmitira 1 solo byte
    Mirf.channel = 10;						//Se utilizara el canal 90
    Mirf.config();							//Se configura el sistema
	Mirf.configRegister(RF_SETUP,0x06);		//Set a 1MHz de ancho de banda
}
//Funcion de iteracion infinita
void loop(){
    byte var; 								//Definicion variable var
    if(Mirf.dataReady()){					//Hay dato presente en la entrada?
        Mirf.getData(&var);					//Mueve el dato recibido a la variable var
        Serial.print(var);					//Muestra el valor en la terminal
  }
}
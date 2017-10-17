//NRF24l01 - Codigo Servidor.
#include <SPI.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <MirfHardwareSpiDriver.h>
#include <Wire.h>
//Estructura de datos a enviar
struct DATOS{
    unsigned long time;
}datos;
//Funcion de configuracion
void setup(){
    Serial.begin(9600);
    Mirf.spi    = &MirfHardwareSpi;
    Mirf.cePin  = 8;
    Mirf.csnPin = 10;
    Mirf.init();
    Mirf.setRADDR((byte *)"server");
    Mirf.payload = sizeof(datos);
    Mirf.channel = 10;
    Mirf.config();
}
//Funcion de iteracion infinita
void loop(){
    if(millis()%1000==0){
        datos.time = millis()/1000;
        Serial.println(datos.time);
        if(!Mirf.isSending()){
            Mirf.setTADDR((byte *)"client");
            Mirf.send((byte *)&datos);
        }
    }
}
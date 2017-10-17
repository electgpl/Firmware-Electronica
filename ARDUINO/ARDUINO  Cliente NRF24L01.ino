// NRF24l01 - Codigo Cliente.
#include <SPI.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <MirfHardwareSpiDriver.h>
#include <Wire.h>
//Estructura de datos a recibir
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
    Mirf.setRADDR((byte *)"client");
    Mirf.payload = sizeof(datos);
    Mirf.channel = 10;
    Mirf.config();
}
//Funcion de iteracion infinita
void loop(){
    if(!Mirf.isSending()&Mirf.dataReady()){
        Mirf.getData((byte *)&datos);
        Serial.println(datos.time);
    }
}
#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager
#include <SimpleDHT.h>
#include <Wire.h>
#include <Adafruit_BMP085.h>
#include "ThingSpeak.h"          //https://github.com/mathworks/thingspeak-arduino

unsigned long myChannelNumber = NUMERO DEL CANAL;
const char * myWriteAPIKey = "APIKEY DEL CANAL";

int pinDHT11 = D0;
int pinFACTORY = D6;
int timeOut = 0;
SimpleDHT11 dht11;
WiFiClient client;
Adafruit_BMP085 bmp;

void handleButton(){
   int debounce = 1000;
   if(digitalRead(pinFACTORY)==LOW){
      delay(debounce);
      if(digitalRead(pinFACTORY)==LOW){
         WiFiManager wifiManager;      
         wifiManager.resetSettings();
         delay(1000);
         ESP.reset();
      }
   }
}

void setup(){
   WiFiManager wifiManager;
   ThingSpeak.begin(client);  // Initialize ThingSpeak
   Serial.begin(115200);
   pinMode(pinFACTORY, INPUT);
   delay(10);
   // wifiManager.resetSettings();
   wifiManager.autoConnect("WS-Electgpl");
   Serial.println("Estacion Conectada a Internet");
   if(!bmp.begin()){
      Serial.println("Error en BMP085");
   }
}

void loop(){
   byte temperatura = 0;
   byte humedad = 0;
   float luminosidad = analogRead(A0);
   handleButton();
   dht11.read(pinDHT11, &temperatura, &humedad, NULL);
   /*
   if(dht11.read(pinDHT11, &temperatura, &humedad, NULL)){
      Serial.println("Error en DHT11");
      return;
   }
   */
   if(timeOut==300){
      ThingSpeak.setField(1, luminosidad);
      ThingSpeak.setField(2, (float)bmp.readTemperature());
      ThingSpeak.setField(3, (int)humedad);
      ThingSpeak.setField(4, (float)bmp.readPressure()/100);
      ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
      Serial.print("luminosidad: ");
      Serial.print(luminosidad);
      Serial.print(" | temperatura BMP: ");
      Serial.print((float)bmp.readTemperature());
      Serial.print(" | temperatura DHT: ");
      Serial.print(temperatura);
      Serial.print(" | humedad: ");
      Serial.print(humedad);
      Serial.print(" | presion: ");
      Serial.println((float)bmp.readPressure()/100);
      timeOut=0; 
   }
   delay(1000);
   timeOut++;
   Serial.print("timeOut");
   Serial.println(timeOut);
}

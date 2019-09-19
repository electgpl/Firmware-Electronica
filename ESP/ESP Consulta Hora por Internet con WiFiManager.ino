//https://www.ntppool.org/zone/south-america
//https://github.com/arduino-libraries/NTPClient
//https://github.com/tzapu/WiFiManager
#include <WiFiManager.h>         
#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
WiFiClient client;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "ar.pool.ntp.org", -10800, 60000);
void setup(){
   WiFiManager wifiManager;
   Serial.begin(115200);
   wifiManager.resetSettings();
   wifiManager.autoConnect("EXO-HORA");
   timeClient.begin(); 
}
void loop(){
   timeClient.update();
   Serial.println(timeClient.getFormattedTime());
   //Serial.print("Horas: "); Serial.println(timeClient.getHours());
   //Serial.print("Minutos: "); Serial.println(timeClient.getMinutes());
   //Serial.print("Segundos: "); Serial.println(timeClient.getSeconds());
   delay(1000);
}

//https://www.ntppool.org/zone/south-america
//https://github.com/arduino-libraries/NTPClient
//https://github.com/tzapu/WiFiManager
#include <WiFiManager.h>
#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
WiFiClient client;
WiFiServer server(80);
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "ar.pool.ntp.org", -10800, 60000);
void setup(){
   WiFiManager wifiManager;
   Serial.begin(115200);
   //wifiManager.resetSettings();
   wifiManager.autoConnect("EstacionLOCAL");
   server.begin();
   Serial.println("Servidor Iniciado");
   Serial.println("");
   Serial.println("Conectado a la WIFI");
   Serial.println("dirección IP: ");
   Serial.println(WiFi.localIP());
   timeClient.begin();
}
void loop(){
   WiFiClient client = server.available();
   if(!client){
      return;
   }
   timeClient.update();
   Serial.println(timeClient.getFormattedTime());
   //Serial.print("Horas: "); Serial.println(timeClient.getHours());
   //Serial.print("Minutos: "); Serial.println(timeClient.getMinutes());
   //Serial.print("Segundos: "); Serial.println(timeClient.getSeconds());
   delay(1000);
   //String t = "20:12:23";
   float temp = 23.5;
   float pres = 1015.3;
   float lum = 70;
   String s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n";
   s += "\r\n<html><head><meta http-equiv=\"refresh\" content=\"5\"></head><body>\r\n"; // refresca automáticamente la página cada 5s
   s += "<h2>Estacion Local</h2>";
   s += "TIEMPO.....: " + String(timeClient.getFormattedTime()) + "<br>";
   s += "TEMPERATURA: " + String(temp) + " &deg" + "C <br>";
   s += "PRESION....: " + String(pres) + " hPa <br>";
   s += "LUMINOSIDAD: " + String(lum)  + " % <br>";
   s += "</body></html>\n";
   client.print(s);
}

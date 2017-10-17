#include <ESP8266WiFi.h>
#include <SimpleDHT.h>
String apiKey = "EHW19OYPTK9J7F40";
const char* ssid = "ssid";
const char* password = "pass";
const char* server = "api.thingspeak.com";
int pinDHT11 = D0;
SimpleDHT11 dht11;
WiFiClient client;
void setup() {
  Serial.begin(115200);
  delay(10);
  pinMode(A0, INPUT);
  WiFi.begin(ssid, password);
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
}
void loop() {
  byte temperatura = 0;
  byte humedad = 0;
  float luminosidad = analogRead(A0);
  if (dht11.read(pinDHT11, &temperatura, &humedad, NULL)) {
    Serial.print("Read DHT11 failed.");
    return;
  }
  if (client.connect(server,80)) {
    String postStr = apiKey;
    postStr +="&field1=";
    postStr += String(luminosidad);
    postStr +="&field2=";
    postStr += String((int)temperatura);
    postStr +="&field3=";
    postStr += String((int)humedad);
    postStr += "\r\n\r\n";
    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(postStr.length());
    client.print("\n\n");
    client.print(postStr);
    Serial.println("% send to Thingspeak");
  }
  client.stop();
  Serial.println("Waiting…");
  delay(20000);
}
#include <ESP8266WiFi.h>
String apiKey = "EHW19OYPTK9J7F40";
const char* ssid = "ssid";
const char* password = "pass";
const char* server = "api.thingspeak.com";
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
  float adc = analogRead(A0);
  if (client.connect(server,80)) {
    String postStr = apiKey;
    postStr +="&field1=";
    postStr += String(adc);
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
    Serial.print("ADC: ");
    Serial.print(adc);
    Serial.println("% send to Thingspeak");
  }
  client.stop();
  Serial.println("Waiting…");
  delay(20000);
}
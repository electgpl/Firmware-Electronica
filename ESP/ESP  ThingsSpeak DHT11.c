#include <DHT.h>
#include <ESP8266WiFi.h>
String apiKey = "EHW19OYPTK9J7F40";
const char* ssid = "ssid";
const char* password = "pass";
const char* server = "api.thingspeak.com";
#define DHTPIN 2
DHT dht(DHTPIN, DHT11,15);
WiFiClient client;
void setup() {
  Serial.begin(115200);
  delay(10);
  dht.begin();
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
  float h = analogRead(A0);
  float t = analogRead(A0)+10;
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  if (client.connect(server,80)) {
    String postStr = apiKey;
    postStr +="&field1=";
    postStr += String(t);
    postStr +="&field2=";
    postStr += String(h);
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
    Serial.print("Temperature: ");
    Serial.print(t);
    Serial.print(" degrees Celcius Humidity: ");
    Serial.print(h);
    Serial.println("% send to Thingspeak");
  }
  client.stop();
  Serial.println("Waiting…");
  delay(20000);
}
//HTTP WebClient
#include <ESP8266WiFi.h>
const char* ssid     = "yourssid";
const char* password = "yourpassword";
const char* host = "www.adafruit.com";
void setup() {
  Serial.begin(115200);
  delay(100);
//Empezamos mediante la conexión a una red Wi-Fi
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
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}
int value = 0;
void loop() {
  delay(5000);
  ++value;
  Serial.print("connecting to ");
  Serial.println(host);
// Utiliza la clase de cliente WiFi para crear conexiones TCP
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  } 
// Ahora se crea la URL de solicitud
  String url = "/testwifi/index.html";
  Serial.print("Requesting URL: ");
  Serial.println(url);
// Envia la solicitud al servidor
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                     "Host: " + host + "\r\n" + 
                     "Connection: close\r\n\r\n");
  delay(500);  
// Lee todas las líneas de la respuesta del servidor y los imprime en UART
  while(client.available()){
    String line = client.readStringUntil('\r');
    Serial.print(line);
  } 
  Serial.println();
  Serial.println("closing connection");
}
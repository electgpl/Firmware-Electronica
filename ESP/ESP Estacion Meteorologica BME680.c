#include <ESP8266WiFi.h>
#include "ThingSpeak.h" // always include thingspeak header file after other header files and custom macros
#include <DFRobot_BME680_I2C.h>
#include <Wire.h>
DFRobot_BME680_I2C bme(0x76);

#define SECRET_SSID "MySSID"    // replace MySSID with your WiFi network name
#define SECRET_PASS "MyPassword"  // replace MyPassword with your WiFi password

#define SECRET_CH_ID 000000     // replace 0000000 with your channel number
#define SECRET_WRITE_APIKEY "XYZ"   // replace XYZ with your channel write API Key

char ssid[] = SECRET_SSID;   // your network SSID (name) 
char pass[] = SECRET_PASS;   // your network password
int keyIndex = 0;            // your network key Index number (needed only for WEP)
WiFiClient  client;

unsigned long myChannelNumber = SECRET_CH_ID;
const char * myWriteAPIKey = SECRET_WRITE_APIKEY;

float number1 = 0;
float number2 = 0;
float number3 = 0;
float number4 = 0;
float number5 = 0;
String myStatus = "";

void setup() {
   Serial.begin(115200);  // Initialize serial
   WiFi.mode(WIFI_STA); 
   ThingSpeak.begin(client);  // Initialize ThingSpeak
   bme.begin();
}

void loop(){
   if(WiFi.status() != WL_CONNECTED){
      Serial.print("Attempting to connect to SSID: ");
      Serial.println(SECRET_SSID);
      while(WiFi.status() != WL_CONNECTED){
         WiFi.begin(ssid, pass);
         Serial.print(".");
         delay(5000);     
      } 
      Serial.println("\nConnected.");
   } 
   bme.startConvert();
   bme.update();
   number1 = analogRead(A0);
   number2 = bme.readTemperature()/100;
   number3 = bme.readHumidity()/1000;
   number4 = bme.readPressure()/100;
   number5 = bme.readGasResistance();
   Serial.print("Luz : "); Serial.println(number1);
   Serial.print("Temp: "); Serial.println(number2);
   Serial.print("Hum : "); Serial.println(number3);
   Serial.print("Pres: "); Serial.println(number4);
   Serial.print("Gas : "); Serial.println(number5);
   ThingSpeak.setField(1, number1);
   ThingSpeak.setField(2, number2);
   ThingSpeak.setField(3, number3);
   ThingSpeak.setField(4, number4);
   ThingSpeak.setField(5, number5);
   ThingSpeak.setStatus(myStatus);
  
   int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
   if(x == 200){
      Serial.println("Channel update successful.");
   }else{
      Serial.println("Problem updating channel. HTTP error code " + String(x));
   }
   delay(20000);
}

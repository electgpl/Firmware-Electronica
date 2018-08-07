#include <FS.h>
#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
#include <BlynkSimpleEsp8266.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager
#include <ArduinoJson.h>          //https://github.com/bblanchon/ArduinoJson

#define BLYNK_PRINT Serial

char blynk_token[34];
bool shouldSaveConfig = false;

void saveConfigCallback (){
   Serial.println("Should save config");
   shouldSaveConfig = true;
}

void setup(){
   Serial.begin(115200);
   if(SPIFFS.begin()){
      if(SPIFFS.exists("/config.json")){
         File configFile = SPIFFS.open("/config.json", "r");
         if(configFile){
            size_t size = configFile.size();
            std::unique_ptr<char[]> buf(new char[size]);
            configFile.readBytes(buf.get(), size);
            DynamicJsonBuffer jsonBuffer;
            JsonObject& json = jsonBuffer.parseObject(buf.get());
            json.printTo(Serial);
            if(json.success()){
               strcpy(blynk_token, json["blynk_token"]);
            }
         }
      }
   }
   WiFiManagerParameter custom_blynk_token("blynk", "blynk token", blynk_token, 33);
   WiFiManager wifiManager;
   wifiManager.resetSettings(); //Poner en un boton de reset en loop por tiempo
   wifiManager.setSaveConfigCallback(saveConfigCallback);
   wifiManager.addParameter(&custom_blynk_token);
   wifiManager.autoConnect("Blynk Gaber");
   strcpy(blynk_token, custom_blynk_token.getValue());
   if(shouldSaveConfig){
      Serial.println("saving config");
      DynamicJsonBuffer jsonBuffer;
      JsonObject& json = jsonBuffer.createObject();
      json["blynk_token"] = blynk_token;
      File configFile = SPIFFS.open("/config.json", "w");
      if(!configFile){
         Serial.println("failed to open config file for writing");
      }
      json.printTo(Serial);
      json.printTo(configFile);
      configFile.close();
   }
   Blynk.config(blynk_token);
   bool result = Blynk.connect();
   if(result != true){
      Serial.println("BLYNK Connection Fail");
      Serial.println(blynk_token);
      wifiManager.resetSettings();
      ESP.reset();
      delay(5000);
   }
}

void loop(){
   Blynk.run();
}

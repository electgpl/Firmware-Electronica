//SD CARD: CS=15, MOSI=13, SCK=14, MISO=12, 3V3 o 5V, GND
#include <SD.h>
#define CS_PIN 15
File myFile;
void setup(){
   Serial.begin(9600);
   Serial.print("Iniciando SD...");
   if(!SD.begin(CS_PIN)){
      Serial.println("Error de SD, Verifique.");
      return;
   }
   Serial.println("SD OK");
}
void loop(){
   float valor_1 = 128.5;
   float valor_2 = 256.5;
   myFile = SD.open("logger.csv", FILE_WRITE);
   if(myFile){
      Serial.println("Archivo Abierto");
      myFile.print(valor_1);
      myFile.print(",");
      myFile.println(valor_2);
      Serial.println("Archivo Grabado y Cerrado");
      myFile.close();
   }else{
      Serial.println("Error al Abrir Archivo");
   }
   delay(2000);
}

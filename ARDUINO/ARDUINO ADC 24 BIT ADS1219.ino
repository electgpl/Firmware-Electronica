#include <Wire.h>
#include <ADS1219.h>
#define address 0x40
#define rst 3
#define drdy 4

ADS1219 ads(drdy);
float valor = 0;
float relativo = 0;
int i=0;

void setup(){
   ads.begin();
   pinMode(drdy,INPUT_PULLUP);
   pinMode(rst,OUTPUT);
   digitalWrite(rst,HIGH);
   Wire.begin();
   Serial.begin(9600);
   ads.setVoltageReference(REF_INTERNAL);
   delay(1000);
   valor=0;
   for(i=0;i<20;i++){
      valor = valor + (ads.readSingleEnded(1)*2.048/pow(2,23));
      delay(10);
   }
   relativo = valor/20;
   delay(1000);
}

void loop(){
   valor=0;
   for(i=0;i<20;i++){
      valor = valor + (ads.readSingleEnded(1)*2.048/pow(2,23) - relativo);
      delay(10);
   }
   Serial.println(valor/20.0,6);
}

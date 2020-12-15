#include <ADS1219.h>
#include <LiquidCrystal.h>
#include <BigNumbers.h>
#define address 0x40
#define rst 3
#define drdy 4
ADS1219 ads(drdy);
const int rs = 8, en = 7, d4 = 9, d5 = 10, d6 = 11, d7 = 12;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
BigNumbers bigNum(&lcd);

#define SAMPLES 30
float valueADC = 0.0;
float Att = 0.032;
float drift = 0.0013;
float voltage = 0.0;

void setup(){
   Serial.begin(9600);
   ads.begin();
   lcd.begin(16, 2);
   bigNum.begin();
   pinMode(drdy,INPUT_PULLUP);
   pinMode(rst,OUTPUT);
   pinMode(6,INPUT_PULLUP);
   digitalWrite(rst,HIGH);
   ads.setVoltageReference(REF_INTERNAL);
   ads.setDataRate(DATA_RATE_1000);
   ads.setGain(GAIN_ONE);
   lcd.setCursor(0, 0);
   lcd.print(F("   VOLTIMETRO   "));
   lcd.setCursor(0, 1);
   lcd.print(F("    Electgpl    "));
   delay(1000);
   lcd.clear();
}

void loop(){
   valueADC = 0;
   for(int i=0; i<SAMPLES; i++){
      valueADC = valueADC + (ads.readDifferential_2_3()*2.048/pow(2,23));
   }
   valueADC = valueADC / SAMPLES;
   voltage = (valueADC/Att)-drift;
   if(voltage < 10.0){
      bigNum.displayLargeInt(voltage*10000, 1, 5, false);   
      bigNum.displayLargeInt(voltage*1000, 1, 4, false); 
      bigNum.displayLargeInt(voltage*100, 1, 3, false); 
      bigNum.displayLargeInt(voltage*10, 1, 2, false); 
      bigNum.displayLargeInt(voltage*1, 0, 1, false); 
      lcd.setCursor(3, 1);
      lcd.print(".");   
   }else{
      bigNum.displayLargeInt(voltage*10000, 1, 5, false);   
      bigNum.displayLargeInt(voltage*1000, 1, 4, false); 
      bigNum.displayLargeInt(voltage*100, 1, 3, false); 
      bigNum.displayLargeInt(voltage*10, 0, 2, false); 
      bigNum.displayLargeInt(voltage*1, 1, 1, false); 
      lcd.setCursor(6, 1);
      lcd.print(".");      
   }
   Serial.println(voltage);
}

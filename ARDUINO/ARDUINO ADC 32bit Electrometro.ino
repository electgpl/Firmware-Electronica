#include <ADS1219.h>
#include <LiquidCrystal.h>
#define address 0x40
#define rst 3
#define drdy 4
ADS1219 ads(drdy);
const int rs = 8, en = 7, d4 = 9, d5 = 10, d6 = 11, d7 = 12;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

#define SAMPLES 30
float valueADC = 0.0;

void setup(){
   Serial.begin(9600);
   ads.begin();
   lcd.begin(16, 2);
   pinMode(drdy,INPUT_PULLUP);
   pinMode(rst,OUTPUT);
   pinMode(6,INPUT_PULLUP);
   digitalWrite(rst,HIGH);
   ads.setVoltageReference(REF_INTERNAL);
   ads.setDataRate(DATA_RATE_20);
   ads.setGain(GAIN_ONE);
   lcd.setCursor(0, 0);
   lcd.print(F("  ELECTROMETRO  "));
   lcd.setCursor(0, 1);
   lcd.print(F("    Electgpl    "));
   delay(1000);
   lcd.clear();
}

void loop(){
   valueADC = 0;
   for(int i=0; i<SAMPLES; i++){
      valueADC = valueADC + (ads.readDifferential_0_1()*2.048/pow(2,23));
   }
   valueADC = valueADC / SAMPLES;
   lcd.setCursor(0, 0);
   lcd.print(F("32BIT TENSION 2V"));
   lcd.setCursor(3, 1);
   lcd.print(valueADC,6);
   Serial.println(valueADC);
}

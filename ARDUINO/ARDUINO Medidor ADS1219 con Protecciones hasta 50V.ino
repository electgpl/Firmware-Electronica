#include <ADS1219.h>
#include <LiquidCrystal.h>
#define address 0x40
#define rst 3
#define drdy 4
ADS1219 ads(drdy);
const int rs = 8, en = 7, d4 = 9, d5 = 10, d6 = 11, d7 = 12;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

#define SAMPLES 10
float valueADC = 0.0;
float Att = 0.032;
float drift = 0.0013;
float voltage = 0.0;

void setup(){
   Serial.begin(9600);
   ads.begin();
   lcd.begin(16, 2);
   pinMode(drdy,INPUT_PULLUP);
   pinMode(rst,OUTPUT);
   pinMode(6,INPUT_PULLUP);
   digitalWrite(rst,HIGH);
   ads.setVoltageReference(REF_INTERNAL);
   ads.setDataRate(DATA_RATE_1000);
   ads.setGain(GAIN_ONE);
   lcd.setCursor(0, 0);
   lcd.print(F("  VOLTIMETRO   "));
   lcd.setCursor(0, 1);
   lcd.print(F("   Electgpl    "));
   delay(1000);
   lcd.clear();
}

void loop(){
   valueADC = 0;
   for(int i=0; i<SAMPLES; i++){
      valueADC = valueADC + (ads.readDifferential_2_3()*0.000000244);
   }
   valueADC = valueADC / SAMPLES;
   voltage = (valueADC/Att)-drift;
   lcd.setCursor(10, 0);
   lcd.print("50Vmax");
   lcd.setCursor(10, 1);
   lcd.print(voltage,4);
   Serial.println(voltage);
}

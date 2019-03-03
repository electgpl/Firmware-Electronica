//Divisor Resistivo IN+ --> 200k --> (+A) --> 1k --> (-A) --> IN-
//Medicion Flotante
#include "HX711.h"              //https://github.com/bogde/HX711
#include <Wire.h> 
#include<LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);
const double calibrationFactorA = 488.5f;
long offsetChannelA = 0;
const uint8_t HX711_dout = A1; 
const uint8_t HX711_sck = A0; 
HX711 scale;

void setup(){
   lcd.begin();
   lcd.backlight();
   lcd.clear();
   lcd.setCursor(0, 0);
   lcd.print("Calibrando 0...");
   scale.begin(HX711_dout, HX711_sck, 64);
   scale.set_gain(64);
   offsetChannelA = scale.read_average(32);
   scale.set_offset(offsetChannelA);
   scale.set_scale(calibrationFactorA); 
   lcd.setCursor(0, 1);
   lcd.print("Finalizado");
   delay(500);
   lcd.clear();
}

void loop(){
   scale.set_gain(64);
   scale.set_offset(offsetChannelA);
   scale.set_scale(calibrationFactorA);
   long valor = 0;  
   for(int i=0; i<10; i++){
      valor = valor + ((scale.get_units(1)/57.0)*100000.0);      
   }
   lcd.clear();
   lcd.setCursor(0, 0);
   lcd.print("uV: ");
   lcd.print(valor/10);
}

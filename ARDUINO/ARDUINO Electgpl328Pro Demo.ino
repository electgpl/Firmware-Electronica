#include <Wire.h>
#include <Adafruit_ADS1X15.h>
#include <Adafruit_MCP4725.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
Adafruit_ADS1115 ads;
Adafruit_MCP4725 dac;
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET     4
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

int16_t adc0, adc1, adc2, adc3;
float volts0, volts1, volts2, volts3;
int dacOut = 0;

void setup(){
   Serial.begin(9600);
   dac.begin(0x60);
   ads.begin();
   display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
   display.setTextColor(SSD1306_WHITE); 
   display.clearDisplay();
   display.setCursor(0,0);
   display.setTextSize(2);
   display.println("  328Pro  ");
   display.println("__________");
   display.println("Eelectgpl ");
   display.write(2);
   display.display();
   delay(4000);
}

void loop(){
   if(dacOut<4096){
      dacOut++;
   }else{ 
      if(dacOut = 4095)
         dacOut = 0;
   }
   dac.setVoltage(dacOut, false);
   adc0 = ads.readADC_SingleEnded(0);
   adc1 = ads.readADC_SingleEnded(1);
   adc2 = ads.readADC_SingleEnded(2);
   adc3 = ads.readADC_SingleEnded(3);
   volts0 = ads.computeVolts(adc0);
   volts1 = ads.computeVolts(adc1);
   volts2 = ads.computeVolts(adc2);
   volts3 = ads.computeVolts(adc3);
   Serial.print("DAC: "); Serial.print(dacOut);   Serial.print(" | ");
   Serial.print("A0: ");  Serial.print(volts0,3); Serial.print("V | ");
   Serial.print("A1: ");  Serial.print(volts1,3); Serial.print("V | ");
   Serial.print("A2: ");  Serial.print(volts2,3); Serial.print("V | ");
   Serial.print("A3: ");  Serial.print(volts3,3); Serial.println("V"); 
   display.clearDisplay();
   display.setTextSize(1);
   display.setCursor(75,0);  display.print("DAC: ");
   display.setCursor(100,0); display.print((int)dacOut); 
   display.setCursor(0,0);   display.print("A0: ");
   display.setCursor(20,0);  display.print(volts0,3); 
   display.setCursor(0,15);  display.print("A1: ");
   display.setCursor(20,15); display.print(volts1,3); 
   display.setCursor(0,30);  display.print("A2: ");
   display.setCursor(20,30); display.print(volts2,3); 
   display.setCursor(0,45);  display.print("A3: ");
   display.setCursor(20,45); display.print(volts3,3); 
   display.display();    
   delay(50);
}

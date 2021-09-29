#include <Wire.h>
#include <Adafruit_ADS1X15.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
Adafruit_ADS1115 ads;
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET     4
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

int16_t adc0, adc1;
float volts0, volts1;
float channel0, channel1;
int SAMPLES = 50;
float ATT = 13.3;

void setup(){
   ads.begin();
   ads.setGain(ADS1X15_REG_CONFIG_PGA_4_096V);
   display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
   display.setTextColor(SSD1306_WHITE); 
   display.clearDisplay();
   display.setCursor(0,0);
   display.setTextSize(2);
   display.println("Voltmeter ");
   display.println("Dual Chan.");
   display.println("Eelectgpl ");
   display.write(2);
   display.display();
   delay(3000);
}

void loop(){
   channel0 = 0;
   channel1 = 0;
   for(int i=0; i<SAMPLES; i++){
      adc0 = ads.readADC_Differential_0_1();
      adc1 = ads.readADC_Differential_2_3();
      volts0 = ads.computeVolts(adc0);
      volts1 = ads.computeVolts(adc1);
      channel0 = channel0 + volts0;
      channel1 = channel1 + volts1;
   }
   channel0 = (channel0 / SAMPLES) * ATT;
   channel1 = (channel1 / SAMPLES) * ATT;
   display.clearDisplay();
   display.setTextSize(3);
   display.setCursor(0,0);  display.print(channel0,3); 
   display.setCursor(0,35); display.print(channel1,3); 
   display.display();
}

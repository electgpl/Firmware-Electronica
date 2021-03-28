#include <Wire.h>
#include <ADS126X.h>   //https://github.com/Molorius/ADS126X
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
ADS126X adc;
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET     4
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
int startPin = 6;
int chip_select = 5;
int pos_pin = 0; //AIN0, positive input
int neg_pin = 1; //AIN1, for negative input
double voltage = 0;
#define SAMPLES 100
void setup(){
   Serial.begin(9600);
   adc.setStartPin(startPin);
   adc.begin(chip_select);
   adc.setFilter(ADS126X_SINC4);
   adc.startADC1();
   display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
   display.setTextColor(SSD1306_WHITE); 
   display.clearDisplay();
   display.setCursor(0,0);
   display.setTextSize(2);
   display.println("VOLTIMETRO");
   display.println("ADS1263 32");
   display.println("Eelectgpl");
   display.write(2);
   display.display();
   delay(2000);    
}
void loop(){
   for(int i=0;i<SAMPLES;i++){
      voltage = voltage + adc.readADC1(pos_pin,neg_pin);
      delay(1);
   }
   voltage = voltage / SAMPLES;
   Serial.println(voltage,0);
   display.clearDisplay();
   display.setTextSize(2);
   display.setCursor(0,0);
   display.print(voltage,0); 
   display.display();  
}

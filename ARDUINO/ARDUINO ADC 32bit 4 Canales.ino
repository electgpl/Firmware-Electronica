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
double voltage1 = 0;
double voltage2 = 0;
double voltage3 = 0;
double voltage4 = 0;
#define SAMPLES 10
void setup(){
   Serial.begin(9600);
   adc.setStartPin(startPin);
   adc.begin(chip_select);
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
   voltage1 = adc.readADC1(0,1); delay(100); 
   voltage1 = adc.readADC1(0,1); delay(100); 
   Serial.print("Canal 1: "); Serial.println(voltage1,0);
   voltage2 = adc.readADC1(2,3); delay(100);
   voltage2 = adc.readADC1(2,3); delay(100);
   Serial.print("Canal 2: "); Serial.println(voltage2,0);
   voltage3 = adc.readADC1(4,5); delay(100);
   voltage3 = adc.readADC1(4,5); delay(100);
   Serial.print("Canal 3: "); Serial.println(voltage3,0);
   voltage4 = adc.readADC1(6,7); delay(100);
   voltage4 = adc.readADC1(6,7); delay(100);
   Serial.print("Canal 4: "); Serial.println(voltage4,0);
   Serial.println(" ");
   display.clearDisplay();
   display.setTextSize(1);
   display.setCursor(0,0);
   display.print("Canal 1 = ");
   display.setCursor(60,0);
   display.print(voltage1,0); 
   display.setTextSize(1);
   display.setCursor(0,15);
   display.print("Canal 2 = ");
   display.setCursor(60,15);
   display.print(voltage2,0); 
   display.setTextSize(1);
   display.setCursor(0,30);
   display.print("Canal 3 = ");
   display.setCursor(60,30);
   display.print(voltage3,0); 
   display.setTextSize(1);
   display.setCursor(0,45);
   display.print("Canal 4 = ");
   display.setCursor(60,45);
   display.print(voltage4,0); 
   display.display();  
}

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET     4
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define SCLK 10
#define CS   11
#define MISO 12

byte spiRead(void){
   int i;
   byte d = 0;
   for(i = 7; i >= 0; i--) {
      digitalWrite(SCLK, LOW);
      delayMicroseconds(10);
      if(digitalRead(MISO)){
         d |= (1 << i);
      }
      digitalWrite(SCLK, HIGH);
      delayMicroseconds(10);
   }
   return d;
}

float MAX6675_read(void){
   uint16_t v;
   digitalWrite(CS, LOW);
   delayMicroseconds(10);
   v = spiRead();
   v <<= 8;
   v |= spiRead();
   digitalWrite(CS, HIGH);
   v >>= 3;
   return v * 0.25;
}

void setup(void){
   Serial.begin(9600);
   pinMode(CS, OUTPUT);
   pinMode(SCLK, OUTPUT);
   pinMode(MISO, INPUT);
   digitalWrite(CS, HIGH);
   display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
   display.setTextColor(SSD1306_WHITE); 
   display.clearDisplay();
   display.setCursor(0,0);
   display.setTextSize(2);
   display.println("  Medidor ");
   display.println("Termo. T-K");
   display.println("Eelectgpl ");
   display.write(2);
   display.display();
   delay(2000);
   display.clearDisplay();
   display.display();
   delay(2000);
}

void loop(void){
   Serial.println(MAX6675_read());
   display.clearDisplay();
   display.setTextSize(3);
   display.setCursor(20,20);
   display.print(MAX6675_read(),0); 
   display.write(247);
   display.display();  
   delay(1000);
}

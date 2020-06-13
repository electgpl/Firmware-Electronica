#include <Adafruit_TFTLCD.h> 
#include <Adafruit_GFX.h>    

#define LCD_CS A3 
#define LCD_CD A2 
#define LCD_WR A1 
#define LCD_RD A0 
#define LCD_RESET A4 

#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

float volt = 0;
int adcRaw = 0;
int x = 1;

Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

void setup() {
  tft.reset();
  tft.begin(0x9325);
  tft.setRotation(1);
  tft.fillScreen(BLACK);
  
  tft.setCursor(70,30);
  tft.setTextColor(WHITE);
  tft.setTextSize(4);
  tft.print("Electgpl");
  
  tft.setCursor(70,100);
  tft.setTextColor(BLUE);
  tft.setTextSize(4);
  tft.print("ADC PLOT");
  
  tft.fillRect(40,160,240,60,RED);
  tft.drawRect(40,160,240,60,WHITE);
  tft.setCursor(55,175);
  tft.setTextColor(WHITE);
  tft.setTextSize(4);
  tft.print("SUSCRIBE!");
  
  delay(3000);
  tft.fillScreen(BLACK);
}

void loop(){
   tft.fillRect(10,10,115,35,BLACK);
   tft.setCursor(10,10);
   tft.setTextColor(CYAN);
   tft.setTextSize(5);
   adcRaw = analogRead(A5);
   volt = adcRaw * 5.0 / 1023.0;
   tft.print(volt);
   
   tft.setCursor(140,10);
   tft.setTextColor(WHITE);
   tft.setTextSize(5);
   tft.print("V");
   
   tft.drawRect(0,80,319,130,MAGENTA);
   tft.drawCircle(x, map(adcRaw, 0, 1023, 200, 100), 2, GREEN);
   x=x+3;
   if(x>318){
      x=1;
      tft.fillRect(1,90,318,110,BLACK);
   }
   
   delay(200);  
}

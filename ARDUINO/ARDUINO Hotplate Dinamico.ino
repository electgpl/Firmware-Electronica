#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET     4
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define TSETPOINT A0
#define SCLK      10
#define CS        11
#define MISO      12
#define LED_R     4
#define LED_G     3
#define FAN       2
#define TRIAC     6
float tempSet    = 0;
float tempK      = 0;
int   valorPWM   = 0;
float hysteresis = 2.0;
//-----------------------------------------------------------
// Setup
//-----------------------------------------------------------
void setup(){
   Serial.begin(9600);
   pinMode(CS, OUTPUT);
   pinMode(SCLK, OUTPUT);
   pinMode(MISO, INPUT);
   digitalWrite(CS, HIGH);
   pinMode(TRIAC, OUTPUT);
   pinMode(LED_R, OUTPUT);
   pinMode(LED_G, OUTPUT);
   pinMode(FAN, OUTPUT);
   display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
   display.setTextColor(SSD1306_WHITE); 
   display.clearDisplay();
   display.setCursor(0,0);
   display.setTextSize(2);
   display.println(" HotPlate ");
   display.println("Histeresis");
   display.println(" Electgpl ");
   display.write(2);
   display.display();
   delay(2000);
   display.clearDisplay();
   display.display();
   delay(2000);
}
//-----------------------------------------------------------
// Lee SPI por firmware
//-----------------------------------------------------------
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
//-----------------------------------------------------------
// Lee el MAX6675 y Termocupla
//-----------------------------------------------------------
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
//-----------------------------------------------------------
// Actualiza Display
//-----------------------------------------------------------
void displayUpdate(){
   Serial.print("tempSet: ");
   Serial.print(tempSet);
   Serial.print(" | tempK: ");
   Serial.print(tempK); 
   Serial.print(" | PWM: ");
   Serial.println(valorPWM); 
   display.clearDisplay();
   display.setTextSize(3);
   display.setCursor(5,5);
   display.println("T: ");
   display.setCursor(40,5);
   display.print(tempK,0); 
   display.write(247);
   display.setCursor(5,35);
   display.println("S: ");
   display.setCursor(40,35);
   display.print(tempSet,0);
   display.write(247);
   display.display();  
}
//-----------------------------------------------------------
// Controla la salida TRIAC + Histeresis
//-----------------------------------------------------------
void ajustaTRIAC(){
   if(tempSet >= (tempK + hysteresis)){
      digitalWrite(TRIAC, HIGH);
      delay(50);
      digitalWrite(TRIAC, LOW);
      delay(200);
   }
   if(tempSet >= (tempK + hysteresis)*1.5){
      digitalWrite(TRIAC, HIGH);
      delay(100);
      digitalWrite(TRIAC, LOW);
      delay(200);
   } 
   if(tempSet <= (tempK - hysteresis)){
      digitalWrite(TRIAC, LOW);
   }
}
//-----------------------------------------------------------
// Controla la salida TRIAC + Histeresis
//-----------------------------------------------------------
void perfisfericos(){
   if(tempSet<10)
      digitalWrite(FAN, HIGH);
   else 
      digitalWrite(FAN, LOW);
   if(tempK<40){
      digitalWrite(LED_R, LOW); 
      digitalWrite(LED_G, HIGH); 
   }else{
      digitalWrite(LED_R, HIGH);
      digitalWrite(LED_G, LOW); 
   }
}
//-----------------------------------------------------------
// Funcion Repetitiva
//-----------------------------------------------------------
void loop(){
   tempSet = map(analogRead(TSETPOINT),0,1023,0,300);
   tempK = MAX6675_read();
   perfisfericos();
   delay(300);
   ajustaTRIAC();
   displayUpdate();
}

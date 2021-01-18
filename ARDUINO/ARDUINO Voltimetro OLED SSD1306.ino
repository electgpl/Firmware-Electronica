#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET     4
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define SAMPLES 10
int raw=0;
int ADCvRef=0;

int vRefADC(){
   long result;
   // Lee 1.1V de referencia interna respecto a Vdd
   ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
   delay(2); //Espera a que Vdd se estabilice.
   ADCSRA |= _BV(ADSC);
   while (bit_is_set(ADCSRA,ADSC));
   result = ADCL;
   result |= ADCH<<8;
   result = 1125300L / result; //Recalcula la tension en mV
   return result;
}

void setup(){
   display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
   display.setTextColor(SSD1306_WHITE);
   display.clearDisplay();
   display.setTextSize(1);
   display.setCursor(0,0); 
   display.print(F("Voltimetro ELECTGPL")); 
   display.setCursor(0,20); 
   display.print(F("Tension de Fuente")); 
   ADCvRef = vRefADC();
   display.setCursor(0,30);
   display.print(F("ADCvRef:"));
   display.setCursor(50,30);
   display.print(ADCvRef);
   display.print(F("mV"));
   display.setCursor(80,30);
   display.display();
   delay(4000);
   display.clearDisplay();
   display.drawRect(0, 0, 128, 64, SSD1306_WHITE);
   display.display();
   display.drawLine(0, 32, 127, 32, SSD1306_WHITE);
   display.setTextSize(2);
   display.setCursor(10,9); 
   display.print(F("Voltmeter")); 
   display.display();
}

void loop(){
   display.setCursor(15,41); 
   for(int i=0;i<SAMPLES;i++){
      raw=raw+analogRead(A0);
      delay(20);
   }
   raw=raw/SAMPLES;
   display.print((float)raw*ADCvRef/1023.0/1000.0,4); 
   display.setCursor(100,41); 
   display.print(F("V")); 
   display.display();
   display.fillRect(15, 41, 80, 20, SSD1306_BLACK);
}

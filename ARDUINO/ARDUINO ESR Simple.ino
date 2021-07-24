#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define DISCHARGE_PIN 8  //NPN + DIODE
#define PULSE_PIN     9  //VCC + PNP + 1000R
#define ESR_PIN       A0
#define SAMPLES 100

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET     4
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

byte esrSamples;
double esr;
int milliVolts;
int Rs = 1100.0;
int ADCvRef = 0;

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

void setup(void){
   Serial.begin(9600);
   pinMode(ESR_PIN, INPUT);
   pinMode(PULSE_PIN, OUTPUT);
   digitalWrite(DISCHARGE_PIN,HIGH);
   display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
   display.setTextColor(SSD1306_WHITE); 
   display.clearDisplay();
   display.setCursor(0,0);
   display.setTextSize(2);
   display.println(" Medidor ");
   display.println("   ESR   ");
   display.println("Eelectgpl");
   display.write(2);
   display.display();
   delay(2000);
   ADCvRef = vRefADC();
   Serial.print("ADCvRef:  ");
   Serial.println(ADCvRef);
   display.clearDisplay();
   display.setCursor(0,0);
   display.setTextSize(2);
   display.println("Vref");
   display.println(ADCvRef);
   display.display();
   delay(2000);
}

void loop(void){
   digitalWrite(DISCHARGE_PIN, HIGH);
   delayMicroseconds(600);
   digitalWrite(DISCHARGE_PIN, LOW);
   digitalWrite(PULSE_PIN,LOW);
   delayMicroseconds(5);
   for(int i=0; i<SAMPLES; i++){
      esrSamples = esrSamples + analogRead(ESR_PIN); 
   }
   esrSamples = esrSamples / SAMPLES;
   digitalWrite(PULSE_PIN, HIGH);
   milliVolts = (esrSamples * (float)ADCvRef) / 1023.0;
   esr = Rs / (((float)ADCvRef / milliVolts) - 1); //voltage divider (R2=R1(U2/(U1-U2)))
   display.clearDisplay();
   display.setTextSize(3);
   display.setCursor(20,20);
   if(esr<20){
      Serial.println(esr,1);
      display.print(esr,1); 
      display.write(233);
   }else{
      Serial.println("OL");
      display.print("OL"); 
   }
   Serial.println();
   display.display();  
   delay(100);
}

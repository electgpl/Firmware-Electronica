#include <Wire.h>
#include <Adafruit_MCP4725.h>
#include<LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);
Adafruit_MCP4725 dac;
//Variables para multitarea
int periodo1 = 5;
int periodo2 = 500;
unsigned long TiempoAhora1 = 0;
unsigned long TiempoAhora2 = 0;
//Variables para mediciones
int dacVal = 0;
float pVal = 0;
float vVal = 0;
float iVal = 0;
float tVal = 0;
//Variable para referencia interna de Arduino
float ADCvRef = 0;
//Macros para curva segura de IRFZ44N
#define PMAX 300
#define VMAX 50
#define IMAX 30
#define TMAX 50
//Macro para promedio de ADC
#define MUESTRAS 50

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

void isrUp(void){
   if(pVal<PMAX){
      if(iVal<IMAX){
         if(vVal<VMAX){
            if(dacVal<50)
               dacVal++;
         }
      }
   }
}

void isrDown(void){
   if(dacVal>0)
      dacVal--;
}

void update(void){
   int i;
   for(i=0;i<MUESTRAS;i++){
      iVal = iVal + analogRead(A0)*ADCvRef/1024.0;
      delay(10);
   }
   for(i=0;i<MUESTRAS;i++){
      vVal = vVal + (analogRead(A1)*ADCvRef/1024.0)*11.0;
      delay(10);
   }
   for(i=0;i<MUESTRAS;i++){
      tVal = tVal + (analogRead(A2)*ADCvRef/1024.0)*100.0;
      delay(10);
   }
   iVal = iVal/MUESTRAS;
   vVal = vVal/MUESTRAS;
   tVal = tVal/MUESTRAS;
   pVal = iVal * vVal;
   if(tVal<TMAX)
      dac.setVoltage(map(dacVal,0,50,0,4095), false);
   else
      dac.setVoltage(1, false);
}

void lcdValue(void){
   lcd.setCursor(0, 0);
   lcd.print("I: ");
   lcd.setCursor(3, 0);
   lcd.print(iVal);
   lcd.setCursor(7, 0);
   lcd.print(" ");
   lcd.setCursor(8, 0);
   lcd.print("V: ");
   lcd.setCursor(11, 0);
   lcd.print(vVal);
   lcd.setCursor(15, 0);
   lcd.print(" ");
   lcd.setCursor(0, 1);
   lcd.print("P: ");
   lcd.setCursor(3, 1);
   lcd.print(pVal);
   lcd.setCursor(7, 1);
   lcd.print(" ");
   lcd.setCursor(8, 1);
   lcd.print("T: ");
   lcd.setCursor(11, 1);
   lcd.print(tVal);
   lcd.setCursor(15, 1);
   lcd.print(" ");
}

void serialDebug(void){
   Serial.print("I: ");
   Serial.print(iVal);
   Serial.print(" | V: ");
   Serial.print(vVal);
   Serial.print(" | P: ");
   Serial.print(pVal);
   Serial.print(" | T: ");
   Serial.println(tVal);
}

void setup(){
   lcd.begin();
   lcd.backlight();
   lcd.clear();
   lcd.setCursor(0, 0);
   lcd.print("Carga Electronic");
   lcd.setCursor(0, 1);
   lcd.print("Electgpl");
   Serial.begin(9600);
   dac.begin(0x62);
   pinMode(2, INPUT_PULLUP);
   pinMode(3, INPUT_PULLUP);
   attachInterrupt(digitalPinToInterrupt(2), isrUp, FALLING); 
   attachInterrupt(digitalPinToInterrupt(3), isrDown, FALLING);
   dac.setVoltage(1, false);
   delay(1500);
   lcd.clear();
   lcd.setCursor(0, 0);
   lcd.print("Tension Fuente");
   Serial.println("Midiendo Tension de Fuente");
   ADCvRef = vRefADC()/1000.0;
   Serial.print("ADCvRef:  ");
   Serial.println(ADCvRef);
   lcd.setCursor(0, 1);
   lcd.print(ADCvRef);
   delay(1500);
   lcd.clear();
}

void loop(){
   if(millis() - TiempoAhora1 > periodo1){
      TiempoAhora1 = millis();
      update();
   }
   if(millis() - TiempoAhora2 > periodo2){
      TiempoAhora2 = millis();
      serialDebug();
      lcdValue();
   }
}

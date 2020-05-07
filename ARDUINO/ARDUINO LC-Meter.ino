#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);
#define outImpulso 2
#define inImpulso  3
#define cargaCap   4
#define descaCap   5
#define mideCap   A0
unsigned long t = 0;
unsigned long iniPeriodo = 0;
unsigned long transcurrido = 0;
float Lx  = 0;
float Cx  = 0;
float fo  = 0;
float cap = 0.000000883;   //Valor del capacitor en paralelo a Lx (Ejemplo 883nF)
float res = 22060.0;       //Valor de la resistencia de carga para Cx (Ejemplo 22k)
float pi  = 3.14159;

float inductancia(void){
   digitalWrite(outImpulso, HIGH);
   delay(1);
   digitalWrite(outImpulso, LOW);
   t = pulseIn(inImpulso, LOW);
   fo = 1000000 / (2 * t);
   Lx = (1 / (4 * pow(pi,2) * pow(fo,2) * cap)) * 1000000;
   return Lx;
}

float capacitancia(void){
   digitalWrite(cargaCap, HIGH);  
   iniPeriodo = micros();    1050
   while(analogRead(mideCap) < 645){}
   transcurrido = micros() - iniPeriodo;
   Cx = ((float)transcurrido / res) * 1000.0;
   digitalWrite(cargaCap, LOW);            
   pinMode(descaCap, OUTPUT);            
   digitalWrite(descaCap, LOW);    
   while(analogRead(mideCap) > 10){}     
   pinMode(descaCap, INPUT);
   return Cx;
}

void setup(){
   lcd.begin();
   lcd.backlight();
   lcd.clear();
   lcd.setCursor(0, 0);
   lcd.print("   Medidor LC   ");
   lcd.setCursor(0, 1);
   lcd.print("    Electgpl    ");
   pinMode(inImpulso, INPUT);
   pinMode(descaCap, INPUT);
   pinMode(outImpulso, OUTPUT);
   pinMode(cargaCap, OUTPUT); 
   delay(2000);
   lcd.clear();
}

void loop(){
   if(inductancia() < 10){
      lcd.setCursor(7, 0);
      lcd.print("  ");
   }else{
      if(inductancia() < 100){
         lcd.setCursor(8, 0);
         lcd.print(" ");
      }
   }
   lcd.setCursor(0, 0);
   lcd.print("Ind.: ");
   lcd.setCursor(6, 0);
   lcd.print(inductancia(),0);
   lcd.setCursor(14, 0);
   lcd.print("uH");
   if(capacitancia() < 1000){
      if(capacitancia() < 10){
         lcd.setCursor(7, 1);
         lcd.print("  ");
      }else{
         if(capacitancia() < 100){
            lcd.setCursor(8, 1);
            lcd.print(" ");
         }
      }
      lcd.setCursor(0, 1);
      lcd.print("Cap.: ");
      lcd.setCursor(6, 1);
      lcd.print(capacitancia(),0);
      lcd.setCursor(14, 1);
      lcd.print("nF");
   }else{
      if(capacitancia() < 10){
         lcd.setCursor(7, 1);
         lcd.print("  ");
      }else{
         if(capacitancia() < 100){
            lcd.setCursor(8, 1);
            lcd.print(" ");
       }
      }
      lcd.setCursor(6, 1);
      lcd.print("        ");
      lcd.setCursor(0, 1);
      lcd.print("Cap.: ");
      lcd.setCursor(6, 1);
      lcd.print(capacitancia() / 1000.0,0);
      lcd.setCursor(14, 1);
      lcd.print("uF");
   }
}

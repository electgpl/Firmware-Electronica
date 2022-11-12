/*
   ESTE PROGRAMA REALIZA LA MEDICION DE CAPACITORES DE BAJO VALOR
   DEL ORDEN DE LOS PICO FARADIOS.
   EL RANGO DE MEDICION PROBADO ES DESDE 1pF hasta 1nF.
   LOS PINES DE MEDICION SON EL A0 y A3 (EL CAPACITOR DEBE CONECTARSE EN ELLOS).
   SE UTILIZA UN DISPLAY LCD DE 16x2 CARACTERES POR I2C Y SALIDA SERIAL.
*/
#include<LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);
const int OUT_PIN = A3;
const int IN_PIN = A0;
float acumulado=0;
float actualizado=0;
void setup(){
   Serial.begin(9600);
   pinMode(OUT_PIN, OUTPUT);
   pinMode(IN_PIN, OUTPUT);
   lcd.begin();
   lcd.backlight();
   lcd.clear();
   lcd.setCursor(0, 0);
   lcd.print("PICO CAPACIMETRO");
   lcd.setCursor(0, 1);
   lcd.print("    Electgpl    ");
   delay(2000);
   lcd.clear();
}
void loop(){
   for(int i=0; i<100; i++){
      pinMode(IN_PIN, INPUT);
      digitalWrite(OUT_PIN, HIGH);
      int val = analogRead(IN_PIN);
      digitalWrite(OUT_PIN, LOW);
      pinMode(IN_PIN, OUTPUT);
      float capacitancia = (float)val * 24.5 / (float)(1023 - val);
      acumulado = capacitancia + acumulado;
   }  
   actualizado = acumulado/100;
   acumulado=0;
   Serial.print("Valor en pF: ");
   Serial.println(actualizado);
   lcd.setCursor(0, 0);
   lcd.print("Capacidad[pf]:  ");
   lcd.setCursor(6, 1);
   lcd.print(actualizado);
   delay(1000);
   lcd.setCursor(0, 1);
   lcd.print("                ");
   delay(100);
}
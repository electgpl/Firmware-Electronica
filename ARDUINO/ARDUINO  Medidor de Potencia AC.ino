#include <math.h>
#include <LiquidCrystal.h>
LiquidCrystal lcd(7, 8, 9, 10, 11, 12); //( RS, EN, d4, d5, d6, d7)
#define CONST_CORRIENTE 2
#define CONST_TENSION   530
int i, adcZ=0;
float adc1=0, valorMax1=0, adc2=0, valorMax2=0;
float tension, corriente, potencia;
void setup(){
   lcd.begin(16, 2);
   lcd.clear();
}
void loop(){
   do{
      adcZ=analogRead(A0)-512;
      delayMicroseconds(20);
   }while(adcZ<10);
   for(i=0;i<500;i++){
      adc1=(analogRead(A0)*5.0/1023.0)-2.5;
      delayMicroseconds(33);
      valorMax1=adc1*adc1+valorMax1;
      adc2=(analogRead(A1)*5.0/1023.0)-2.5;
      delayMicroseconds(33);
      valorMax2=adc2*adc2+valorMax2;
   }  
   tension=sqrt(valorMax1/500)*CONST_TENSION;
   corriente=sqrt(valorMax2/500)*CONST_CORRIENTE;
   potencia=tension*corriente;
   delay(2000);
   if(tension<50.0||corriente<0.03){
      lcd.setCursor(0,0);
      lcd.print("     W:000.0    ");
      lcd.setCursor(0,1);
      lcd.print("V:000.0   A:0.00");
   }else{
      lcd.setCursor(5,0);
      lcd.print("W:");
      lcd.setCursor(7,0);
      lcd.print(potencia);
      lcd.setCursor(0,1);
      lcd.print("V:");
      lcd.setCursor(2,1);
      lcd.print(tension);
      lcd.setCursor(10,1);
      lcd.print("A:");
      lcd.setCursor(12,1);
      lcd.print(corriente);
   }
   valorMax1=0;
   valorMax2=0;
}
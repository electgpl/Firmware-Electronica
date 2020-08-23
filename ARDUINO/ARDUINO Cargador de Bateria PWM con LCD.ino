#include<LiquidCrystal.h>
const int rs = 9, en = 8, d4 = 3, d5 = 4, d6 = 6, d7 = 7;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
//-----------------------------------------------------------
//Pines del Arduino
//-----------------------------------------------------------
#define VSENSE A0
#define ISENSE A1
#define VADJ   A2
#define IADJ   A3
#define PWM     5
//-----------------------------------------------------------
//Variables y Constantes de ADC
//-----------------------------------------------------------
#define MUESTRAS 300
int RawValue= 0;
int ADCvRef = 0;
//-----------------------------------------------------------
//Variables y Constantes de Medicion de Corriente
//-----------------------------------------------------------
double tensionAmp = 0;
double amperes = 0;
int i = 0;
float corriente = 0;
float r_shunt = 1.0;
int   corrienteSet = 0;
//-----------------------------------------------------------
//Variables y Constantes de PWM
//-----------------------------------------------------------
int valorPWM = 0;
//-----------------------------------------------------------
//Variables y Constantes de Medicion de Tension
//-----------------------------------------------------------
double volts = 0;
double tensionBAT = 0;
float tension = 0;
float divisor = 0.312;
float v_diodo = 0.5;
float v_shunt = 0;
int tensionSet = 0;
int tensionSetAux = 0;
//-----------------------------------------------------------
//Funcion Setup de Configuracion
//-----------------------------------------------------------
void setup(){
   lcd.begin(16, 2);
   lcd.clear();
   Serial.begin(9600);
   ADCvRef = vRefADC();
   Serial.print("Tension de Alimentacion:  ");
   Serial.print(ADCvRef);
   Serial.println(" mV");
   lcd.setCursor(0, 0);
   lcd.print("Cargador Baterias");
   lcd.setCursor(0, 1);
   lcd.print("Vref: ");
   lcd.setCursor(6, 1);
   lcd.print(ADCvRef);
   lcd.setCursor(11, 1);
   lcd.print("mV");
   delay(2000);
   pinMode(PWM, OUTPUT);
   valorPWM=50;
   analogWrite(PWM, valorPWM);
   delay(500);
   lcd.clear();
}
//-----------------------------------------------------------
//Funcion Retetitiva
//-----------------------------------------------------------
void loop(){
   mideTensionBat();
   Serial.print("V_BAT: ");
   Serial.println(tension,3);
   lcd.setCursor(0, 0);
   lcd.print("V: ");
   lcd.setCursor(3, 0);
   lcd.print(tension);
  
   mideTensionSet();
   lcd.setCursor(0, 1);
   lcd.print("Vs: ");
   if(tensionSet < 10){
      lcd.setCursor(4, 1);
      lcd.print(" ");     
      lcd.setCursor(5, 1);
      lcd.print(tensionSet);
   }else{
      lcd.setCursor(4, 1);
      lcd.print(tensionSet);
   }
  
   mideCorrienteCarga();
   Serial.print("I_BAT: ");
   Serial.println(corriente,3);
   lcd.setCursor(9, 0);
   lcd.print("I: ");
   lcd.setCursor(12, 0);
   lcd.print(corriente);
  
   mideCorrienteSet();
   lcd.setCursor(9, 1);
   lcd.print("Is: ");
   lcd.setCursor(14, 1);
   lcd.print(corrienteSet);

   ajustaPWMcargador();
   Serial.println(" ");
   Serial.println(" ");
}
//-----------------------------------------------------------
//Funcion de calculo de tension de alimentacion
//-----------------------------------------------------------
int vRefADC(){
   long result;
   ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
   delay(2);
   ADCSRA |= _BV(ADSC);
   while (bit_is_set(ADCSRA,ADSC));
   result = ADCL;
   result |= ADCH<<8;
   result = 1125300L / result;
   return result;
}
//-----------------------------------------------------------
//Funcion de medicion de corriente
//-----------------------------------------------------------
float mideCorrienteCarga(){
   amperes = 0;
   for(i=0;i<MUESTRAS;i++){
      RawValue = analogRead(ISENSE);
      tensionAmp = (RawValue / 1023.0) * ADCvRef;
      amperes = amperes + tensionAmp;
   }
   corriente = ((amperes / MUESTRAS) / 1000.0) / r_shunt;
   return corriente;
}
//-----------------------------------------------------------
//Funcion de seteo de corriente
//-----------------------------------------------------------
float mideCorrienteSet(){
   amperes = 0;
   for(i=0;i<MUESTRAS;i++){
      RawValue = analogRead(IADJ);
      tensionAmp = (RawValue / 1023.0) * ADCvRef;
      amperes = amperes + tensionAmp;
   }
   corrienteSet = (amperes / MUESTRAS) / 1000.0;
   return corrienteSet;
}
//-----------------------------------------------------------
//Funcion de medicion de Tension con divisor
//-----------------------------------------------------------
float mideTensionBat(){
   volts = 0;
   for(i=0;i<MUESTRAS;i++){
      RawValue = analogRead(VSENSE);
      tensionBAT = (RawValue / 1023.0) * ADCvRef;
      volts = volts + tensionBAT;
   }
   tension = (((volts / MUESTRAS) / 1000.0) / divisor) - v_diodo - v_shunt;
   return tension;
}
//-----------------------------------------------------------
//Funcion de seteo de tension
//-----------------------------------------------------------
float mideTensionSet(){
   volts = 0;
   for(i=0;i<MUESTRAS;i++){
      RawValue = analogRead(VADJ);
      tensionBAT = (RawValue / 1023.0) * ADCvRef;
      volts = volts + tensionBAT;
   }
   tensionSetAux = (volts / MUESTRAS) / 1000.0;
   tensionSet = map(tensionSetAux, 0, 5, 4, 15);
   return tensionSet;
}
//-----------------------------------------------------------
//Controla el PWM en funcion de la corriente de carga
//-----------------------------------------------------------
void ajustaPWMcargador(){
   if(corriente < corrienteSet){
      if(tension < tensionSet){
         if(valorPWM<255)
            valorPWM++;
         delay(1);
         analogWrite(PWM, valorPWM);
         Serial.print("PWM: ");
         Serial.println(valorPWM); 
      } 
      else{
         if(valorPWM>0)
            valorPWM--;
         delay(1);
         analogWrite(PWM, valorPWM);
         Serial.print("PWM: ");
         Serial.println(valorPWM); 
      }
   }
   else{
      Serial.println("Limite");
   }
}   

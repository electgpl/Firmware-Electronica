//-----------------------------------------------------------
//Pines del Arduino
//-----------------------------------------------------------
#define VSENSE A0
#define ISENSE A1
#define PWM 5
//-----------------------------------------------------------
//Variables y Constantes de ADC
//-----------------------------------------------------------
#define MUESTRAS 200
int RawValue= 0;
int ADCvRef = 0;
//-----------------------------------------------------------
//Variables y Constantes de Medicion de Corriente
//-----------------------------------------------------------
#define I_MAX 0.5
double tensionAmp = 0;
double amperes = 0;
int i = 0;
float corriente = 0;
float r_shunt = 1.0;
//-----------------------------------------------------------
//Variables y Constantes de PWM
//-----------------------------------------------------------
int valorPWM = 0;
//-----------------------------------------------------------
//Variables y Constantes de Medicion de Tension
//-----------------------------------------------------------
#define V_MAX 14.4
double volts = 0;
double tensionBAT = 0;
float tension = 0;
float divisor = 0.312;
float v_diodo = 0.5;
float v_shunt = 0;
//-----------------------------------------------------------
//Funcion Setup de Configuracion
//-----------------------------------------------------------
void setup(){
   Serial.begin(9600);
   ADCvRef = vRefADC();
   Serial.print("Tension de Alimentacion:  ");
   Serial.print(ADCvRef);
   Serial.println(" mV");
   delay(2000);
   pinMode(PWM, OUTPUT);
   valorPWM=50;
   analogWrite(PWM, valorPWM);
   delay(500);
}
//-----------------------------------------------------------
//Funcion Retetitiva
//-----------------------------------------------------------
void loop(){
   mideCorrienteCarga();
   Serial.print("I_BAT: ");
   Serial.println(corriente,3);
   mideTensionBat();
   Serial.print("V_BAT: ");
   Serial.println(tension,3);
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
//Funcion de medicion con el sensor
//-----------------------------------------------------------
float mideCorrienteCarga(){
   amperes = 0;
   for(i=0;i<MUESTRAS;i++){
      RawValue = analogRead(ISENSE);
      tensionAmp = (RawValue / 1023.0) * ADCvRef;
      amperes = amperes + tensionAmp;
      delay(1);
   }
   corriente = ((amperes / MUESTRAS) / 1000.0) / r_shunt;
   return corriente;
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
      delay(1);
   }
   tension = (((volts / MUESTRAS) / 1000.0) / divisor) - v_diodo - v_shunt;
   return tension;
}
//-----------------------------------------------------------
//Controla el PWM en funcion de la corriente de carga
//-----------------------------------------------------------
void ajustaPWMcargador(){
   if(corriente < I_MAX){
      if(tension < V_MAX){
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

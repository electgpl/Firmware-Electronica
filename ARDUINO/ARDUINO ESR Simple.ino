#define DISCHARGE_PIN 8  //NPN + DIODE
#define PULSE_PIN     9  //VCC + PNP + 100R
#define ESR_PIN       A0
#define SAMPLES 100

byte esrSamples;
double esr;
int milliVolts;
int Rs = 1100.0;
int Rm;
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
   ADCvRef = vRefADC();
   Serial.print("ADCvRef:  ");
   Serial.println(ADCvRef);
}

void loop(void){
   digitalWrite(DISCHARGE_PIN, HIGH);
   delayMicroseconds(600);
   digitalWrite(DISCHARGE_PIN, LOW);
   digitalWrite(PULSE_PIN,LOW);
   delayMicroseconds(5);
   esrSamples = analogRead(ESR_PIN); 
   digitalWrite(PULSE_PIN, HIGH);
   milliVolts = (esrSamples * 4593.0) / 1023.0;
   Rm = Rs / ((4593.0 / milliVolts) - 1); //voltage divider (R2=R1(U2/(U1-U2)))
   for(int i=0; i<SAMPLES; i++){
      esr = esr + Rm; 
   }
   esr = esr / SAMPLES;
   if(esr<20){
      Serial.println(esr,1);
   }else{
      Serial.println("OL");
   }
   Serial.println();
   delay(100);
}

float aFwd, aRef, vFwd, vRef, vSWR, vS11;
int ADCvRef = 0;

void setup(){
   Serial.begin(9600);
   ADCvRef = vRefADC();
   Serial.print("ADCvRef[mV]:  ");
   Serial.println(ADCvRef);
}

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

void loop(){
   aFwd = analogRead(A0);
   aRef = analogRead(A1);
   vFwd = aFwd * ADCvRef / 1023.0;
   vRef = aRef * ADCvRef / 1023.0;
   vSWR = (aFwd+aRef)/(aFwd-aRef);
   vS11 = 20*log10((vSWR-1)/(vSWR+1));

   Serial.print("vFWD[mV]: "); Serial.print(vFwd);
   Serial.print(" | "); 
   Serial.print("vREF[mV]: "); Serial.print(vRef);  
   Serial.print(" | "); 
   Serial.print("SWR: "); Serial.print(vSWR,2); Serial.print(":1");
   Serial.print(" | "); 
   Serial.print("S11[dB]: "); Serial.println(vS11);
 
   delay(300);
}

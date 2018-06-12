int ADCvRef = 0;

void setup(){ 
   Serial.begin(9600);
   Serial.println("Midiendo Tension de Fuente");
   ADCvRef = vRefADC();
   Serial.print("ADCvRef:  ");
   Serial.println(ADCvRef);
}

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

void loop(){
}

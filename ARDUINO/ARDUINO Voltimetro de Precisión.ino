//Milivoltimetro de presicion con ADC interno de 10bit.
//Variables y Constantes de Calibracion ADC
#define MUESTRAS 100
const int analogIn = A0;
long ADCvRef = 0;
int rawValue = 0;
int j = 0;
float relativo = 0;
float cero = 0;
//Variables y Constantes de Medicion para ACS712
float tension = 0;
int i = 0;
//Funcion de configuracion
//Calculo de tension de alimentacion
//Calculo de cero relativo
void setup(){ 
   Serial.begin(9600);
   vRefADC();
   Serial.print("Tension de Alimentacion:  ");
   Serial.print((float)ADCvRef);
   Serial.println(" mV");
   relativoACS();
   delay(2000);
}
//Funcion repetitiva de medicion
void loop(){
   tension = 0;
   for(i=0;i<MUESTRAS;i++){
      rawValue = analogRead(analogIn);
      tension = tension + ((rawValue / 1023.0) * ADCvRef) - relativo;
      delay(10);
   }
   Serial.print("Tension Medida en ADC: ");
   Serial.print(tension/MUESTRAS);
   Serial.println(" mV");
   delay(500);
}
//Funcion de calculo de tension de alimentacion
int vRefADC(){
   ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
   delay(2);
   ADCSRA |= _BV(ADSC);
   while (bit_is_set(ADCSRA,ADSC));
   ADCvRef = ADCL;
   ADCvRef |= ADCH<<8;
   ADCvRef = 1125300L / ADCvRef;
   return ADCvRef;
}
//Funcion de calculo de cero relativo
float relativoACS(){
   cero = 0;
   for(j=0;j<MUESTRAS;j++){
      rawValue = analogRead(analogIn);
      cero = cero + ((rawValue / 1023.0) * ADCvRef);
      delay(25);
   }
   cero = cero / MUESTRAS;
   relativo = cero;
   Serial.print("Relativizando a cero: ");
   Serial.print(relativo);
   Serial.println(" mV");
   return relativo;
}

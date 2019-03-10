//Filtro Media Móvil como Pasa Bajos
//An=a*M+(1-a)*An
//alpha 1: Sin filtro
//alpha 0: Filtrado totalmente
//alpha clásico 0.05

int adc_filtrado = 0;
int adc_raw = 0;
#define alpha 0.05

void setup(){
   Serial.begin(9600);
}

void loop(){
   adc_raw = analogRead(A0);
   adc_filtrado = (alpha*adc_raw) + ((1-alpha)*adc_filtrado);
   Serial.print(adc_raw);
   Serial.print(",");  
   Serial.println(adc_filtrado);
}

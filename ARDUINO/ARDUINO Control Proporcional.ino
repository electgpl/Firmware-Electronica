int potenciometro = 0;
int voltimetro = 0;
int valorPWM = 0;

void setup(){
   pinMode(9, OUTPUT);
   pinMode(LED_BUILTIN, OUTPUT);
}

void loop(){
   digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  
   voltimetro    = analogRead(A0);
   potenciometro = analogRead(A1);

   if(potenciometro>voltimetro){
      valorPWM = map(potenciometro, 0, 1023, 0, 255);
      analogWrite(9, valorPWM);
      delay(300);
   }
   
   if(potenciometro<voltimetro){
      valorPWM = map(potenciometro, 0, 1023, 0, 255);
      analogWrite(9, valorPWM);
      delay(300);
   }

   delay(100);
}

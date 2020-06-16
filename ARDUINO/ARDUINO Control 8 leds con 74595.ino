int pinData  = 2;
int pinLatch = 3;
int pinClock = 4;

void ledWrite(int Led){
   shiftOut(pinData, pinClock, LSBFIRST, Led);
   digitalWrite(pinLatch, HIGH);
   digitalWrite(pinLatch, LOW);
}

void setup(){
   pinMode(pinData, OUTPUT);
   pinMode(pinLatch, OUTPUT);
   pinMode(pinClock, OUTPUT);
}

void loop(){
   int valorADC = analogRead(A0);
   if(valorADC > 1000)
      ledWrite(B11111111);
   else 
      if(valorADC > 900)
         ledWrite(B11111110);
      else
         if(valorADC > 800)
            ledWrite(B11111100);
         else 
            if(valorADC > 700)
               ledWrite(B11111000);
            else 
               if(valorADC > 600)
                  ledWrite(B11110000);
               else 
                  if(valorADC > 500)
                     ledWrite(B11100000);
                  else 
                     if(valorADC > 400)
                        ledWrite(B11000000);
                     else 
                        if(valorADC > 300)
                           ledWrite(B10000000);
                        else 
                           if(valorADC > 200)
                              ledWrite(B00000000);

}

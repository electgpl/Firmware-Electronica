unsigned long previousMicros = 0;
volatile int interval = 1000;
int i=500, j=0;

void setup(){
   pinMode(7, OUTPUT);
   Serial.begin(115200);
}

void loop(){
   while(i!=0){
      for(j=0;j<10;j++){
         digitalWrite(7, !digitalRead(7));
         delayMicroseconds(interval);
      }
      interval=i;
      Serial.println(analogRead(A0));
      i--;
   }
}

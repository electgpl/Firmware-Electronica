int pinData  = 2;
int pinLatch = 3;
int pinClock = 4;
#define TIEMPO 200

void ledWrite(int RLed, int GLed, int BLed){
   shiftOut(pinData, pinClock, LSBFIRST, BLed);
   shiftOut(pinData, pinClock, LSBFIRST, GLed);
   shiftOut(pinData, pinClock, LSBFIRST, RLed);
   digitalWrite(pinLatch, HIGH);
   digitalWrite(pinLatch, LOW);
}

void setup(){
   pinMode(pinData, OUTPUT);
   pinMode(pinLatch, OUTPUT);
   pinMode(pinClock, OUTPUT);
}

void loop(){
   ledWrite(128,0,0); delay(TIEMPO);
   ledWrite(64,0,0);  delay(TIEMPO);
   ledWrite(32,0,0);  delay(TIEMPO);
   ledWrite(16,0,0);  delay(TIEMPO);
   ledWrite(8,0,0);   delay(TIEMPO);
   ledWrite(4,0,0);   delay(TIEMPO);
   ledWrite(2,0,0);   delay(TIEMPO);
   ledWrite(1,0,0);   delay(TIEMPO);
   ledWrite(0,128,0); delay(TIEMPO);
   ledWrite(0,64,0);  delay(TIEMPO);
   ledWrite(0,32,0);  delay(TIEMPO);
   ledWrite(0,16,0);  delay(TIEMPO);
   ledWrite(0,8,0);   delay(TIEMPO);
   ledWrite(0,4,0);   delay(TIEMPO);
   ledWrite(0,2,0);   delay(TIEMPO);
   ledWrite(0,1,0);   delay(TIEMPO);
   ledWrite(0,0,128); delay(TIEMPO);
   ledWrite(0,0,64);  delay(TIEMPO);
   ledWrite(0,0,32);  delay(TIEMPO);
   ledWrite(0,0,16);  delay(TIEMPO);
   ledWrite(0,0,8);   delay(TIEMPO);
   ledWrite(0,0,4);   delay(TIEMPO);
   ledWrite(0,0,2);   delay(TIEMPO);
   ledWrite(0,0,1);   delay(TIEMPO);
}

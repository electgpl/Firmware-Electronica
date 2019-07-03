#define latchPin  11    //MOSI - PIN 12 de 74595
#define clockPin  13    //SCK  - PIN 11 de 74595
#define dataPin   12    //MISO - PIN 14 de 74595
#define In_1       7
#define In_2       8
#define In_3       9
#define In_4      10
#define Out_1      3
#define Out_2      4
#define Out_3      5
#define Out_4      6
int numWrite    = 0;
int i=0;
const int MUESTRAS = 10;
String comandoLeido;
int timer1_counter;
bool F_LED1_ON = LOW;
bool F_LED2_ON = LOW;
bool F_LED3_ON = LOW;
bool F_LED4_ON = LOW;
bool F_LED5_ON = LOW;
bool F_LED6_ON = LOW;
bool F_LED7_ON = LOW;
bool F_LED8_ON = LOW;
bool toggle = LOW;

void ledWrite(int numLed, boolean stateLed){
   bitWrite(numWrite, numLed, stateLed);
   shiftOut(dataPin, clockPin, MSBFIRST, numWrite); 
   digitalWrite(latchPin, HIGH);
   digitalWrite(latchPin, LOW);
}

void portWrite(int bytePort){
   ledWrite(0, bytePort&B00000001); 
   ledWrite(1, bytePort&B00000010); 
   ledWrite(2, bytePort&B00000100); 
   ledWrite(3, bytePort&B00001000); 
   ledWrite(4, bytePort&B00010000);
   ledWrite(5, bytePort&B00100000);
   ledWrite(6, bytePort&B01000000);
   ledWrite(7, bytePort&B10000000);
}

ISR(TIMER1_OVF_vect){
   TCNT1 = timer1_counter;
   if(F_LED1_ON){ 
      ledWrite(0,toggle); 
   }
   if(F_LED2_ON){ 
      ledWrite(1,toggle); 
   }
   if(F_LED3_ON){ 
      ledWrite(2,toggle); 
   }
   if(F_LED4_ON){ 
      ledWrite(3,toggle);  
   }
   if(F_LED5_ON){ 
      ledWrite(4,toggle); 
   }
   if(F_LED6_ON){ 
      ledWrite(5,toggle); 
   }
   if(F_LED7_ON){ 
      ledWrite(6,toggle); 
   }
   if(F_LED8_ON){ 
      ledWrite(7,toggle); 
   }
   toggle = !toggle;
}

void setup(){
   Serial.begin(4800);
   noInterrupts();           // disable all interrupts
   TCCR1A = 0;
   TCCR1B = 0;               // preload timer (65536-3036)*0,0000000625‬*256=1s > 1Hz
   timer1_counter = 49911;   // preload timer (65536-34286)*0,0000000625‬*256=0.5s > 2Hz
   TCNT1 = timer1_counter;   // preload timer (65536-49911)*0,0000000625‬*256=0.25s > 4Hz
   TCCR1B |= (1 << CS12);    // 256 prescaler 
   TIMSK1 |= (1 << TOIE1);   // enable timer overflow interrupt
   interrupts();             // enable all interrupts
   pinMode(latchPin, OUTPUT);
   pinMode(clockPin, OUTPUT);
   pinMode(dataPin, OUTPUT);
   pinMode(Out_1,  OUTPUT);
   pinMode(Out_2,  OUTPUT);
   pinMode(Out_3,  OUTPUT);
   pinMode(Out_4,  OUTPUT);
   pinMode(In_1, INPUT_PULLUP);
   pinMode(In_2, INPUT_PULLUP); 
   pinMode(In_3, INPUT_PULLUP);
   pinMode(In_4, INPUT_PULLUP); 
   digitalWrite(Out_1, LOW);
   digitalWrite(Out_2, LOW);
   digitalWrite(Out_3, LOW);
   digitalWrite(Out_4, LOW);
   portWrite(B10101010);
   delay(300); 
   portWrite(B01010101);
   delay(300); 
   portWrite(B00000000);
}

void respuestaOK(void){
   Serial.println("OK");
}

void leeAT(void){
   uint32_t bufferSize = 25;
   char charBuf[25];
   char *IncomingData;
   if(Serial.available()){
      comandoLeido=Serial.readStringUntil('\r');
      comandoLeido.toCharArray(charBuf, 25);
      if(charBuf[0] == 'A' && charBuf[1] == 'T' && charBuf[2] == '+'){
         charBuf[bufferSize - 1] = '\0';
         IncomingData = &charBuf[3];
         if(*IncomingData == 'A'){
            if((strcmp(IncomingData, "A1")) == 0){
               Serial.println(analogRead(A6));
            }
            if((strcmp(IncomingData, "A2")) == 0){
               Serial.println(analogRead(A7));     
            }
         }else if(*IncomingData == 'I'){
            if((strcmp(IncomingData, "I1")) == 0){
               Serial.println("I1");
               Serial.println(digitalRead(In_1));
            }
            else if((strcmp(IncomingData, "I2")) == 0){
               Serial.println("I2");
               Serial.println(digitalRead(In_2));
            }
            else if((strcmp(IncomingData, "I3")) == 0){
               Serial.println("I3");
               Serial.println(digitalRead(In_3));
            }
            else if((strcmp(IncomingData, "I4")) == 0){
               Serial.println("I4");
               Serial.println(digitalRead(In_4));
            }
         }else if(*IncomingData == 'O'){
            if((strcmp(IncomingData, "O1ON")) == 0){
               Serial.println("O1ON");
               digitalWrite(Out_1,HIGH);
               respuestaOK();
            }
            else if((strcmp(IncomingData, "O2ON")) == 0){
               Serial.println("O2ON");
               digitalWrite(Out_2,HIGH);
               respuestaOK();
            }
            else if((strcmp(IncomingData, "O3ON")) == 0){
               Serial.println("O3ON");
               digitalWrite(Out_3,HIGH);
               respuestaOK();
            }
            else if((strcmp(IncomingData, "O4ON")) == 0){
               Serial.println("O4ON");
               digitalWrite(Out_4,HIGH);
               respuestaOK();
            }
            else if((strcmp(IncomingData, "O1OFF")) == 0){
               Serial.println("O1OFF");
               digitalWrite(Out_1,LOW);
               respuestaOK();
            }
            else if((strcmp(IncomingData, "O2OFF")) == 0){
               Serial.println("O2OFF");
               digitalWrite(Out_2,LOW);
               respuestaOK();
            }
            else if((strcmp(IncomingData, "O3OFF")) == 0){
               Serial.println("O3OFF");
               digitalWrite(Out_3,LOW);
               respuestaOK();
            }
            else if((strcmp(IncomingData, "O4OFF")) == 0){
               Serial.println("O4OFF");
               digitalWrite(Out_4,LOW);
               respuestaOK();
            }
            else if((strcmp(IncomingData, "OALLOFF")) == 0){
               Serial.println("OALLOFF");
               digitalWrite(Out_1,LOW);
               digitalWrite(Out_2,LOW);
               digitalWrite(Out_3,LOW);
               digitalWrite(Out_4,LOW);
               respuestaOK();
            }
         }else if(*IncomingData == 'F'){
            if((strcmp(IncomingData, "F1ON")) == 0){
               Serial.println("F1ON");
               F_LED1_ON = HIGH;
               respuestaOK();
            }
            else if((strcmp(IncomingData, "F2ON")) == 0){
               Serial.println("F2ON");
               F_LED2_ON = HIGH;
               respuestaOK();
            }
            else if((strcmp(IncomingData, "F3ON")) == 0){
               Serial.println("F3ON");
               F_LED3_ON = HIGH;
               respuestaOK();
            }
            else if((strcmp(IncomingData, "F4ON")) == 0){
               Serial.println("F4ON");
               F_LED4_ON = HIGH;
               respuestaOK();
            }
            else if((strcmp(IncomingData, "F5ON")) == 0){
               Serial.println("F5ON");
               F_LED5_ON = HIGH;
               respuestaOK();
            }
            else if((strcmp(IncomingData, "F6ON")) == 0){
               Serial.println("F6ON");
               F_LED6_ON = HIGH;
               respuestaOK();
            }
            else if((strcmp(IncomingData, "F7ON")) == 0){
               Serial.println("F7ON");
               F_LED7_ON = HIGH;
               respuestaOK();
            }
            else if((strcmp(IncomingData, "F8ON")) == 0){
               Serial.println("F8ON");
               F_LED8_ON = HIGH;
               respuestaOK();
            }
         }else if(*IncomingData == 'L'){
            if((strcmp(IncomingData, "L1ON")) == 0){
               Serial.println("L1ON");
               ledWrite(0,HIGH);
               respuestaOK();
            }
            else if((strcmp(IncomingData, "L2ON")) == 0){
               Serial.println("L2ON");
               ledWrite(1,HIGH);
               respuestaOK();
            }
            else if((strcmp(IncomingData, "L3ON")) == 0){
               Serial.println("L3ON");
               ledWrite(2,HIGH);
               respuestaOK();
            }
            else if((strcmp(IncomingData, "L4ON")) == 0){
               Serial.println("L4ON");
               ledWrite(3,HIGH);
               respuestaOK();
            }
            else if((strcmp(IncomingData, "L5ON")) == 0){
               Serial.println("L5ON");
               ledWrite(4,HIGH);
               respuestaOK();
            }
            else if((strcmp(IncomingData, "L6ON")) == 0){
               Serial.println("L6ON");
               ledWrite(5,HIGH);
               respuestaOK();
            }
            else if((strcmp(IncomingData, "L7ON")) == 0){
               Serial.println("L7ON");
               ledWrite(6,HIGH);
               respuestaOK();
            }
            else if((strcmp(IncomingData, "L8ON")) == 0){
               Serial.println("L8ON");
               ledWrite(7,HIGH);
               respuestaOK();
            }
            else if((strcmp(IncomingData, "L1OFF")) == 0){
               Serial.println("L1OFF");
               ledWrite(0,LOW);
               F_LED1_ON = LOW;
               respuestaOK();
            }
            else if((strcmp(IncomingData, "L2OFF")) == 0){
               Serial.println("L2OFF");
               ledWrite(1,LOW);
               F_LED2_ON = LOW;
               respuestaOK();
            }
            else if((strcmp(IncomingData, "L3OFF")) == 0){
               Serial.println("L3OFF");
               ledWrite(2,LOW);
               F_LED3_ON = LOW;
               respuestaOK();
            }
            else if((strcmp(IncomingData, "L4OFF")) == 0){
               Serial.println("L4OFF");
               ledWrite(3,LOW);
               F_LED4_ON = LOW;
               respuestaOK();
            }
            else if((strcmp(IncomingData, "L5OFF")) == 0){
               Serial.println("L5OFF");
               ledWrite(4,LOW);
               respuestaOK();
            }
            else if((strcmp(IncomingData, "L6OFF")) == 0){
               Serial.println("L6OFF");
               ledWrite(5,LOW);
               respuestaOK();
            }
            else if((strcmp(IncomingData, "L7OFF")) == 0){
               Serial.println("L7OFF");
               ledWrite(6,LOW);
               respuestaOK();
            }
            else if((strcmp(IncomingData, "L8OFF")) == 0){
               Serial.println("L8OFF");
               ledWrite(7,LOW);
               respuestaOK();
            }
            else if((strcmp(IncomingData, "LALLOFF")) == 0){
               Serial.println("LALLOFF");
               portWrite(B00000000);
               F_LED1_ON = LOW;
               F_LED2_ON = LOW;
               F_LED3_ON = LOW;
               F_LED4_ON = LOW;
               F_LED5_ON = LOW;
               F_LED6_ON = LOW;
               F_LED7_ON = LOW;
               F_LED8_ON = LOW;
               respuestaOK();
            }
         }
      }
   }
}

void loop(){
   leeAT();
}

int speakerPin = 12;

int length0 = 27;
char notes0[] = "eeeeeeegcde fffffeeeeddedg";
int beats0[] = { 2,2,4,2,2,4,2,2,2,2,6,2,2,2,4,2,2,2,2,2,2,2,2,2,4,4 };

int length1 = 69;
char notes1[] = "ddaagfedcdefga ddaagfedcdefga avgavCDagfdefgfgavaagfedfedgfgavCDagfed";
int beats1[] = { 2,2,2,2,2,2,2,2,2,2,2,2,2,4,2,2,2,2,2,2,2,2,2,2,2,2,2,2,4,2,2,2,2,2,2,2,2,2,2,2,2,2,2,4,2,2,4,2,2,2,2,2,2,4,1,1,2,4,2,2,2,2,2,2,2,2,2,2,8 };

int length2 = 77;
char notes2[] = "cffefaagCCCvagacffefaagCffeedcCCCfvaagCCCfvaagDDDCvavgavCffgaDDDCvavgavCffgf";
int beats2[] = {2,2,3,1,2,2,2,2,2,2,3,1,2,2,4,2,2,3,1,2,2,2,2,2,2,3,1,2,2,4,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,4,2,1,1,3,1,2,2,4,3,1,2,2,2,2,4,2,1,1,3,1,2,2,4,8 };

int length3 = 64;
char notes3[] = "ggdgadbabCbaggsesgabsedd DCbCbabgasedggsgagdbbabCbabCbagsgCbagg";
int beats3[] = { 2,4,2,2,4,4,2,2,2,2,4,2,2,4,2,2,2,2,2,2,4,3,1,6,2,4,2,2,4,4,2,2,2,2,3,1,2,2,2,2,2,2,4,2,2,2,2,2,2,4,2,2,2,2,2,2,4,2,2,4,3,1,6,8 };


int tempo = 300;

void playTone(int tone, int duration) {
   for(long i = 0; i < duration * 1000L; i += tone * 2) {
      digitalWrite(speakerPin, HIGH);
      delayMicroseconds(tone);
      digitalWrite(speakerPin, LOW);
      delayMicroseconds(tone);
   }
}

void output_leds(char letra){
   switch(letra){
   case 'c':
      PORTD = B00000101;
      break;  
   case 'd':
      PORTD = B10100000;
      break;  
   case 'e':
      PORTD = B00010100;
      break;  
   case 'f':
      PORTD = B00101000;
      break;  
   case 's':
      PORTD = B01100000;
      break;  
   case 'g':
      PORTD = B00000110;
      break;  
   case 'a':
      PORTD = B00011000;
      break;  
   case 'v':
      PORTD = B10101010;
      break;  
   case 'b':
      PORTD = B10101010;
      break;  
   case 'C':
      PORTD = B11000011;
      break;  
   case 'D':
      PORTD = B00111100;
      break;  
   case 'E':
      PORTD = B11001100;
      break;        
   }
}

void playNote(char note, int duration) {
   char names[] = { 'c' , 'd' , 'e' , 'f' , 's' , 'g' , 'a' , 'v' , 'b' , 'C', 'D', 'E' };
   int tones[]  = { 1915, 1700, 1519, 1432, 1352, 1275, 1136, 1073, 1014, 956, 852, 758 };
   for(int i = 0; i < 12; i++) {
      if(names[i] == note) {
         playTone(tones[i], duration);
         output_leds(names[i]);
      }
   }
}

void setup() {
   pinMode(speakerPin, OUTPUT);
   DDRD = B11111111;
   PORTD = B00000000;
}

void loop() {

   for(int i = 0; i < length0; i++) {
      if(notes0[i] == ' ') {
         delay(beats0[i] * tempo); // rest
      }else{
         playNote(notes0[i], beats0[i] * tempo);
      }
      delay(tempo / 2); 
   }
   PORTD = B00000000;
   delay(1000);

   for(int i = 0; i < length1; i++) {
      if(notes1[i] == ' ') {
         delay(beats1[i] * tempo); // rest
      }else{
         playNote(notes1[i], beats1[i] * tempo);
      }
      delay(tempo / 2); 
   }
   PORTD = B00000000;
   delay(1000);
  
   for(int i = 0; i < length2; i++) {
      if(notes2[i] == ' ') {
         delay(beats2[i] * tempo); // rest
      }else{
         playNote(notes2[i], beats2[i] * tempo);
      }
      delay(tempo / 2); 
   }
   PORTD = B00000000;
   delay(1000);

   for(int i = 0; i < length3; i++) {
      if(notes3[i] == ' ') {
         delay(beats3[i] * tempo); // rest
      }else{
         playNote(notes3[i], beats3[i] * tempo);
      }
      delay(tempo / 2); 
   }
   PORTD = B00000000;
   delay(1000);
}

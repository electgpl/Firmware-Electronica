#include<SPI.h>
#include<LiquidCrystal.h>
const int rs = 2, en = 3, d4 = 8, d5 = 5, d6 = 6, d7 = 7;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

//DATA PIN:11, CLOCK PIN:13, FSYNC PIN:4
#define FSYNC 4

#define WAVE_SINE     0x2000
#define WAVE_SQUARE   0x2028
#define WAVE_TRIANGLE 0x2002

#define b_UP    A0
#define b_DOWN  A1
#define b_MULTI A2
#define b_FUNC  A3

long int counter = 50; 
long int counter_ant = 0;
long int function = 0;
long int function_ant = 0;
int multi = 0;
int func = 0;
int func_ant = 0;

void AD9833setup(){
   pinMode(FSYNC, OUTPUT);
   digitalWrite(FSYNC, HIGH);
   SPI.begin();
   delay(50); 
   AD9833reset();
}

void AD9833reset(){
   WriteRegister(0x100);
   delay(10);
}

void AD9833setFrequency(long frequency, int Waveform){
   long FreqWord = (frequency * pow(2, 28)) / 25.0E6;
   int MSB = (int)((FreqWord & 0xFFFC000) >> 14);
   int LSB = (int)(FreqWord & 0x3FFF);
   LSB |= 0x4000;
   MSB |= 0x4000; 
   WriteRegister(0x2100);   
   WriteRegister(LSB);
   WriteRegister(MSB);
   WriteRegister(0xC000);
   WriteRegister(Waveform);
}

void WriteRegister(int dat){ 
   SPI.setDataMode(SPI_MODE2);        
   digitalWrite(FSYNC, LOW);
   delayMicroseconds(10); 
   SPI.transfer(dat>>8);
   SPI.transfer(dat&0xFF);
   digitalWrite(FSYNC, HIGH);
   SPI.setDataMode(SPI_MODE0);
}

void setup(){
   Serial.begin(9600);
   AD9833setup();
   pinMode(b_UP, INPUT_PULLUP);
   pinMode(b_DOWN,INPUT_PULLUP);
   pinMode(b_MULTI,INPUT_PULLUP);
   pinMode(b_FUNC,INPUT_PULLUP);
   lcd.begin(16, 2);
   lcd.clear();
}

void actualiza_func(){
   if(!digitalRead(b_FUNC)){
      delay(50);
      if(!digitalRead(b_FUNC)){ 
         if(func_ant == 0)
            func = 1;
         if(func_ant == 1)
            func = 2;
         if(func_ant == 2)
            func = 0;
         func_ant = func;
      }
   }
   if(func == 0){
      lcd.setCursor(6, 1);
      lcd.print("  SENOIDAL");
      function = WAVE_SINE;
   }
   if(func == 1){
      lcd.setCursor(6, 1);
      lcd.print("TRIANGULAR");
      function = WAVE_TRIANGLE;
   }
   if(func == 2){
      lcd.setCursor(6, 1);
      lcd.print("  CUADRADA");
      function = WAVE_SQUARE;
   }
   if(counter_ant != counter || function_ant != function){
      AD9833setFrequency(counter, function);
   }
   counter_ant = counter;
   function_ant = function;
}

void limpiaDigitos(){
   if(counter < 100){
      lcd.setCursor(2, 0);
      lcd.print("      ");  
   }else{
      if(counter < 1000){
         lcd.setCursor(3, 0);
         lcd.print("     "); 
      }else{
         if(counter < 10000){
            lcd.setCursor(4, 0);
            lcd.print("    "); 
         }else{
            if(counter < 100000){
               lcd.setCursor(5, 0);
               lcd.print("   "); 
            }else{
               if(counter < 1000000){
                  lcd.setCursor(6, 0);
                  lcd.print("  "); 
               }else{
                  if(counter < 10000000){
                     lcd.setCursor(7, 0);
                     lcd.print(" "); 
                  }
               }
            }
         }
      }                
   }
}

void loop(){ 
   switch(multi){
   case 0:
      if(!digitalRead(b_MULTI)){ 
         delay(50);
         if(!digitalRead(b_MULTI)){ 
            multi = 1;
         }
      }
      if(!digitalRead(b_UP)){ 
         delay(50);
         if(!digitalRead(b_UP)){ 
            counter++;
         }
      }
      if(!digitalRead(b_DOWN)){ 
         delay(50);
         if(!digitalRead(b_DOWN)){ 
            if(counter>0)
               counter--;
         }
      }
      lcd.setCursor(0, 1);
      lcd.print("10^0");
      lcd.setCursor(14, 0);
      lcd.print("Hz");
      lcd.setCursor(0, 0);
      lcd.print(counter);
      limpiaDigitos();
      actualiza_func();
      break;
   case 1:
      if(!digitalRead(b_MULTI)){ 
         delay(50);
         if(!digitalRead(b_MULTI)){ 
            multi = 2;
         }
      }
      if(!digitalRead(b_UP)){ 
         delay(50);
         if(!digitalRead(b_UP)){ 
            counter = counter + 1000;
         }
      }
      if(!digitalRead(b_DOWN)){ 
         delay(50);
         if(!digitalRead(b_DOWN)){ 
            if(counter>1000)
               counter = counter - 1000;
         }
      }
      lcd.setCursor(0, 1);
      lcd.print("10^3");
      lcd.setCursor(14, 0);
      lcd.print("Hz");
      lcd.setCursor(0, 0);
      lcd.print(counter);
      limpiaDigitos();
      actualiza_func();
      break;
   case 2:
      if(!digitalRead(b_MULTI)){ 
         delay(50);
         if(!digitalRead(b_MULTI)){ 
            multi = 0;
         }
      }
      if(!digitalRead(b_UP)){ 
         delay(50);
         if(!digitalRead(b_UP)){ 
            if(counter<12500000)
               counter = counter + 1000000;
         }
      }
      if(!digitalRead(b_DOWN)){ 
         delay(50);
         if(!digitalRead(b_DOWN)){ 
            if(counter>1000000)
               counter = counter - 1000000;
         }
      }
      lcd.setCursor(0, 1);
      lcd.print("10^6");
      lcd.setCursor(14, 0);
      lcd.print("Hz");
      lcd.setCursor(0, 0);
      lcd.print(counter);
      limpiaDigitos();
      actualiza_func();
      break;
   }
}

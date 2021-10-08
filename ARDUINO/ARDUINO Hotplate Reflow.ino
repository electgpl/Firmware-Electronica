#include <LiquidCrystal.h>
const int rs = 4, en = 3, d4 = 8, d5 = 7, d6 = 6, d7 = 5;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
#define TSETPOINT A0
#define SCLK      10
#define CS        11
#define MISO      12
#define TRIAC     2
#define BOTON     A2
#define START     A3
// Reflow profile
int profile[3] = {150, 230, 20};
float tSet = 0;
float tSen = 0;
int hysteresis = 2;
int i = 0;
//-----------------------------------------------------------
// Setup
//-----------------------------------------------------------
void setup(){
   Serial.begin(9600);
   lcd.begin(16, 2);
   lcd.clear();
   pinMode(CS, OUTPUT);
   pinMode(SCLK, OUTPUT);
   pinMode(MISO, INPUT);
   digitalWrite(CS, HIGH);
   pinMode(TRIAC, OUTPUT);
   pinMode(BOTON, INPUT);
}
//-----------------------------------------------------------
// Lee SPI por firmware
//-----------------------------------------------------------
byte spiRead(void){
   int i;
   byte d = 0;
   for(i = 7; i >= 0; i--) {
      digitalWrite(SCLK, LOW);
      delayMicroseconds(10);
      if(digitalRead(MISO)){
         d |= (1 << i);
      }
      digitalWrite(SCLK, HIGH);
      delayMicroseconds(10);
   }
   return d;
}
//-----------------------------------------------------------
// Lee el MAX6675 y Termocupla
//-----------------------------------------------------------
float MAX6675_read(void){
   uint16_t v;
   digitalWrite(CS, LOW);
   delayMicroseconds(10);
   v = spiRead();
   v <<= 8;
   v |= spiRead();
   digitalWrite(CS, HIGH);
   v >>= 3;
   return v * 0.25;
}
//-----------------------------------------------------------
// Actualiza Display
//-----------------------------------------------------------
void displayUpdate(){
   Serial.println(tSen);
   lcd.setCursor(0,1);  lcd.print("Set:");
   if(tSet<100){
      lcd.setCursor(6,1);  lcd.print(" "); 
   }
   lcd.setCursor(4,1);  lcd.print(tSet,0); 
   lcd.setCursor(8,1);  lcd.print("Sen:");
   if(tSen<100){
      lcd.setCursor(14,1);  lcd.print(" "); 
   }
   lcd.setCursor(12,1); lcd.print(tSen,0); 
}
//-----------------------------------------------------------
// Controla la salida TRIAC + Histeresis
//-----------------------------------------------------------
void ajustaTRIAC(int velocidad){
   if(tSet >= (tSen + hysteresis)){
      digitalWrite(TRIAC, HIGH);
      delay(velocidad);
      digitalWrite(TRIAC, LOW);
      delay(100);
   } 
   if(tSet <= (tSen - hysteresis)){
      digitalWrite(TRIAC, LOW);
   }
}
//-----------------------------------------------------------
// Actualiza el Reflow
//-----------------------------------------------------------
void actualizaReflow(int vel){
   tSen = MAX6675_read();
   delay(300);
   displayUpdate();
   ajustaTRIAC(vel);
}
//-----------------------------------------------------------
// Aplica perfil de Reflow
//-----------------------------------------------------------
void reflow(){
   while(digitalRead(START)){
      lcd.setCursor(0,0);  lcd.print("INICIO?         ");
      actualizaReflow(50);
   }
   if(profile[0]>tSen){
      for(int j=0; j<300; j++){
        i=0;
        tSet = profile[i];
        actualizaReflow(50);
        lcd.setCursor(0,0);  lcd.print("RAMPA           ");
      }
   }
   if(profile[0]>tSen){
      for(int j=0; j<300; j++){
        i=0;
        tSet = profile[i];
        actualizaReflow(50);
        lcd.setCursor(0,0);  lcd.print("PRE-CALENTADO   ");
      }
   }
   if(profile[1]>tSen){
      for(int j=0; j<100; j++){
        i=1;
        tSet = profile[i];
        actualizaReflow(150);
        lcd.setCursor(0,0);  lcd.print("RESOLDADO       ");
      }
   }
   if(profile[2]<tSen){
      for(int j=0; j<600; j++){
        i=2;
        tSet = profile[i];
        actualizaReflow(50);
        lcd.setCursor(0,0);  lcd.print("ENFRIAMIENTO    ");
      }
      i=0;
   }
}
//-----------------------------------------------------------
// Control Manual
//-----------------------------------------------------------
void manual(){
   lcd.setCursor(0,0);  lcd.print("MANUAL          ");
   tSet = map(analogRead(TSETPOINT),0,1023,0,300);
   tSen = MAX6675_read();
   delay(300);
   ajustaTRIAC(50);
   displayUpdate();
}
//-----------------------------------------------------------
// Funcion Repetitiva
//-----------------------------------------------------------
void loop(){
   if(digitalRead(BOTON)){
      manual();
   }else{
      reflow();
   }
}

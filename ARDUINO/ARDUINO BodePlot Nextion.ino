#include<SoftwareSerial.h>
#include<SPI.h>
#define FSYNC 4
#define WAVE_SINE     0x2000
#define WAVE_SQUARE   0x2028
#define WAVE_TRIANGLE 0x2002
#define softRx 8
#define softTx 9
#define MUESTRAS 200
SoftwareSerial nextion(softRx,softTx);
int inputADC = A0;
int adcVec[MUESTRAS] = {0};
char recv = 0;
unsigned long freq = 0;

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

void plotter(int dBsSend){
   String plotterSend = "add ";
   plotterSend += 1;
   plotterSend += ",";  
   plotterSend += 0;
   plotterSend += ",";
   plotterSend += dBsSend;
   nextion.print(plotterSend);
   nextion.write(0xFF);
   nextion.write(0xFF);
   nextion.write(0xFF);
}

void sendText(String sendStr){
   String textSend = "t17.txt=";
   textSend += '"';
   textSend += sendStr;
   textSend += '"';
   nextion.print(textSend);
   nextion.write(0xFF);
   nextion.write(0xFF);
   nextion.write(0xFF);
}

void adquiereADC(void){
   sendText("   Read...");
   for(int i=0; i<MUESTRAS; i++){
      freq = i+1;
      AD9833setFrequency(freq*100, WAVE_SQUARE);
      delay(10);
      adcVec[i] = analogRead(inputADC);
      delay(10);
   }  
}

void showPlot(void){
   for(int i=0; i<MUESTRAS; i++){
      plotter(adcVec[i]);
      plotter(adcVec[i]);
   }  
}

void testButton(void){
   recv = '0';
   while(recv != 'e'){
      recv = nextion.read();
   }
}

void compute(void){
   sendText("compute...");
   for(int i=0; i<MUESTRAS; i++){
      adcVec[i]=map(adcVec[i],0,1024,0,249);
      delay(5);
   }
}

void setup(){
   Serial.begin(9600);
   nextion.begin(9600);
   AD9833setup();
}

void loop(){
   adquiereADC();
   compute();
   showPlot();
   testButton();
}

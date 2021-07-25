#include "arduinoFFT.h"
#include <SoftwareSerial.h>
#define softRx  10
#define softTx  11
#define SAMPLES 32
#define xres    16
#define yres     8
SoftwareSerial nextion(softRx,softTx);
arduinoFFT FFT = arduinoFFT();
int MY_ARRAY[]={0, 128, 192, 224, 240, 248, 252, 254, 255};

double vReal[SAMPLES];
double vImag[SAMPLES];
char data_avgs[xres];
int yvalue;
int displaycolumn , displayvalue;
int peaks[xres];

void nextionSendOSC(int id, int canal, int val){
   String dataSendOSC = "add ";
   dataSendOSC += id;
   dataSendOSC += ",";  
   dataSendOSC += canal;
   dataSendOSC += ",";
   dataSendOSC += val;
   nextion.print(dataSendOSC);
   nextion.write(0xFF);
   nextion.write(0xFF);
   nextion.write(0xFF);
}

void nextionSendSA(int id, int k){  
   String dataSendSA = "j";
   dataSendSA += id;
   dataSendSA += ".val=";  
   dataSendSA += map(k,0,255,0,100);
   nextion.print(dataSendSA);
   nextion.write(0xFF);
   nextion.write(0xFF);
   nextion.write(0xFF);
}

void setup(){
   nextion.begin(9600);
}
 
void loop(){
   for(int i=0; i<SAMPLES; i++){
      int value = analogRead(A0) - 512;
      vReal[i]= value/8;
      vImag[i] = 0;                         
   }
   nextionSendOSC(17,0,map(analogRead(A0),0,1024,0,255));
   FFT.Windowing(vReal, SAMPLES, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
   FFT.Compute(vReal, vImag, SAMPLES, FFT_FORWARD);
   FFT.ComplexToMagnitude(vReal, vImag, SAMPLES);
   int step = (SAMPLES/2)/xres; 
   int c=0;
   for(int i=0; i<(SAMPLES/2); i+=step){
      data_avgs[c] = 0;
      for (int k=0 ; k< step ; k++){
          data_avgs[c] = data_avgs[c] + vReal[i+k];
      }
      data_avgs[c] = data_avgs[c]/step; 
      c++;
   }
   for(int i=0; i<xres; i++){
      data_avgs[i] = constrain(data_avgs[i],0,80);
      data_avgs[i] = map(data_avgs[i], 0, 80, 0, yres);
      yvalue=data_avgs[i];
      peaks[i] = peaks[i]-1;
      if (yvalue > peaks[i]) 
          peaks[i] = yvalue ;
      yvalue = peaks[i];    
      displayvalue=MY_ARRAY[yvalue];
      displaycolumn=(xres-1)-i;
      nextionSendSA(displaycolumn,displayvalue);
   }
}

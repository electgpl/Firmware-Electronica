#include "LedControl.h"
#include "arduinoFFT.h"

const uint16_t samples = 16;
double vReal[samples];
double vImag[samples];
#define SCL_INDEX 0x00
#define SCL_TIME 0x01
#define SCL_FREQUENCY 0x02
#define SCL_PLOT 0x03

arduinoFFT FFT = arduinoFFT();
LedControl lc = LedControl(12,11,10,1);

void setup(){
   lc.shutdown(0,false);
   lc.setIntensity(0,8);
   lc.clearDisplay(0);
}

void loop(){
   for(int i=0; i<samples; i++){
      vReal[i] = analogRead(A0);
      vImag[i] = 0;
   }
   
   FFT.Windowing(vReal, samples, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
   FFT.Compute(vReal, vImag, samples, FFT_FORWARD);
   FFT.ComplexToMagnitude(vReal, vImag, samples);
   
   for(uint16_t j = 0; j < (samples >> 1); j++){
      if((int)vReal[j]<10)
         lc.setRow(0,j,1);
      else 
         if((int)vReal[j]<30)
            lc.setRow(0,j,3);
         else 
            if((int)vReal[j]<50)
               lc.setRow(0,j,7);
            else 
               if((int)vReal[j]<70)
                  lc.setRow(0,j,15);
               else 
                  if((int)vReal[j]<90)
                     lc.setRow(0,j,31);
                  else 
                     if((int)vReal[j]<110)
                        lc.setRow(0,j,63);
                     else 
                        if((int)vReal[j]<130)
                           lc.setRow(0,j,127);
                        else 
                           if((int)vReal[j]<150)
                              lc.setRow(0,j,255);    
   }
}

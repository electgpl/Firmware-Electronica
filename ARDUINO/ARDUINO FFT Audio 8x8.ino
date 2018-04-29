/*
   Programa Analizador de Espectro de Audio
   Implementación de biblioteca "arduinoFFT" para el control FFT
   Implementación de matriz de Leds 8x8 con MAX7219
   Pines de conexión:
      CLK pin 11
      CS  pin 10
      DIN pin 12
      ADC pin A0
   Autor: Electgpl
*/

#include "LedControl.h"
#include "arduinoFFT.h"

const uint16_t samples = 16;
uint16_t k = 0;
double vReal[samples];
double vImag[samples];

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
   
   FFT.Windowing(vReal, samples, FFT_WIN_TYP_RECTANGLE, FFT_FORWARD);
   FFT.Compute(vReal, vImag, samples, FFT_FORWARD);
   FFT.ComplexToMagnitude(vReal, vImag, samples);

   k = (samples/2)-1;

   for(uint16_t j = 0; j < (samples/2); j++){
      if((byte)vReal[j]<10)
         lc.setRow(0,k,1);
      else 
         if((byte)vReal[j]<30)
            lc.setRow(0,k,3);
         else 
            if((byte)vReal[j]<50)
               lc.setRow(0,k,7);
            else 
               if((byte)vReal[j]<70)
                  lc.setRow(0,k,15);
               else 
                  if((byte)vReal[j]<90)
                     lc.setRow(0,k,31);
                  else 
                     if((byte)vReal[j]<110)
                        lc.setRow(0,k,63);
                     else 
                        if((byte)vReal[j]<130)
                           lc.setRow(0,k,127);
                        else 
                           if((byte)vReal[j]<150)
                              lc.setRow(0,k,255);   
       if(k>0)
          k--;
   }
}

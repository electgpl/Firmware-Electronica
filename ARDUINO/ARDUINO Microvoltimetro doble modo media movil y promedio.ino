#include "LedControl.h"
#include <Wire.h>
#include <ADS1219.h>
#define address 0x40
#define rst 3
#define drdy 4

ADS1219 ads(drdy);
LedControl lc=LedControl(12,11,10,1);

long int adc_filtrado = 0;
long int adc_raw = 0;
#define alpha 0.01

int x10000000 = 0;
int x1000000 = 0;
int x100000 = 0;
int x10000 = 0;
int x1000 = 0;
int x100 = 0;
int x10 = 0;
int x1 = 0;

long int valorADC = 0.0;

void setup(){
   lc.shutdown(0,false);
   lc.setIntensity(0,8);
   lc.clearDisplay(0);  
   ads.begin();
   pinMode(13,OUTPUT);
   pinMode(7,INPUT_PULLUP);
   pinMode(drdy,INPUT_PULLUP);
   pinMode(rst,OUTPUT);
   digitalWrite(rst,HIGH);
   Wire.begin();
   ads.setVoltageReference(REF_INTERNAL);
}

void loop(){
   if(digitalRead(7)){
      valorADC = 0;
      for(int i=0; i<10; i++){
         valorADC = valorADC + (ads.readDifferential_2_3()*2.048/8388608)*1000000;
      }
      valorADC = valorADC / 10;
   }else{
      adc_raw = (ads.readDifferential_2_3()*2.048/8388608)*1000000;
      adc_filtrado = (alpha*adc_raw) + ((1-alpha)*adc_filtrado);
      valorADC = adc_filtrado;
   }
   
   x10000000 =  valorADC / 10000000;
   x1000000  = (valorADC / 1000000) %10;
   x100000   = (valorADC / 100000) %10;
   x10000    = (valorADC / 10000) %10;
   x1000     = (valorADC / 1000) %10;
   x100      = (valorADC / 100) %10;      
   x10       = (valorADC / 10) %10;  
   x1        =  valorADC %10;

   //lc.setDigit(0,7,x10000000,false);
   lc.setDigit(0,6,x1000000,true);
   lc.setDigit(0,5,x100000,false);
   lc.setDigit(0,4,x10000,false);
   lc.setDigit(0,3,x1000,false);
   lc.setDigit(0,2,x100,false);
   lc.setDigit(0,1,x10,false);
   lc.setDigit(0,0,x1,false);

   delay(50);
   digitalWrite(13, !digitalRead(13));
}

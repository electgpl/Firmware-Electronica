#include "LedControl.h"
#include <Wire.h>
#include <ADS1219.h>
#define address 0x40
#define rst 3
#define drdy 4

ADS1219 ads(drdy);
LedControl lc=LedControl(12,13,10,1);

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
   pinMode(drdy,INPUT_PULLUP);
   pinMode(rst,OUTPUT);
   digitalWrite(rst,HIGH);
   Wire.begin();
   ads.setVoltageReference(REF_INTERNAL);
}

void loop(){
   valorADC = (ads.readSingleEnded(0)*2.048/8388608)*10000000;

   x10000000 =  valorADC / 10000000;
   x1000000  = (valorADC / 1000000) %10;
   x100000   = (valorADC / 100000) %10;
   x10000    = (valorADC / 10000) %10;
   x1000     = (valorADC / 1000) %10;
   x100      = (valorADC / 100) %10;      
   x10       = (valorADC / 10) %10;  
   x1        =  valorADC %10;

   lc.setDigit(0,7,x10000000,false);
   lc.setDigit(0,6,x1000000,true);
   lc.setDigit(0,5,x100000,false);
   lc.setDigit(0,4,x10000,false);
   lc.setDigit(0,3,x1000,false);
   lc.setDigit(0,2,x100,false);
   lc.setDigit(0,1,x10,false);
   lc.setDigit(0,0,x1,false);

   delay(150);
   lc.setDigit(0,0,x1,true);
   delay(150);
}

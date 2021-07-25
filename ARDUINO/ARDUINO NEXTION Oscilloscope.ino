#include <SoftwareSerial.h>
#define softRx 10
#define softTx 11
#define id 1
#define canal 0
SoftwareSerial nextion(softRx,softTx);
int inputADC = A0;
void setup(){
   Serial.begin(9600);
   nextion.begin(9600);
}
void loop(){
   int valueADC = map(analogRead(inputADC),0,1024,0,255);
   String dataSend = "add ";
   dataSend += id;
   dataSend += ",";  
   dataSend += canal;
   dataSend += ",";
   dataSend += valueADC;
   nextion.print(dataSend);
   nextion.write(0xFF);
   nextion.write(0xFF);
   nextion.write(0xFF);
   Serial.println(valueADC);
}

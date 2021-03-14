#define outputPin 10
boolean const BitsDatosSalida[25]={0,1,0,1,1,0,0,0,1,0,1,0,0,1,1,1,1,0,1,0,0,0,0,1,0};
void setup(){
   pinMode(outputPin, OUTPUT);
}
void loop(){
   delay(2000);
   for(int i=0;i<10;i++){
      delayMicroseconds(5000);
      for(int j=0;j<25;j++){
         if(BitsDatosSalida[j]){
            digitalWrite(outputPin,HIGH);
            delayMicroseconds(1100);
            digitalWrite(outputPin,LOW);
            delayMicroseconds(370); 
         }else{
            digitalWrite(outputPin,HIGH);
            delayMicroseconds(370);
            digitalWrite(outputPin,LOW);
            delayMicroseconds(1100);
         }  
      }
   }
}

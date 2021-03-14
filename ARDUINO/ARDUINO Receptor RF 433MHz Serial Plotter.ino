int vector[800];
void setup(){
   Serial.begin(115200);
   for(int i=0; i<800; i++){
      if(digitalRead(2)){
         vector[i] = 1;
         delayMicroseconds(100);
      }else{
         vector[i] = 0;
         delayMicroseconds(100);
      }   
   }
   for(int i=0; i<800; i++){
      if(vector[i])
         Serial.println(100);
      else
         Serial.println(0);
   }
}

void loop(){ 
}

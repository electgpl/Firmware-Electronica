float corriente0 = 0;
float corriente1 = 1;
float corriente2 = 2;
float corriente3 = 3;
float corriente4 = 4;
float corriente5 = 5;
float shunt = 0.33;
int swi=0;
int flag=LOW;
String comandoLeido;
String estado[22]={"AT",
                   "AT+HELP",
                   "AT+I0",
                   "AT+I1",
                   "AT+I2",
                   "AT+I3",
                   "AT+I4",
                   "AT+I5",
                   "AT+D2ON",
                   "AT+D2OFF",
                   "AT+D3ON",  
                   "AT+D3OFF",         
                   "AT+D4ON",
                   "AT+D4OFF",
                   "AT+D5ON",
                   "AT+D5OFF",         
                   "AT+D6ON",
                   "AT+D6OFF",         
                   "AT+D7ON",    
                   "AT+D7OFF",
                   "AT+ALLON",
                   "AT+ALLOFF"};
void setup(){
   Serial.begin(9600);
   //analogReference(INTERNAL1V1);
   pinMode(2,OUTPUT);
   pinMode(3,OUTPUT);
   pinMode(4,OUTPUT);
   pinMode(5,OUTPUT);
   pinMode(6,OUTPUT);
   pinMode(7,OUTPUT);
}
void consultaAT(void){
   while(Serial.available()){
      comandoLeido=Serial.readStringUntil('\n');
      swi=0;
      flag=LOW;
      while(swi<23 && flag==LOW){
         if(estado[swi]==comandoLeido){
            switch(swi){
            case 0:
               Serial.println("OK");
               Serial.println("");
               flag=HIGH;
               break;  
            case 1:
               Serial.println("> HELP");
               Serial.println("   > AT+IX");
               Serial.println("      > AT+I0");
               Serial.println("      > AT+I1");
               Serial.println("      > AT+I2");
               Serial.println("      > AT+I3");
               Serial.println("      > AT+I4");
               Serial.println("      > AT+I5");
               Serial.println("   > AT+DXON");
               Serial.println("      > AT+D2ON");
               Serial.println("      > AT+D3ON");
               Serial.println("      > AT+D4ON");
               Serial.println("      > AT+D5ON");
               Serial.println("      > AT+D6ON");
               Serial.println("      > AT+D7ON");
               Serial.println("   > AT+DXOFF");
               Serial.println("      > AT+D2OFF");
               Serial.println("      > AT+D3OFF");
               Serial.println("      > AT+D4OFF");
               Serial.println("      > AT+D5OFF");
               Serial.println("      > AT+D6OFF");
               Serial.println("      > AT+D7OFF");
               Serial.println("   > AT+ALLON");
               Serial.println("   > AT+ALLOFF");
               Serial.println("OK");
               Serial.println("");
               flag=HIGH;
               break;  
            case 2:
               Serial.println("AT+I0");
               corriente0 = (analogRead(A0) * 5000.0 / 1023.0) / shunt;
               delay(10);
               Serial.println(corriente0);
               Serial.println("");
               flag=HIGH;
               break;
            case 3:
               Serial.println("AT+I1");
               corriente1 = (analogRead(A1) * 5000.0 / 1023.0) / shunt;
               delay(10);
               Serial.println(corriente1);
               Serial.println("");
               flag=HIGH;
               break;
            case 4:
               Serial.println("AT+I2");
               corriente2 = (analogRead(A2) * 5000.0 / 1023.0) / shunt;
               delay(10);
               Serial.println(corriente2);
               Serial.println("");
               flag=HIGH;
               break;
            case 5:
               Serial.println("AT+I3");
               corriente3 = (analogRead(A3) * 5000.0 / 1023.0) / shunt;
               delay(10);
               Serial.println(corriente3);
               Serial.println("");
               flag=HIGH;
               break;
            case 6:
               Serial.println("AT+I4");
               corriente4 = (analogRead(A4) * 5000.0 / 1023.0) / shunt;
               delay(10);
               Serial.println(corriente4);
               Serial.println("");
               flag=HIGH;
               break;
            case 7:
               Serial.println("AT+I5");
               corriente5 = (analogRead(A5) * 5000.0 / 1023.0) / shunt;
               delay(10);
               Serial.println(corriente5);
               Serial.println("");
               flag=HIGH;
               break;
            case 8:   
               Serial.println("AT+D2ON");
               digitalWrite(2,HIGH);
               Serial.println("OK D2 ON");
               Serial.println("");
               flag=HIGH;
               break;
            case 9:   
               Serial.println("AT+D2OFF");
               digitalWrite(2,LOW);
               Serial.println("OK D2 OFF");
               Serial.println("");
               flag=HIGH;
               break;
            case 10:   
               Serial.println("AT+D3ON");
               digitalWrite(3,HIGH);
               Serial.println("OK D3 ON");
               Serial.println("");
               flag=HIGH;
               break;
            case 11:   
               Serial.println("AT+D3OFF");
               digitalWrite(3,LOW);
               Serial.println("OK D3 OFF");
               Serial.println("");
               flag=HIGH;
               break;
            case 12:   
               Serial.println("AT+D4ON");
               digitalWrite(4,HIGH);
               Serial.println("OK D4 ON");
               Serial.println("");
               flag=HIGH;
               break;
            case 13:   
               Serial.println("AT+D4OFF");
               digitalWrite(4,LOW);
               Serial.println("OK D4 OFF");
              Serial.println("");
               flag=HIGH;
               break;
            case 14:   
               Serial.println("AT+D5ON");
               digitalWrite(5,HIGH);
               Serial.println("OK D5 ON");
               Serial.println("");
               flag=HIGH;
               break;
            case 15:   
               Serial.println("AT+D5OFF");
               digitalWrite(5,LOW);
               Serial.println("OK D5 OFF");
               Serial.println("");
               flag=HIGH;
               break;
            case 16:   
               Serial.println("AT+D6ON");
               digitalWrite(6,HIGH);
               Serial.println("OK D6 ON");
               Serial.println("");
               flag=HIGH;
               break;
            case 17:   
               Serial.println("AT+D6OFF");
               digitalWrite(6,LOW);
               Serial.println("OK D6 OFF");
               Serial.println("");
               flag=HIGH;
               break;
            case 18:   
               Serial.println("AT+D7ON");
               digitalWrite(7,HIGH);
               Serial.println("OK D7 ON");
               Serial.println("");
               flag=HIGH;
               break;
            case 19:   
               Serial.println("AT+D7OFF");
               digitalWrite(7,LOW);
               Serial.println("OK D7 OFF");
               Serial.println("");
               flag=HIGH;
               break;
            case 20:   
               Serial.println("AT+ALLON");
               digitalWrite(2,HIGH);
               digitalWrite(3,HIGH);
               digitalWrite(4,HIGH);
               digitalWrite(5,HIGH);
               digitalWrite(6,HIGH);
               digitalWrite(7,HIGH);
               Serial.println("OK ALL ON");
               Serial.println("");
               flag=HIGH;
               break;
            case 21:   
               Serial.println("AT+ALLOFF");
               digitalWrite(2,LOW);
               digitalWrite(3,LOW);
               digitalWrite(4,LOW);
               digitalWrite(5,LOW);
               digitalWrite(6,LOW);
               digitalWrite(7,LOW);
               Serial.println("OK ALL OFF");
               Serial.println("");
               flag=HIGH;
               break;                              
            default:
               Serial.println("AT-DEFAULT");
               break;
            }
         }
         else{
            flag=LOW;
         }
         swi++;
      }
      if(flag==LOW){
         Serial.println("AT-ERROR");
         Serial.println("");
      }
   }
}
void loop(){
   consultaAT();

}

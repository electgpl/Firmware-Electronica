const int Trigger_1 = 2;
const int Trigger_2 = 4;
const int Echo_1 = 3;
const int Echo_2 = 5;
const int Pulsador_1 = 6;
const int Pulsador_2 = 7;
const int Bobina_1 = 8;
const int Bobina_2 = 9;
int distancia_1 = 0;
int distancia_2 = 0;
int medicion_1 = 0;
int medicion_2 = 0;
int swi=0;
int i=0;
const int MUESTRAS = 10;
int flag=LOW;
String comandoLeido;
String estado[10]={"AT",
                  "AT+HELP",
                  "AT+D1",
                  "AT+D2",
                  "AT+P1",
                  "AT+P2",
                  "AT+B1",
                  "AT+B2",
                  "AT+TEST",
                  "AT+INFO"};
void setup(){
   Serial.begin(9600);
   pinMode(LED_BUILTIN, OUTPUT);
   pinMode(Trigger_1, OUTPUT);
   pinMode(Trigger_2, OUTPUT);
   pinMode(Bobina_1,  OUTPUT);
   pinMode(Bobina_2,  OUTPUT);
   pinMode(Echo_1, INPUT);
   pinMode(Echo_2, INPUT);
   pinMode(Pulsador_1, INPUT_PULLUP);
   pinMode(Pulsador_2, INPUT_PULLUP); 
   digitalWrite(Trigger_1, LOW);
   digitalWrite(Trigger_2, LOW);
   digitalWrite(Bobina_1, LOW);
   digitalWrite(Bobina_2, LOW);
}
int medirDistancia_1(void){
   digitalWrite(Trigger_1, HIGH);
   delayMicroseconds(10);
   digitalWrite(Trigger_1, LOW);
   distancia_1 = pulseIn(Echo_1, HIGH)/58;
   delay(100);  
   return distancia_1;
}
int medirDistancia_2(void){
   digitalWrite(Trigger_2, HIGH);
   delayMicroseconds(10);
   digitalWrite(Trigger_2, LOW);
   distancia_2 = pulseIn(Echo_2, HIGH)/58;
   delay(100);
   return distancia_2;
}
void consultaAT(void){
   while(Serial.available()){
      comandoLeido=Serial.readStringUntil('\n');
      swi=0;
      flag=LOW;
      while(swi<11 && flag==LOW){
         if(estado[swi]==comandoLeido){
            switch(swi){
            case 0:
               Serial.println("OK");
               Serial.println("");
               flag=HIGH;
               break;  
            case 1:
               Serial.println("> HELP");
               Serial.println("   > MEDICION DE DISTANCIA: ");
               Serial.println("      > AT+D1");
               Serial.println("      > AT+D2");
               Serial.println("   > ESTADO DE PULSADORES: ");
               Serial.println("      > AT+P1");
               Serial.println("      > AT+P2");
               Serial.println("   > ACTIVA TEMPORALMENTE SALIDAS: ");
               Serial.println("      > AT+B1");
               Serial.println("      > AT+B2");
               Serial.println("   > TEST TOGGLE LED 1 SEGUNDO: ");
               Serial.println("      > AT+TEST");
               Serial.println("   > INFORMACION DEL EQUIPO: ");
               Serial.println("      > AT+INFO");               
               Serial.println("OK");
               Serial.println("");
               flag=HIGH;
               break;  
            case 2:
               Serial.println("AT+D1");
               medicion_1 = 0;
               for(i=0;i<MUESTRAS;i++){
                  medicion_1 = medicion_1 + medirDistancia_1();
                  delay(10);
               }
               Serial.println(medicion_1/MUESTRAS);
               Serial.println("");
               flag=HIGH;
               break;
            case 3:
               Serial.println("AT+D2");
               medicion_2 = 0;
               for(i=0;i<MUESTRAS;i++){
                  medicion_2 = medicion_2 + medirDistancia_2();
                  delay(10);
               }
               Serial.println(medicion_2/MUESTRAS);
               Serial.println("");
               flag=HIGH;
               break;
            case 4:   
               Serial.println("AT+P1");
               Serial.println(digitalRead(Pulsador_1));
               Serial.println("");
               flag=HIGH;
               break;
            case 5:   
               Serial.println("AT+P2");
               Serial.println(digitalRead(Pulsador_2));
               Serial.println("");
               flag=HIGH;
               break;
            case 6:   
               Serial.println("AT+B1");
               digitalWrite(Bobina_1,HIGH);
               delay(250);
               digitalWrite(Bobina_1,LOW);
               Serial.println("OK B1");
               Serial.println("");
               flag=HIGH;
               break;
            case 7:   
               Serial.println("AT+B2");
               digitalWrite(Bobina_2,HIGH);
               delay(250);
               digitalWrite(Bobina_2,LOW);
               Serial.println("OK B2");
               Serial.println("");
               flag=HIGH;
               break;    
            case 8:   
               Serial.println("AT+TEST");
               digitalWrite(LED_BUILTIN,HIGH);
               delay(1000);
               digitalWrite(LED_BUILTIN,LOW);
               Serial.println("OK TEST");
               Serial.println("");
               flag=HIGH;
               break;    
            case 9:   
               Serial.println(" ");
               Serial.println(" ");
               Serial.println("  _|_|_|_|  _|                        _|                          _|  ");
               Serial.println("  _|        _|    _|_|      _|_|_|  _|_|_|_|    _|_|_|  _|_|_|    _|  ");  
               Serial.println("  _|_|_|    _|  _|_|_|_|  _|          _|      _|    _|  _|    _|  _|  ");  
               Serial.println("  _|        _|  _|        _|          _|      _|    _|  _|    _|  _|  ");  
               Serial.println("  _|_|_|_|  _|    _|_|_|    _|_|_|      _|_|    _|_|_|  _|_|_|    _|  ");  
               Serial.println("                                                    _|  _|            ");  
               Serial.println("                                                _|_|    _|            ");
               Serial.println(" ");
               Serial.println("     Control Calibre Ultrasonico version: 2016");
               Serial.println(" ");
               Serial.println("     Trigger1.: PIN 2 ");
               Serial.println("     Echo1....: PIN 3 ");
               Serial.println("     Trigger2.: PIN 4 ");
               Serial.println("     Echo2....: PIN 5 ");
               Serial.println("     Pulsador1: PIN 6 ");
               Serial.println("     Pulsador2: PIN 7 ");
               Serial.println("     Bobina1..: PIN 8 ");
               Serial.println("     Bobina2..: PIN 9 ");
               Serial.println(" ");
               Serial.println(" ");
               flag=HIGH;
               break;   
            default:
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

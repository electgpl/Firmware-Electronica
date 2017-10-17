#include <Servo.h>
Servo myservo;
#define GREEN 9
#define BLUE 10
#define RED 11
#define delayTimeRGB 20
#define BUZZER 6
#define BOTON 2
#define LDR A0
#define TEMP A1
#define POTE A2
#define SERVO 5
#define BLINK 13
void setup() {
    pinMode(BUZZER, OUTPUT);
    pinMode(BOTON, INPUT);
    pinMode(LDR, INPUT);
    pinMode(TEMP, INPUT);
    pinMode(POTE, INPUT);
    pinMode(SERVO, OUTPUT);
    pinMode(GREEN, OUTPUT);
    pinMode(BLUE, OUTPUT);
    pinMode(RED, OUTPUT);
    Serial.begin(9600);
    myservo.attach(SERVO);
}
void loop() {
    //funcionSERVO();
    funcionRGB();  
    //funcionLDR();
    //funcionPOTE();
    //funcionTEMP();
    //funcionBUZZER();
}
void funcionRGB() {
    for(int b=0;b<16;b++){
       analogWrite(BLUE, b);
       delay(delayTimeRGB);
    }
    for(int r=16;r>1;r--){
       analogWrite(RED, r);
       delay(delayTimeRGB);
    }
    for(int g=0;g<16;g++){
       analogWrite(GREEN, g);
       delay(delayTimeRGB);
    }
    for(int b=16;b>1;b--){
       analogWrite(BLUE, b);
       delay(delayTimeRGB);
    }      
    for(int r=0;r<16;r++){
       analogWrite(RED, r);
       delay(delayTimeRGB);
    }
    for(int g=16;g>1;g--){
       analogWrite(GREEN, g);
       delay(delayTimeRGB);
    }
}
void funcionPOTE() {
    int sensorValue = analogRead(POTE);
    Serial.println("POTENCIOMETRO");
    Serial.print("ADC: ");
    Serial.println(sensorValue);
    digitalWrite(BLINK, HIGH);
    delay(100);
    digitalWrite(BLINK, LOW);
    delay(1000);
}
void funcionLDR() {
    int sensorValue = analogRead(LDR);
    Serial.println("LDR");
    Serial.print("LUX: ");
    Serial.println(map(sensorValue,0,1023,20,1000));
    digitalWrite(BLINK, HIGH);
    delay(100);
    digitalWrite(BLINK, LOW);
    delay(1000);
}
void funcionTEMP() {
    int sensorValue = analogRead(TEMP);
    Serial.println("TEMPERATURA");
    Serial.print("C: ");
    Serial.println(sensorValue*0.41);
    digitalWrite(BLINK, HIGH);
    delay(100);
    digitalWrite(BLINK, LOW);
    delay(1000);
}
void funcionSERVO() {
    for (int pos = 0; pos <= 180; pos += 1) { 
        myservo.write(pos);
        delay(15);
    }
    for (int pos = 180; pos >= 0; pos -= 1) {
        myservo.write(pos);
        delay(15);
    }
}
#define G 12
#define F 11
#define A 10
#define B 9
#define E 8
#define D 7
#define C 6
#define DP 13
#define GND1 5
#define GND2 4
#define GND3 3
#define GND4 2
void digito_0(){
digitalWrite(A, HIGH);
digitalWrite(B, HIGH);
digitalWrite(C, HIGH);
digitalWrite(D, HIGH);
digitalWrite(E, HIGH);
digitalWrite(F, HIGH);
digitalWrite(G, LOW);
}
void digito_1(){
digitalWrite(A, LOW);
digitalWrite(B, HIGH);
digitalWrite(C, HIGH);
digitalWrite(D, LOW);
digitalWrite(E, LOW);
digitalWrite(F, LOW);
digitalWrite(G, LOW);
}
void digito_2(){
digitalWrite(A, HIGH);
digitalWrite(B, HIGH);
digitalWrite(C, LOW);
digitalWrite(D, HIGH);
digitalWrite(E, HIGH);
digitalWrite(F, LOW);
digitalWrite(G, HIGH);
}
void digito_3(){
digitalWrite(A, HIGH);
digitalWrite(B, HIGH);
digitalWrite(C, HIGH);
digitalWrite(D, HIGH);
digitalWrite(E, LOW);
digitalWrite(F, LOW);
digitalWrite(G, HIGH);
}
void digito_4(){
digitalWrite(A, LOW);
digitalWrite(B, HIGH);
digitalWrite(C, HIGH);
digitalWrite(D, LOW);
digitalWrite(E, LOW);
digitalWrite(F, HIGH);
digitalWrite(G, HIGH);
}
void digito_5(){
digitalWrite(A, HIGH);
digitalWrite(B, LOW);
digitalWrite(C, HIGH);
digitalWrite(D, HIGH);
digitalWrite(E, LOW);
digitalWrite(F, HIGH);
digitalWrite(G, HIGH);
}
void digito_6(){
digitalWrite(A, HIGH);
digitalWrite(B, LOW);
digitalWrite(C, HIGH);
digitalWrite(D, HIGH);
digitalWrite(E, HIGH);
digitalWrite(F, HIGH);
digitalWrite(G, HIGH);
}
void digito_7(){
digitalWrite(A, HIGH);
digitalWrite(B, HIGH);
digitalWrite(C, HIGH);
digitalWrite(D, LOW);
digitalWrite(E, LOW);
digitalWrite(F, LOW);
digitalWrite(G, LOW);
}
void digito_8(){
digitalWrite(A, HIGH);
digitalWrite(B, HIGH);
digitalWrite(C, HIGH);
digitalWrite(D, HIGH);
digitalWrite(E, HIGH);
digitalWrite(F, HIGH);
digitalWrite(G, HIGH);
}
void digito_9(){
digitalWrite(A, HIGH);
digitalWrite(B, HIGH);
digitalWrite(C, HIGH);
digitalWrite(D, HIGH);
digitalWrite(E, LOW);
digitalWrite(F, HIGH);
digitalWrite(G, HIGH);
}
void numero(int digito){
   switch(digito){
   case 0:
      digito_0();
      break;
   case 1:
      digito_1();
      break;
   case 2:
      digito_2();
      break;
   case 3:
      digito_3();
      break;
   case 4:
      digito_4();
      break;
   case 5:
      digito_5();
      break;
   case 6:
      digito_6();
      break;
   case 7:
      digito_7();
      break;
   case 8:
      digito_8();
      break;
   case 9:
      digito_9();
      break;
   default:
      break;
   }
}
void mux(int dig4, int dig3, int dig2, int dig1){
   digitalWrite(GND1, HIGH);
   digitalWrite(GND2, LOW);
   digitalWrite(GND3, LOW);
   digitalWrite(GND4, LOW);
   numero(dig1);
   delay(3);
   digitalWrite(GND1, LOW);
   digitalWrite(GND2, HIGH);
   digitalWrite(GND3, LOW);
   digitalWrite(GND4, LOW);
   numero(dig2);
   delay(3);
   digitalWrite(GND1, LOW);
   digitalWrite(GND2, LOW);
   digitalWrite(GND3, HIGH);
   digitalWrite(GND4, LOW);
   numero(dig3);
   delay(3);
   digitalWrite(GND1, LOW);
   digitalWrite(GND2, LOW);
   digitalWrite(GND3, LOW);
   digitalWrite(GND4, HIGH);
   numero(dig4);
   delay(3);
}
void mostrar(int valor){
   int uno, diez, cien, mil;
   mil=valor/1000;
   cien=(valor/100)%10;
   diez=(valor/10)%10;
   uno=valor%10;
   mux(uno,diez,cien,mil);
}
const int OUT_PIN = A4;
const int IN_PIN = A0;
long int acumulado=0;
long int actualizado=0;
void setup(){
   pinMode(A, OUTPUT);
   pinMode(B, OUTPUT);
   pinMode(C, OUTPUT);
   pinMode(D, OUTPUT);
   pinMode(E, OUTPUT);
   pinMode(F, OUTPUT);
   pinMode(G, OUTPUT);
   pinMode(DP, OUTPUT);
   pinMode(GND1, OUTPUT);
   pinMode(GND2, OUTPUT);
   pinMode(GND3, OUTPUT);
   pinMode(GND4, OUTPUT);
   pinMode(OUT_PIN, OUTPUT);
   pinMode(IN_PIN, OUTPUT);
}
void loop(){ 
   for(int i=0; i<100; i++){
      pinMode(IN_PIN, INPUT);
      digitalWrite(OUT_PIN, HIGH);
      int val = analogRead(IN_PIN);
      digitalWrite(OUT_PIN, LOW);
      pinMode(IN_PIN, OUTPUT);
      float capacitancia = (float)val * 24.5 / (float)(1023 - val);
      acumulado = capacitancia + acumulado;
      mostrar(actualizado);
   }  
   actualizado = acumulado/100;
   acumulado=0;
}

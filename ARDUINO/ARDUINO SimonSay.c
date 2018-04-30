int secuencia[NIVEL_MAX];
int secuenciaUsuario[NIVEL_MAX];
int nivelActual = 1;
int velocidad = 1000;
const int NIVEL_MAX = 100;

void setup(){
   pinMode(A0, INPUT);
   pinMode(A1, INPUT);
   pinMode(A2, INPUT);
   pinMode(A3, INPUT);

   pinMode(2, OUTPUT);
   pinMode(3, OUTPUT);
   pinMode(4, OUTPUT);
   pinMode(5, OUTPUT);

   digitalWrite(2, LOW);
   digitalWrite(3, LOW);
   digitalWrite(4, LOW);
   digitalWrite(5, LOW);
}

void loop(){
   if(nivelActual == 1){
      generaSecuencia();
      muestraSecuencia();
      leeSecuencia();
   }
   if(nivelActual != 1){
      muestraSecuencia();
      leeSecuencia();
   }
}

void muestraSecuencia(){
   digitalWrite(2, LOW);
   digitalWrite(3, LOW);
   digitalWrite(4, LOW);
   digitalWrite(5, LOW);
   for(int i = 0; i < nivelActual; i++){
      digitalWrite(secuencia[i], HIGH);
      delay(velocidad);
      digitalWrite(secuencia[i], LOW);
      delay(200);
   }
}

void leeSecuencia(){
   int flag = 0;
   for(int i = 0; i < nivelActual; i++){
      flag = 0;
      while(flag == 0){
         if(digitalRead(A0) == LOW){
            digitalWrite(5, HIGH);
            secuenciaUsuario[i] = 5;
            flag = 1;
            delay(200);
            if(secuenciaUsuario[i] != secuencia[i]){
               secuenciaError();
               return;
            }
            digitalWrite(5, LOW);
         }
         if(digitalRead(A1) == LOW){
            digitalWrite(4, HIGH);
            secuenciaUsuario[i] = 4;
            flag = 1;
            delay(200);
            if(secuenciaUsuario[i] != secuencia[i]){
               secuenciaError();
               return;
            }
            digitalWrite(4, LOW);
         }
         if(digitalRead(A2) == LOW){
            digitalWrite(3, HIGH);
            secuenciaUsuario[i] = 3;
            flag = 1;
            delay(200);
            if(secuenciaUsuario[i] != secuencia[i]){
               secuenciaError();
               return;
            }
            digitalWrite(3, LOW);
         }
         if(digitalRead(A3) == LOW){
            digitalWrite(2, HIGH);
            secuenciaUsuario[i] = 2;
            flag = 1;
            delay(200);
            if(secuenciaUsuario[i] != secuencia[i]){
               secuenciaError();
               return;
            }
            digitalWrite(2, LOW);
         }
      }
   }
   secuenciaCorrecta();
}

void generaSecuencia(){
   randomSeed(millis());
   for(int i = 0; i < NIVEL_MAX; i++){
      secuencia[i] = random(2,6);
   }
}

void secuenciaError(){
   for(int i = 0; i < 3; i++){
      digitalWrite(2, HIGH);
      digitalWrite(3, HIGH);
      digitalWrite(4, HIGH);
      digitalWrite(5, HIGH);
      delay(250);
      digitalWrite(2, LOW);
      digitalWrite(3, LOW);
      digitalWrite(4, LOW);
      digitalWrite(5, LOW);
      delay(250);
   }
   nivelActual = 1;
   velocidad = 1000;
}

void secuenciaCorrecta(){
   if(nivelActual < NIVEL_MAX);
      nivelActual++;
   velocidad -= 50;
   delay(500);
}

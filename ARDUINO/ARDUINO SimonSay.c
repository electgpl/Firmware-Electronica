#define   BUZZER   7
#define   ENTRADA_A  A0
#define   ENTRADA_B  A1
#define   ENTRADA_C  A2
#define   ENTRADA_D  A3
#define   SALIDA_A   2
#define   SALIDA_B   3
#define   SALIDA_C   4
#define   SALIDA_D   5

int melodia[ ] = {262, 196, 196, 220, 196, 0, 247, 262};
int duracionNotas[] = {4, 8, 8, 4, 4, 4, 4, 4};
int nivelActual = 1;
int velocidad = 500;
const int NIVEL_MAX = 100;
int secuencia[NIVEL_MAX];
int secuenciaUsuario[NIVEL_MAX];

void setup(){
   pinMode(ENTRADA_D, INPUT);
   pinMode(ENTRADA_C, INPUT);
   pinMode(ENTRADA_B, INPUT);
   pinMode(ENTRADA_A, INPUT);
   pinMode(SALIDA_A, OUTPUT);
   pinMode(SALIDA_B, OUTPUT);
   pinMode(SALIDA_C, OUTPUT);
   pinMode(SALIDA_D, OUTPUT);
   digitalWrite(SALIDA_A, LOW);
   digitalWrite(SALIDA_B, LOW);
   digitalWrite(SALIDA_C, LOW);
   digitalWrite(SALIDA_D, LOW);
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
   digitalWrite(SALIDA_A, LOW);
   digitalWrite(SALIDA_B, LOW);
   digitalWrite(SALIDA_C, LOW);
   digitalWrite(SALIDA_D, LOW);
   for(int i = 0; i < nivelActual; i++){
      if( secuencia[i] == SALIDA_A ){
         tone(BUZZER, 200);
         delay(200);
         noTone(BUZZER);
      }
      if( secuencia[i] == SALIDA_B ){
         tone(BUZZER, 300);
         delay(200);
         noTone(BUZZER);
      }
      if( secuencia[i] == SALIDA_C ){
         tone(BUZZER, 400);
         delay(200);
         noTone(BUZZER);
      }
      if( secuencia[i] == SALIDA_D ){
         tone(BUZZER, 500);
         delay(200);
         noTone(BUZZER);
      }
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
         if(digitalRead(ENTRADA_D) == LOW){
            digitalWrite(SALIDA_D, HIGH);
            tone(BUZZER, 500);
            delay(300);
            noTone(BUZZER);
            secuenciaUsuario[i] = SALIDA_D;
            flag = 1;
            delay(200);
            if(secuenciaUsuario[i] != secuencia[i]){
               secuenciaError();
               return;
            }
            digitalWrite(SALIDA_D, LOW);
         }
         if(digitalRead(ENTRADA_C) == LOW){
            digitalWrite(SALIDA_C, HIGH);
            tone(BUZZER, 400);
            delay(300);
            noTone(BUZZER);
            secuenciaUsuario[i] = SALIDA_C;
            flag = 1;
            delay(200);
            if(secuenciaUsuario[i] != secuencia[i]){
               secuenciaError();
               return;
            }
            digitalWrite(SALIDA_C, LOW);
         }
         if(digitalRead(ENTRADA_B) == LOW){
            digitalWrite(SALIDA_B, HIGH);
            tone(BUZZER, 300);
            delay(300);
            noTone(BUZZER);
            secuenciaUsuario[i] = SALIDA_B;
            flag = 1;
            delay(200);
            if(secuenciaUsuario[i] != secuencia[i]){
               secuenciaError();
               return;
            }
            digitalWrite(SALIDA_B, LOW);
         }
         if(digitalRead(ENTRADA_A) == LOW){
            digitalWrite(SALIDA_A, HIGH);
            tone(BUZZER, 200);
            delay(300);
            noTone(BUZZER);
            secuenciaUsuario[i] = SALIDA_A;
            flag = 1;
            delay(200);
            if(secuenciaUsuario[i] != secuencia[i]){
               secuenciaError();
               return;
            }
            digitalWrite(SALIDA_A, LOW);
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

void melodiaError(){
   for(int i = 0; i < 8; i++){
      int duracionNota = 1000/duracionNotas[i];
      tone(BUZZER, melodia[i],duracionNotas);
      int pausaEntreNotas = duracionNota * 1.30;
      delay(pausaEntreNotas);
      noTone(BUZZER);
   }
}

void secuenciaError(){
   digitalWrite(SALIDA_A, HIGH);
   digitalWrite(SALIDA_B, HIGH);
   digitalWrite(SALIDA_C, HIGH);
   digitalWrite(SALIDA_D, HIGH);
   delay(250);
   digitalWrite(SALIDA_A, LOW);
   digitalWrite(SALIDA_B, LOW);
   digitalWrite(SALIDA_C, LOW);
   digitalWrite(SALIDA_D, LOW);
   delay(250);
   melodiaError();
   nivelActual = 1;
   velocidad = 500;
}

void secuenciaCorrecta(){
   if(nivelActual < NIVEL_MAX);
      nivelActual++;
   velocidad -= 50;
   delay(200);
}

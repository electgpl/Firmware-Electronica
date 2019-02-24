#define feedbackADC A0
#define ajusteADC   A1
#define salidaPWM   5
int valFeedback = 0;
int valAjuste   = 0;
int duty=0;
void setup(){
   TCCR0B = TCCR0B & B11111000 | B00000001;
   pinMode(salidaPWM,OUTPUT);
   analogWrite(salidaPWM, 128);
}
void loop(){
   valFeedback = analogRead(feedbackADC);
   valAjuste   = analogRead(ajusteADC);
   if(valFeedback < valAjuste){
      if(duty>15)
         duty--;
      analogWrite(salidaPWM, duty);
   }else{
      if(duty<240)
         duty++;
      analogWrite(salidaPWM, duty);
   }
}

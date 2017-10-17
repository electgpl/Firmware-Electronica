int led_R=9, led_G=10, led_B=11,
    Mux_0=5, Mux_1=6, Mux_2=7;
void setup(){
   pinMode(led_R, OUTPUT);
   pinMode(led_G, OUTPUT);
   pinMode(led_B, OUTPUT);
   pinMode(Mux_0, OUTPUT);
   pinMode(Mux_1, OUTPUT);
   pinMode(Mux_2, OUTPUT);
}
void pwm_duty(int r, int g, int b){
   analogWrite(led_R, map(r,0,16,0,255));
   delay(10);
   analogWrite(led_G, map(g,0,16,0,255));
   delay(10);
   analogWrite(led_B, map(b,0,16,0,255));
   delay(10);
}
void output_c(int valor){
   switch(valor){
   case 0:
      digitalWrite(Mux_0, LOW);
      digitalWrite(Mux_1, LOW);
      digitalWrite(Mux_2, LOW);
      break;  
   case 1:
      digitalWrite(Mux_0, HIGH);
      digitalWrite(Mux_1, LOW);
      digitalWrite(Mux_2, LOW);
      break;  
   case 2:
      digitalWrite(Mux_0, LOW);
      digitalWrite(Mux_1, HIGH);
      digitalWrite(Mux_2, LOW);
      break;  
   case 3:
      digitalWrite(Mux_0, HIGH);
      digitalWrite(Mux_1, HIGH);
      digitalWrite(Mux_2, LOW);
      break;  
   case 4:
      digitalWrite(Mux_0, LOW);
      digitalWrite(Mux_1, LOW);
      digitalWrite(Mux_2, HIGH);
      break;  
   case 5:
      digitalWrite(Mux_0, HIGH);
      digitalWrite(Mux_1, LOW);
      digitalWrite(Mux_2, HIGH);
      break;  
   case 6:
      digitalWrite(Mux_0, LOW);
      digitalWrite(Mux_1, HIGH);
      digitalWrite(Mux_2, HIGH);
      break;  
   case 7:
      digitalWrite(Mux_0, HIGH);
      digitalWrite(Mux_1, HIGH);
      digitalWrite(Mux_2, HIGH);
      break;  
   }
}
void fadeRGB(void){
   int r, g, b, i;
   for(b=0;b<16;b++){
      pwm_duty(16,0,b);
      for(i=0;i<8;i++){
         output_c(i); 
         delay(1);
      }
   }
   for(r=16;r>1;r--){
      pwm_duty(r,0,16);
      for(i=0;i<8;i++){
         output_c(i); 
         delay(1);
      }
   }
   for(g=0;g<16;g++){
      pwm_duty(0,g,16);
      for(i=0;i<8;i++){
         output_c(i); 
         delay(1);
      }
   }
   for(b=16;b>1;b--){
      pwm_duty(0,16,b);
      for(i=0;i<8;i++){
         output_c(i); 
         delay(1);
      }
   } 
   for(r=0;r<16;r++){
      pwm_duty(r,16,0);
      for(i=0;i<8;i++){
         output_c(i); 
         delay(1);
      }
   }
   for(g=16;g>1;g--){
      pwm_duty(16,g,0);
      for(i=0;i<8;i++){
         output_c(i); 
         delay(1);
      }
   }
}
void mixRGB(void){
   pwm_duty(0,0,0);
   output_c(0);
   delay(1);
   pwm_duty(0,0,16);
   output_c(1);
   delay(1);
   pwm_duty(0,16,0);
   output_c(2);
   delay(1);
   pwm_duty(0,16,16);
   output_c(3);
   delay(1);
   pwm_duty(16,0,0);
   output_c(4);
   delay(1);
   pwm_duty(16,0,16);
   output_c(5);
   delay(1);
   pwm_duty(16,16,0);
   output_c(6);
   delay(1);
   pwm_duty(16,16,16);
   output_c(7);
   delay(1);
}
void flashRGB(void){
   pwm_duty(8,8,8);
   output_c(0);
   delay(100);
   pwm_duty(0,0,16);
   output_c(1);
   delay(100);
   pwm_duty(0,16,0);
   output_c(2);
   delay(100);
   pwm_duty(0,16,16);
   output_c(3);
   delay(100);
   pwm_duty(16,0,0);
   output_c(4);
   delay(100);
   pwm_duty(16,0,16);
   output_c(5);
   delay(100);
   pwm_duty(16,16,0);
   output_c(6);
   delay(100);
   pwm_duty(16,16,16);
   output_c(7);
   delay(100);
}
void coloresRGB(int r, int g, int b){
   int i;
   for(i=0;i<8;i++){
      pwm_duty(r,g,b);
      for(i=0;i<8;i++){
         output_c(i); 
         delay(1);
      }
   }
}
void loop(){
   int i, r, g, b;
   for(i=0;i<5;i++)
      fadeRGB();
   for(i=0;i<100;i++)
      mixRGB();
   for(i=0;i<10;i++)
      flashRGB();
   for(r=0;r<8;r++)
      for(i=0;i<100;i++)
         coloresRGB(b,g,r);
   for(g=0;g<8;g++)
      for(i=0;i<100;i++)
         coloresRGB(b,g,r);
   for(b=0;b<8;b++)
      for(i=0;i<100;i++)
         coloresRGB(b,g,r);
}
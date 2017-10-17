#include<16F883.h>
#use delay(int=4000000)
#fuses NOWDT
//Configuracion sistema operativo
#use rtos(timer=0,minor_cycle=1ms)
//Definicion de variables globales
int num[10]={0xFC,0x60,0xDA,0xF2,0x66,0xB6,0xBE,0xE0,0xFE,0xF6};
int secD=3,secU=0,buz=0,flag_2=1;
int1 flag_1=0;
//Funcion de sensado de displays
#task(rate=20ms,max=1ms)
void f_sensado();
//Funcion de cuenta 
#task(rate=850ms,max=1ms)
void f_cuenta();
//Funcion de buzzer
#task(rate=100ms,max=1ms)
void f_buzzer();
//Funcion de validacion
#task(rate=20ms,max=1ms)
void f_valida();
//Funcion de convinaciones
#task(rate=20ms,max=1ms)
void f_convina();
//Funcion principal
void main(){
   setup_timer_0(RTCC_INTERNAL|RTCC_DIV_4);
   set_tris_a(0b11111100);
   set_tris_b(0b00001000);
   set_tris_c(0b00000000);
   output_low(pin_a0);
   output_low(pin_a1);
   rtos_run();
}
//Funcion de sensado de displays
void f_sensado(){
   //output_b(0b00000001);
   output_low(pin_b1);
   output_high(pin_b0);
   output_c(num[secD]);
   delay_ms(1);
   //output_b(0x00);
   output_low(pin_b0);
   //output_b(0b00000010);
   output_high(pin_b1);
   output_c(num[secU]);
   delay_ms(1); 
   //output_b(0x00);   
   output_low(pin_b1);
}
//Funcion de cuenta con FAIL
void f_cuenta(){  
   if(flag_1==0&&flag_2!=6&&input(pin_b3)==1){
      if(secU--<1){
         secU=9;
         secD--;
      }
   }
}
//Funcion de buzzer con STOP y DESARME
void f_buzzer(){ 
   if(secD==0&&secU<5&&input(pin_b3)==1){
      if(++buz>2&&input(pin_b3)==1){
      output_toggle(pin_a1);
      buz=0;
      }
   }else{
      if(++buz==20&&input(pin_b3)==1){
         output_toggle(pin_a1);
      }else{
         if(buz>22&&input(pin_b3)==1){
         output_toggle(pin_a1);
         buz=3;
         }
      }   
   }
   if (input(pin_b3)==0||flag_2==6){
      output_low(pin_a1);
      buz=0;
   }
} 
//Funcion de validacion con FAIL
void f_valida(){
   if((secU==0&&secD==0)||input(pin_a2)==0||flag_1==1){
      output_high(pin_a0);
      sleep();
   }else
      output_low(pin_a0);   
}
//Funcion de convinaciones
void f_convina(){
   if(input(pin_a3)==0&&flag_2==1)
      flag_2=2;
   if(input(pin_a4)==0){     
      if(flag_2==2){
         flag_2=3;
      }else{
         if(flag_2<2&&input(pin_b3)==1)
            flag_1=1;
      }
   }
   if(input(pin_a5)==0){     
      if(flag_2==3){
         flag_2=4;
      }else{
         if(flag_2<3&&input(pin_b3)==1)
            flag_1=1;
      }
   }
   if(input(pin_a6)==0){     
      if(flag_2==4){
         flag_2=5;
      }else{
         if(flag_2<4&&input(pin_b3)==1)
            flag_1=1;
      }
   }
   if(input(pin_a7)==0){     
      if(flag_2==5){
         flag_2=6;
      }else{
         if(flag_2<5&&input(pin_b3)==1)
            flag_1=1;
      }
   }
}
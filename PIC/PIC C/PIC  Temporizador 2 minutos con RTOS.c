#include<16F883.h>
#use delay(int=4000000)
#fuses NOWDT
//Configuracion sistema operativo
#use rtos(timer=0,minor_cycle=1ms)
//Definicion de variables globales
int num[10]={0xFC,0x60,0xDA,0xF2,0x66,0xB6,0xBE,0xE0,0xFE,0xF6};
int secD=5,secU=9,buz=0;
int1 flag_1=0;
//Funcion de sensado de displays
#task(rate=20ms,max=1ms)
void funcion1();
//Funcion de cuenta 
#task(rate=850ms,max=1ms)
void funcion2();
//Funcion de buzzer
#task(rate=100ms,max=1ms)
void funcion3();
//Funcion de validacion
#task(rate=20ms,max=1ms)
void funcion4();
//Funcion principal
void main(){
   setup_timer_0(RTCC_INTERNAL|RTCC_DIV_4);
   output_low(pin_a0);
   rtos_run();
}  
//secuencia de desarme
void convinacion(void){
   flag_1=0;
   if(input(pin_a3)==0)
      if(input(pin_a4)==0)
         if(input(pin_a5)==0)
            if(input(pin_a6)==0)
               if(input(pin_a7)==0)
                  flag_1=1;
} 
//Funcion de sensado de displays
void funcion1(){
   output_b(0b00000001);
   output_c(num[secD]);
   delay_ms(1);
   output_b(0x00);
   convinacion();
   output_b(0b00000010);   
   output_c(num[secU]);
   delay_ms(1); 
   output_b(0x00);   
   convinacion();
}
//Funcion de cuenta
void funcion2(){  
   if(flag_1==0){
      if(secU--<1){
         secU=9;
         if(secD--<1){ 
            secD=5;  
         }
      }
   }
}
//Funcion de buzzer
void funcion3(){ 
   if(flag_1==0){
      if(secD==0&&secU<5){
         output_toggle(pin_a1);
      }else{
         if(++buz>2){
            output_toggle(pin_a1);
            buz=0;
         }   
      }  
   }   
} 
//Funcion de validacion
void funcion4(){
   if((secU==0&&secD==0)||input(pin_a2)==0){
      output_high(pin_a0);
      sleep();
   }else
      output_low(pin_a0);   
}
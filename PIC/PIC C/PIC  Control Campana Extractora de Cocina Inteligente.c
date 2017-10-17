//*******************************************************
#include <16F883.h>
#FUSES NOWDT
#FUSES HS
#FUSES MCLR
#use delay(clock=4000000)
#include <LCD.C>
#use rtos(timer=0,minor_cycle=1ms)
//Macros de Salidas
#define SALIDA_LAMPARA  PIN_C1
#define SALIDA_LEDS     PIN_C2
#define SALIDA_BL_LCD   PIN_C3
#define SALIDA_MOTOR_V1 PIN_C4
#define SALIDA_MOTOR_V2 PIN_C5
#define SALIDA_MOTOR_V3 PIN_C6
#define SALIDA_BUZZER   PIN_C7
//Variables globales
int vel_A1=0, vel_A2=0, con_A2=0, con_A21=0, con_A3=0, timer_A1=0, 
    con_A31=0, con_A4=0, con_A41=0, valor=0, timer_A4=0, timer_A3=0;
int1 flag_A2=0, flag_A21=0, flag_A3=0, flag_A4=0, flag_motor=0;
//Funcion LCD
#task(rate=10ms,max=1ms)
void f_lcd();
//Funcion Boton1, Enciende o apaga el extractor
#task(rate=10ms,max=1ms)
void f_boton1();
//Funcion Boton2, Maneja 3 velocidades del extractor, 
//manteniendolo presionado salta a la mas alta.
#task(rate=10ms,max=1ms)
void f_boton2();
//Funcion Boton3, Enciende o apaga la luz, 
//manteniendolo presionado enciende la luz con 
//temporizador de apagado automatico.
#task(rate=10ms,max=1ms)
void f_boton3();
//Funcion Boton4, Enciende el extractor con temporizador 
//de apagado automatico.
#task(rate=10ms,max=1ms)
void f_boton4();
//Funcion Boton5, Alarma acustia para cocina, 
//activa incrementando tiempo.
#task(rate=10ms,max=1ms)
void f_boton5();
//Funcion Boton6, Reset
#task(rate=10ms,max=1ms)
void f_boton6();
//Funcion principal
void main(){
   setup_timer_0(RTCC_INTERNAL|RTCC_DIV_1);
   output_low(SALIDA_BUZZER);
   output_high(SALIDA_LEDS);
   output_low(SALIDA_BL_LCD);
   output_low(SALIDA_MOTOR_V1);
   output_low(SALIDA_MOTOR_V2);
   output_low(SALIDA_MOTOR_V3);
   output_low(SALIDA_LAMPARA);
   lcd_init();
   rtos_run();
}
//Funcion LCD
void f_lcd(){
   valor=0;
   if(                    flag_A21) valor=1;
   if(          flag_A3)            valor=2;
   if(          flag_A3 & flag_A21) valor=3;
   if(flag_A4)                      valor=4;
   if(flag_A4 &           flag_A21) valor=5;
   if(flag_A4 & flag_A3)            valor=6;
   if(flag_A4 & flag_A3 & flag_A21) valor=7;
   if((input(PIN_A2)==1)&
      (input(PIN_A3)==1)&
      (input(PIN_A4)==1)){
      switch(valor){
      case 1:
         lcd_gotoxy(1,1);
         printf(lcd_putc," Timer Activado ");
         lcd_gotoxy(1,2);
         printf(lcd_putc,"       Luz      ");
         break;
      case 2:
         lcd_gotoxy(1,1);
         printf(lcd_putc," Timer Activado ");
         lcd_gotoxy(1,2);
         printf(lcd_putc,"   Extractor    ");
         break;
      case 3:
         lcd_gotoxy(1,1);
         printf(lcd_putc," Timer Activado ");
         lcd_gotoxy(1,2); 
         printf(lcd_putc,"Luz y Extractor ");
         break;
      case 4:
         lcd_gotoxy(1,1);
         printf(lcd_putc," Timer Activado ");
         lcd_gotoxy(1,2);
         printf(lcd_putc,"     Alarma     ");
         break;
      case 5:
         lcd_gotoxy(1,1);
         printf(lcd_putc," Timer Activado ");
         lcd_gotoxy(1,2); 
         printf(lcd_putc,"  Luz y Alarma  ");
         break;
      case 6:
         lcd_gotoxy(1,1);
         printf(lcd_putc," Timer Activado ");
         lcd_gotoxy(1,2); 
         printf(lcd_putc,"Extrac. y Alarm.");
         break;
      case 7:
         lcd_gotoxy(1,1);
         printf(lcd_putc," Timer Activado ");
         lcd_gotoxy(1,2);
         printf(lcd_putc,"Extr, Alar y Luz");
         break;
      default:
         break;
      }
   }
}   
//Funcion Boton1, Enciende o apaga el extractor
void f_boton1(){
   if(input(PIN_A0)==0){
      delay_ms(50);
      if(input(PIN_A0)==0){
         output_high(SALIDA_BL_LCD);
         if(flag_motor){
            lcd_gotoxy(1,1);
            printf(lcd_putc,"   Extractor    ");
            lcd_gotoxy(1,2);
            printf(lcd_putc,"    Apagado     ");
            flag_motor=FALSE;
            output_low(SALIDA_MOTOR_V1);
            output_low(SALIDA_MOTOR_V2);
            output_low(SALIDA_MOTOR_V3);
            delay_ms(500);
            con_A31=0;
            flag_A3=0;
            lcd_init();
         }else{
            lcd_gotoxy(1,1);
            printf(lcd_putc,"   Extractor    ");
            lcd_gotoxy(1,2);
            printf(lcd_putc,"   Encendido    ");
            flag_motor=TRUE;
            output_low(SALIDA_MOTOR_V1);
            output_high(SALIDA_MOTOR_V2);
            output_low(SALIDA_MOTOR_V3);
            delay_ms(500);
         }  
      }
   }
}
//Funcion Boton2, Maneja 3 velocidades del extractor, 
//manteniendolo presionado salta a la mas alta.
void f_boton2(){
   if(input(PIN_A1)==0){
      delay_ms(100);
      timer_A1++;
      if(timer_A1>5){
         timer_A1=0;
         output_high(SALIDA_BL_LCD);
         output_low(SALIDA_MOTOR_V1);
         output_low(SALIDA_MOTOR_V2);
         output_high(SALIDA_MOTOR_V3);  
         flag_motor=TRUE;
         lcd_gotoxy(1,1);
         printf(lcd_putc,"   Extractor    ");
         lcd_gotoxy(1,2);
         printf(lcd_putc,"Maxima Velocidad");
         delay_ms(500);
         lcd_init();
      }
      delay_ms(400);
      if((input(PIN_A1)==1)&&(timer_A1>0)){
         output_high(SALIDA_BL_LCD);
         ++vel_A1;
         if(vel_A1<4){
            lcd_gotoxy(1,1);
            printf(lcd_putc,"   Extractor    ");
            lcd_gotoxy(1,2);
            printf(lcd_putc,"  Velocidad: %d ",vel_A1);
            delay_ms(450);
            switch(vel_A1){
            case 1:
               flag_motor=TRUE;
               output_high(SALIDA_MOTOR_V1);
               output_low(SALIDA_MOTOR_V2);
               output_low(SALIDA_MOTOR_V3);
               break;
            case 2:
               flag_motor=TRUE;
               output_low(SALIDA_MOTOR_V1);
               output_high(SALIDA_MOTOR_V2);
               output_low(SALIDA_MOTOR_V3);
               break;
            case 3:
               flag_motor=TRUE;
               output_low(SALIDA_MOTOR_V1);
               output_low(SALIDA_MOTOR_V2);
               output_high(SALIDA_MOTOR_V3);
               break;
            default: 
               flag_motor=FALSE;
               output_low(SALIDA_MOTOR_V1);
               output_low(SALIDA_MOTOR_V2);
               output_low(SALIDA_MOTOR_V3);
               output_low(SALIDA_BL_LCD);
               break;
            }
         }else{
            output_low(SALIDA_BL_LCD);
            vel_A1=0;
         }
      }
   }
}
//Funcion Boton3, Enciende o apaga la luz, 
//manteniendolo presionado enciende la luz con 
//temporizador de apagado automatico.
void f_boton3(){
   if(input(PIN_A2)==0){
      delay_ms(50);
      if(input(PIN_A2)==1)
         vel_A2=0;
      vel_A2++;
      output_high(SALIDA_BL_LCD);
      if(input(PIN_A2)==0){
         if(vel_A2<3){
            if(flag_A2==0){
               flag_A2=1;
               flag_A21=0;
               delay_ms(50);
               lcd_gotoxy(1,1);
               printf(lcd_putc,"  Iluminacion   ");
               lcd_gotoxy(1,2);
               printf(lcd_putc,"   Encendida    ");
               output_high(SALIDA_LAMPARA);
               delay_ms(500);
            }else{
               flag_A2=0;
               flag_A21=0;
               delay_ms(50);
               lcd_gotoxy(1,1);
               printf(lcd_putc,"  Iluminacion   ");
               lcd_gotoxy(1,2);
               printf(lcd_putc,"    Apagada     ");
               output_low(SALIDA_LAMPARA);
               delay_ms(500);
               lcd_init();
            }   
         }else{
            if(input(PIN_A2)==0){
               ++con_A2;
               if(con_A2>10){
                  lcd_gotoxy(1,1);
                  printf(lcd_putc,"  Timer de Luz  ");
                  lcd_gotoxy(1,2); 
                  printf(lcd_putc," Tiempo: 10 min "); 
                  delay_ms(100);
                  flag_A2=1;
                  con_A21=600;
                  lcd_init();
                  flag_A21=1;
               }
            }
         }
      }
   }else{
      if((input(PIN_A2)==1)&
         (input(PIN_A3)==1)&
         (input(PIN_A4)==1)&
         (flag_A21)){
         output_high(SALIDA_LAMPARA);
         flag_A2=1;
         delay_ms(800);
         con_A21--;
         if(con_A21<1){
            flag_A21=0;
            con_A21=0;
            output_low(SALIDA_LAMPARA);
         } 
      }
   }   
}
//Funcion Boton4, Enciende el extractor con temporizador 
//de apagado automatico.
void f_boton4(){
   if(input(PIN_A3)==0){
      output_high(SALIDA_BL_LCD);
      delay_ms(50);
      timer_A3++;
      if(timer_A3>10){
         flag_A3=0;      
         timer_A3=0;
         con_A3=0;
         con_A31=0;
         output_low(SALIDA_MOTOR_V1);
         output_low(SALIDA_MOTOR_V2);
         output_low(SALIDA_MOTOR_V3); 
         flag_motor=FALSE;
         lcd_init();
      }
      delay_ms(400);
      if((input(PIN_A3)==1)&&(timer_A3>0)){
         if(con_A3<60){
            ++con_A3;
            lcd_gotoxy(1,1);
            printf(lcd_putc," Timer Extractor ");
            lcd_gotoxy(1,2);
            printf(lcd_putc," Tiempo: %d min",con_A3*5); 
            con_A31=con_A3*300;
            delay_ms(800);
            lcd_init();
            flag_A3=1;
         }else{
            delay_ms(100);
            con_A3=0;
         }
      }
   }else{
      if((input(PIN_A2)==1)&
         (input(PIN_A3)==1)&
         (input(PIN_A4)==1)&
         (flag_A3)){
         flag_motor=TRUE;
         output_low(SALIDA_MOTOR_V1);
         output_high(SALIDA_MOTOR_V2);
         output_low(SALIDA_MOTOR_V3);
         delay_ms(800);
         con_A31--;
         if(con_A31<1 || flag_motor==FALSE){
            flag_A3=0;
            con_A31=0;
            flag_motor=FALSE;
            output_low(SALIDA_MOTOR_V1);
            output_low(SALIDA_MOTOR_V2);
            output_low(SALIDA_MOTOR_V3);
         }           
      }
   }   
}   
//Funcion Boton5, Alarma acustia para cocina, 
//activa incrementando tiempo.
void f_boton5(){
   if(input(PIN_A4)==0){
      output_high(SALIDA_BL_LCD);
      delay_ms(50);
      timer_A4++;
      if(timer_A4>5){
         flag_A4=0;      
         timer_A4=0;
         con_A4=0;
         con_A41=0;
         lcd_init();
      }
      delay_ms(400);
      if((input(PIN_A4)==1)&&(timer_A4>0)){
         if(con_A4<60){
            ++con_A4;
            lcd_gotoxy(1,1);
            printf(lcd_putc," Timer de Alarma ");
            lcd_gotoxy(1,2); 
            printf(lcd_putc," Tiempo: %d min",con_A4); 
            con_A41=con_A4*60;
            delay_ms(800);
            lcd_init();
            flag_A4=1;
         }else{
            delay_ms(100);
            con_A4=0;
         }
      }   
   }else{
      if((input(PIN_A2)==1)&
         (input(PIN_A3)==1)&
         (input(PIN_A4)==1)&
         (flag_A4)){
         delay_ms(800);
         con_A41--;
         if(con_A41<1){
            flag_A4=0;
            con_A41=0;
            output_high(SALIDA_BUZZER);
            delay_ms(500);
            output_low(SALIDA_BUZZER);
            delay_ms(500);
            output_high(SALIDA_BUZZER);
            delay_ms(500);
            output_low(SALIDA_BUZZER);
            delay_ms(500);
            output_high(SALIDA_BUZZER);
            delay_ms(500);
            output_low(SALIDA_BUZZER);
            delay_ms(500);
            output_low(SALIDA_BL_LCD);
            con_A41=0;
            con_A4=0;
         }
      }
   }   
}
//Funcion Boton6, Reset
void f_boton6(){
   if(input(PIN_A5)==0){
      delay_ms(50);
      if(input(PIN_A5)==0){
         reset_cpu();
         output_low(SALIDA_BL_LCD);
      }   
   }      
}
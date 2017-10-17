#include <16F883.h>
#use delay(clock=4000000)
#use rs232(baud=9600,parity=N,xmit=PIN_C6,rcv=PIN_C7,bits=8)
#use rtos(timer=0, minor_cycle=1ms)
#task (rate=20ms, max=1ms) 
void func1();
#task (rate=20ms, max=1ms)
void func2();
char dato;
void main(){
   setup_timer_0(RTCC_INTERNAL|RTCC_DIV_4);  
   printf("*************************************************\r");
   printf("*        BoARd - Control Eléctrico 1.1          *\r");
   printf("* CONTROL ELECTRICO POR INTERFAZ SERIE 232/485  *\r");
   printf("*-----------------------------------------------*\r");
   printf("* Q:SALIDA1 | W:SALIDA2 | E:SALIDA3 | R:SALIDA4 *\r");
   printf("*************************************************\r");
   rtos_run();
}
void func1(){  
   if(kbhit())
      dato=getc();
}  
void func2(){
   switch(dato){
   case 'q':      
      output_high(PIN_B0);
      break;
   case 'w':
      output_high(PIN_B1);
      break;
   case 'e':
      output_high(PIN_B2);
      break;
   case 'r':
      output_high(PIN_B3);
      break;     
   case 'a':      
      output_low(PIN_B0);
      break;
   case 's':
      output_low(PIN_B1);
      break;
   case 'd':
      output_low(PIN_B2);
      break;
   case 'f':
      output_low(PIN_B3);
      break;          
   default:
      break;
   } 
}
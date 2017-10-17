#include <16F883.h>
#device adc=10
#FUSES XT,NOWDT
#use delay(clock=4000000)
#include <LCD.C>
void main(){
   int16 valorADC, frecuencia;
   int menu=1;
   float corriente, tension, diodos, temperatura, capacitancia;
   setup_adc_ports(sAN0|VSS_VDD);
   setup_adc(ADC_CLOCK_INTERNAL);
   lcd_init();
   while(TRUE){       
      if(input(PIN_C1)==1){
         delay_ms(200);
         menu++;
         if(menu>7)
         menu=1;
      }     
      switch(menu){
      case 1:
         lcd_gotoxy(1,1);
         printf(lcd_putc,"VOLTIMETRO    DC");
         output_low(pin_c4);
         output_low(pin_c5);
         output_low(pin_c7);    
         set_adc_channel(0);
         delay_us(20);
         valorADC = read_adc();
         tension = 5.0 * valorADC / 1024.0;         
         lcd_gotoxy(1,2);
         printf(lcd_putc,"VM:10V    %1.3fV",tension*2);
         break;
      case 2:
         lcd_gotoxy(1,1);
         printf(lcd_putc,"AMPERIMETRO   DC");
         output_high(pin_c4);
         output_low(pin_c5);
         output_low(pin_c7); 
         set_adc_channel(0);
         delay_us(20);
         valorADC = read_adc();
         corriente = (5.0 * valorADC / 1024.0) * 0.2118;   
         lcd_gotoxy(1,2);
         printf(lcd_putc,"IM:1A     %1.3fA",corriente);
         break;
      case 3:
         lcd_gotoxy(1,1);
         printf(lcd_putc,"DIODOS Y LEDS   ");
         output_low(pin_c4);
         output_high(pin_c5);
         output_low(pin_c7);
         set_adc_channel(0);
         delay_us(20);
         valorADC = read_adc();
         diodos = 5.0 * valorADC / 1024.0;         
         lcd_gotoxy(1,2);
         printf(lcd_putc,"          %1.3fV",diodos);
         break; 
      case 4:
         lcd_gotoxy(1,1);
         printf(lcd_putc,"TEMPERATURA     ");
         output_high(pin_c4);
         output_high(pin_c5);
         output_low(pin_c7);
         set_adc_channel(0);
         delay_us(20);
         valorADC = read_adc();
         temperatura = (5.0 * valorADC / 1024.0) * 100;         
         lcd_gotoxy(1,2);
         printf(lcd_putc,"          %2.1fâ,temperatura);
         break;   
      case 5:
         lcd_gotoxy(1,1);
         printf(lcd_putc,"CAPACITANCIA     ");
         output_low(pin_c4);
         output_low(pin_c5);
         output_high(pin_c7);
         set_timer1(0); 
         setup_timer_1(T1_EXTERNAL|T1_DIV_BY_1); 
         delay_ms(1000); 
         setup_timer_1(T1_DISABLED); 
         frecuencia=get_timer1(); 
         capacitancia = (7.178 / frecuencia) * 1000000;
         lcd_gotoxy(1,2);
         if(capacitancia > 1000)
            printf(lcd_putc,"        %4.1f nF ",capacitancia/1000); 
         else   
            printf(lcd_putc,"        %4.1f pF ",capacitancia);         
         break;           
      case 6:
         lcd_gotoxy(1,1);
         printf(lcd_putc,"FRECUENCIMETRO  ");
         output_high(pin_c4);
         output_low(pin_c5);
         output_high(pin_c7);
         set_timer1(0); 
         setup_timer_1(T1_EXTERNAL|T1_DIV_BY_1); 
         delay_ms(1000); 
         setup_timer_1(T1_DISABLED); 
         frecuencia=get_timer1(); 
         lcd_gotoxy(1,2);
         printf(lcd_putc,"       %6lu Hz",frecuencia);
         break;          
      case 7:
         lcd_gotoxy(1,1);
         printf(lcd_putc,"CONTINUIDAD     ");
         output_low(pin_c4);
         output_high(pin_c5);
         output_high(pin_c7);
         lcd_gotoxy(1,2);
         if(input(PIN_C2)==0){
            printf(lcd_putc,"CIRCUITO CERRADO");
            output_high(pin_c3);
         }
         else{   
            printf(lcd_putc,"CIRCUITO ABIERTO");
            output_low(pin_c3);
         }            
         break;         
      default:
         lcd_gotoxy(1,1);
         printf(lcd_putc,"    **ERROR**   ");
         lcd_gotoxy(1,2);
         printf(lcd_putc,"   [REINICIAR]  ");      
      }
   }
}
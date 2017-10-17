#include <16f883.h>                       
#use delay(int=4000000)                   
#fuses XT,NOWDT                           
#include <LCD.C>                          
char teclaPulsada=0;                      
#byte PORTC = 0x07                        
void leerTeclado(){                       
   output_c(0b11111110);                  
   if(PORTC==0b11101110) teclaPulsada='7';
   if(PORTC==0b11011110) teclaPulsada='4';
   if(PORTC==0b10111110) teclaPulsada='1';
   if(PORTC==0b01111110) teclaPulsada='C';
   delay_ms(50);                          
   output_c(0b11111101);                  
   if(PORTC==0b11101101) teclaPulsada='8';
   if(PORTC==0b11011101) teclaPulsada='5';
   if(PORTC==0b10111101) teclaPulsada='2';
   if(PORTC==0b01111101) teclaPulsada='0';
   delay_ms(50);                          
   output_c(0b11111011);                  
   if(PORTC==0b11101011) teclaPulsada='9';
   if(PORTC==0b11011011) teclaPulsada='6';
   if(PORTC==0b10111011) teclaPulsada='3';
   if(PORTC==0b01111011) teclaPulsada='=';
   delay_ms(50);                          
   output_c(0b11110111);                  
   if(PORTC==0b11100111) teclaPulsada='/';
   if(PORTC==0b11010111) teclaPulsada='X';
   if(PORTC==0b10110111) teclaPulsada='-';
   if(PORTC==0b01110111) teclaPulsada='+';
   delay_ms(50);                          
}                                         
void main(void){                          
   lcd_init();                            
   lcd_gotoxy(1,1);                       
   printf(lcd_putc,"Electgpl Teclado");   
   while(TRUE){                           
      leerTeclado();                      
      lcd_gotoxy(1,2);                    
      printf(lcd_putc,"TECLA...:  %c  ",teclaPulsada);
   }
}
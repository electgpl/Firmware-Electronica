//Telefono Celular para modulo SIM900
//Comando para llamar: ATD + [NUMERO] 
//Comando para colgar: ATH0
//LCD 16x2 en 4bit
//Teclado 4x4
//Botones de llamada y borrado
#include <16f883.h>
#use delay(int=4000000)
#fuses XT,NOWDT
#include <LCD.C>
#use rs232(baud=9600,xmit=pin_a0,rcv=pin_a1,bits=8,parity=N)
char teclaPulsada=0;
#byte PORTC = 0x07
void leerTeclado(){
   teclaPulsada='x';
   output_c(0b11111110);
   if(PORTC==0b11101110) teclaPulsada='1';
   if(PORTC==0b11011110) teclaPulsada='4';
   if(PORTC==0b10111110) teclaPulsada='7';
   if(PORTC==0b01111110) teclaPulsada='*';
   delay_ms(50);
   output_c(0b11111101);
   if(PORTC==0b11101101) teclaPulsada='2';
   if(PORTC==0b11011101) teclaPulsada='5';
   if(PORTC==0b10111101) teclaPulsada='8';
   if(PORTC==0b01111101) teclaPulsada='0';
   delay_ms(50);    
   output_c(0b11111011);
   if(PORTC==0b11101011) teclaPulsada='3';
   if(PORTC==0b11011011) teclaPulsada='6';
   if(PORTC==0b10111011) teclaPulsada='9';
   if(PORTC==0b01111011) teclaPulsada='#';
   delay_ms(50);
}
void borrado(){
   lcd_gotoxy(1,1);
   printf(lcd_putc,"Cortando...");
   delay_ms(100);
   printf("ATH0;");
}
void main(void){
   inicio:
   char telefono[11];
   lcd_init();
   lcd_gotoxy(1,1);
   printf(lcd_putc,"    Electgpl    ");
   lcd_gotoxy(1,2);
   printf(lcd_putc,"Telefono Celular");
   delay_ms(1500);
   lcd_init();
   lcd_gotoxy(1,1);
   printf(lcd_putc,"Listo...");
   delay_ms(500);
   lcd_init();   
   lcd_gotoxy(1,2); 
   printf(lcd_putc,"[CALL]     [END]");
   int i=0;
   while(TRUE){
      leerTeclado();
      if(teclaPulsada!='x'){
         telefono[i]=teclaPulsada;
         i++;
      }   
      lcd_gotoxy(1,1);
      printf(lcd_putc,"%s",telefono); 
      if(input(PIN_A2)==1){
         telefono="         ";
         borrado();
         goto inicio;
      }   
      if(input(PIN_A1)==1){
         if(input(PIN_A2)==1){
            telefono="         ";
            borrado();
            goto inicio;
         }
         printf("ATD + %s;",telefono);
         i=0;   
      }   
   }
}
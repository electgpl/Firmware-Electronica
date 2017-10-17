#include <16F628A.h>
#FUSES NOWDT
#FUSES HS
#FUSES NOPUT
#FUSES NOPROTECT
#FUSES NOBROWNOUT
#FUSES NOMCLR
#FUSES NOLVP
#FUSES NOCPD
#use delay(clock=4000000)
#define LCD_ENABLE_PIN  PIN_B7
#define LCD_RS_PIN      PIN_B4
#define LCD_RW_PIN      PIN_B5
#define LCD_DATA4       PIN_B0
#define LCD_DATA5       PIN_B1
#define LCD_DATA6       PIN_B2
#define LCD_DATA7       PIN_B3
#include <LCD.C>
void main(){
   lcd_init();
   lcd_gotoxy(1,1);
   printf(lcd_putc,"Calculando...");
   int16 pulsos;
   while(true){
      set_timer1(0);
      setup_timer_1(T1_EXTERNAL|T1_DIV_BY_1);
      delay_ms(10500);
      setup_timer_1(T1_DISABLED);
      pulsos=get_timer1();
      lcd_init(); 
      lcd_gotoxy(1,1);
      printf(lcd_putc,"Frec.Card.: %3lu",pulsos*6);
      if(pulsos*6>110){
         lcd_gotoxy(1,2);
         printf(lcd_putc,"Taquicardia");
      }else 
         if(pulsos*6<70){
            lcd_gotoxy(1,2);
            printf(lcd_putc,"Bradicardia");
         }else{
            lcd_gotoxy(1,2);
            printf(lcd_putc,"Normal");
         }  
   }
}
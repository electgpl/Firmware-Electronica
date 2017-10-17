#include <16F883.h>
#use delay(clock=4000000)
#include <lcd420.c>
void main(){
   int1 flag1=0, flag2=0;
   lcd_init();
   output_low(PIN_A3);
   output_low(PIN_A4);
   while(TRUE){
      if(input(PIN_A0)){
         flag1=1; 
      }else{
         if(flag2==0){
            lcd_gotoxy(1,1);
            printf(lcd_putc," |--| |--[ ]---( )-|");
            lcd_gotoxy(1,2);
            printf(lcd_putc," |--| |--------( )-|");
            lcd_gotoxy(1,3);
            printf(lcd_putc," |--| |--|          ");
            lcd_gotoxy(1,4);
            printf(lcd_putc," |--| |------(RST)-|");            
            delay_ms(1000);
         }
         while(flag1==1&&flag2==0&&input(PIN_A0)){
            output_high(PIN_A3);
            output_high(PIN_A4);
            lcd_gotoxy(1,1);
            printf(lcd_putc,">|--|/|--[ ]---(*)-|");
            lcd_gotoxy(1,2);
            printf(lcd_putc," |--| |--------( )-|");
            lcd_gotoxy(1,3);
            printf(lcd_putc," |--| |--|          ");
            lcd_gotoxy(1,4);
            printf(lcd_putc," |--| |------(RST)-|");
            delay_ms(1000);
            lcd_gotoxy(1,1);
            printf(lcd_putc," |--| |--[3]---(*)-|");
            lcd_gotoxy(1,2);
            printf(lcd_putc,">|--|/|--------(*)-|");
            lcd_gotoxy(1,3);
            printf(lcd_putc,">|--|/|--|          ");
            lcd_gotoxy(1,4);
            printf(lcd_putc," |--| |------(RST)-|");
            delay_ms(1000);
            lcd_gotoxy(1,1);
            printf(lcd_putc," |--| |--[2]---(*)-|");
            lcd_gotoxy(1,2);
            printf(lcd_putc,">|--|/|--------(*)-|");
            lcd_gotoxy(1,3);
            printf(lcd_putc,">|--|/|--|          ");
            lcd_gotoxy(1,4);
            printf(lcd_putc," |--| |------(RST)-|");
            delay_ms(1000);
            lcd_gotoxy(1,1);
            printf(lcd_putc," |--| |--[1]---(*)-|");
            lcd_gotoxy(1,2);
            printf(lcd_putc,">|--|/|--------(*)-|");
            lcd_gotoxy(1,3);
            printf(lcd_putc,">|--|/|--|          ");  
            lcd_gotoxy(1,4);
            printf(lcd_putc," |--| |------(RST)-|");
            delay_ms(1000);
            output_low(PIN_A3);
            lcd_gotoxy(1,1);
            printf(lcd_putc," |--| |--[ ]---( )-|");
            lcd_gotoxy(1,2);
            printf(lcd_putc," |--| |--------(*)-|");
            lcd_gotoxy(1,3);
            printf(lcd_putc,">|--|/|--|          ");    
            lcd_gotoxy(1,4);
            printf(lcd_putc," |--| |------(RST)-|");
            flag2=1;
         }
      }
      if(input(PIN_A1)){
         output_low(PIN_A3);
         output_low(PIN_A4);
         lcd_gotoxy(1,1);
         printf(lcd_putc," |--| |--[ ]---( )-|");
         lcd_gotoxy(1,2);
         printf(lcd_putc," |--| |--------( )-|");
         lcd_gotoxy(1,3);
         printf(lcd_putc," |--| |--|          "); 
         lcd_gotoxy(1,4);
         printf(lcd_putc,">|--|/|------(RST)-|");
         delay_ms(1000);
         reset_cpu();
      }else{
         lcd_gotoxy(1,4);
         printf(lcd_putc," |--| |------(RST)-|");
      }
   }   
}
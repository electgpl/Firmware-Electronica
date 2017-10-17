#include <16F883.h>
#device adc=8
#FUSES NOWDT
#FUSES INTRC_IO
#FUSES NOPUT
#FUSES MCLR
#FUSES NOBROWNOUT
#FUSES NOLVP
#FUSES NOCPD
#FUSES NOWRT
#FUSES NODEBUG
#FUSES NOPROTECT
#use delay(int=4000000)
#define RTC_RST    PIN_A0
#define RTC_SCLK   PIN_A1
#define RTC_IO     PIN_A2
#include <DS1302.C>
#include <LCD.C>
void main()
{
int HH, MIN, SS;
int HHSet, MMset;
int HHAlarm, MMAlarm;
int i;
   lcd_init();
   lcd_putc("Electgpl Clock");
   delay_ms(1000);
   rtc_init();
   lcd_init();
   while(1)
   {
   if (input(PIN_A5)==1)
      {
      if (input(PIN_A4)==1) 
         {
         lcd_gotoxy(15,1);
         lcd_putc("ST");
         rtc_set_datetime(0,0,0,0,HHSet,MMSet);
         MMSet = MMSet + 1;
         if (MMSet > 59)
            {
            delay_ms(100);
            MMSet = 0;
            HHSet = HHSet + 1;
            }  
            if (HHSet > 23)
               {
               HHSet = 0;
               } 
         lcd_gotoxy(1,1);
         printf(lcd_putc,"HORA %02d:%02d:00   ",HHSet,MMSet);               
         }
      else
         {
         if (input(PIN_A3)==1) 
            {
            lcd_gotoxy(15,1);
            lcd_putc("AL");
            MMAlarm = MMAlarm + 1;
            if (MMAlarm > 59)
               {
               delay_ms(100);
               MMAlarm = 0;
               HHAlarm = HHAlarm + 1;
               }  
               if (HHAlarm > 23)
                  {
                  HHAlarm = 0;
                  } 
            lcd_gotoxy(1,1);
            printf(lcd_putc,"HORA %02d:%02d:00   ",HHAlarm,MMAlarm);    
            write_eeprom(8,HHAlarm);
            write_eeprom(9,MMAlarm);
            } 
         } 
      }   
   else     
      {
      rtc_get_time(HH,MIN,SS);
      lcd_gotoxy(1,1);
      printf(lcd_putc,"HORA %02d:%02d:%02d   ",HH,MIN,SS);
      HHAlarm=read_eeprom(8);
      MMAlarm=read_eeprom(9);
      lcd_gotoxy(1,2);
      printf(lcd_putc,"ALARMA  %02d:%02d   ",HHAlarm,MMAlarm);
      if((HH == HHAlarm) && (MIN == MMAlarm))
         output_high(pin_a6);
      else
         output_low(pin_a6);       
      }
   }   
}
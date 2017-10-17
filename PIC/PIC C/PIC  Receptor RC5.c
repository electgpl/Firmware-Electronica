//LCD module connections
#define LCD_RS_PIN PIN_B1
#define LCD_RW_PIN PIN_B2
#define LCD_ENABLE_PIN PIN_B3
#define LCD_DATA4 PIN_B4
#define LCD_DATA5 PIN_B5
#define LCD_DATA6 PIN_B6
#define LCD_DATA7 PIN_B7
//End LCD module connections

#include <16F84A.h>
#fuses HS,NOWDT,PUT,NOPROTECT
#use delay(crystal=8000000)
#include <lcd.c>
#use fast_io(B)

int1 toggle;
unsigned int8 count, i, address, command;
void main(){
  port_b_pullups(TRUE);                  // Enable PORTB pull-ups
  output_b(0);                           // PORTB initial state
  set_tris_b(1);
  lcd_init();                            // Initialize LCD module
  lcd_putc('\f');                        // LCD clear
  lcd_gotoxy(3, 1);                      // Go to column 3 row 1
  lcd_putc("RC5 protocol");
  lcd_gotoxy(5, 2);                      // Go to column 5 row 2
  lcd_putc("decoder");
  while(TRUE){
    ret:
    count = 0;
    while(input(PIN_B0));                // Wait until RB0 pin falls
    // Check if the received signal uses RC5 protocol
    while((input(PIN_B0) == 0) && (count < 20)){
      count++;
      delay_us(50);}
    if( (count > 20) || (count < 14))      // Signal doesn't use RC5 protocol
      goto ret;                            // Return
    count = 0;
    while((input(PIN_B0)) && (count < 20)){
      count++;
      delay_us(50);}
    if( (count > 20) || (count < 14))      // Signal doesn't use RC5 protocol
      goto ret;                            // Return
    count = 0;
    while((input(PIN_B0) == 0) && (count < 40)){
      count++;
      delay_us(50);}
    if( (count > 40) || (count < 14))      // Signal doesn't use RC5 protocol
      goto ret;                            // Return
    // End check (The received signal uses RC5 protocol)
    if(count > 30)
      delay_us(400);
    else
      delay_us(1300);
    for(i = 0; i < 12; i++){
      if(i == 0){
        if(input(PIN_B0) == 1)   toggle = 0;
        else                     toggle = 1;
        }
      else {
        if(i < 6){                                //Read address bits
          if(input(PIN_B0) == 1)
            bit_clear(address, (5 - i));          //Clear bit (5-i)
          else
            bit_set(address, (5 - i));           //Set bit (5-i)
          }
        else {                                 //Read command bits
          if(input(PIN_B0) == 1)
            bit_clear(command, (11 - i));        //Clear bit (11-i)
          else
            bit_set(command, (11 - i));          //Set bit (11-i)
          }
        }
      delay_us(1778);
      }
                     // Display results
    address &= 0x1F;
    command &= 0x3F;
    lcd_putc('\f');
    lcd_gotoxy(2, 1);
    lcd_putc("Toggle bit:");
    lcd_gotoxy(1, 2);
    lcd_putc("Adds:");
    lcd_gotoxy(9, 2);
    lcd_putc("Cmd:");
    lcd_gotoxy(14, 1);
    printf(lcd_putc,"%1u",toggle);
    lcd_gotoxy(6, 2);
    printf(lcd_putc,"%2u",address);
    lcd_gotoxy(14, 2);
    printf(lcd_putc,"%2u",command);
    }
}
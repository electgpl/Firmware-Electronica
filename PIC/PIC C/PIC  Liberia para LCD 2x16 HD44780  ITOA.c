#include <16F676.h>
#device adc=8                   // Configura ADC a 8bit
#FUSES NOWDT                    // No Watch Dog Timer
#FUSES INTRC_IO                 // Internal RC Osc, no CLKOUT
#FUSES NOPROTECT                // Code not protected from reading
#FUSES NOBROWNOUT               // No brownout reset
#FUSES NOMCLR                   // Master Clear pin used for I/O
#FUSES NOCPD                    // No EE protection
#FUSES NOPUT                    // No Power Up Timer
#use delay(int=4000000)         // Oscilador Interno
#define LCD_RS  PIN_C0          // Register select
#define LCD_EN  PIN_C1          // Enable
#define LCD_D4  PIN_C2          // Data bits
#define LCD_D5  PIN_C3          // Data bits
#define LCD_D6  PIN_C4          // Data bits
#define LCD_D7  PIN_C5          // Data bits
#define LCD_STROBE (output_high(LCD_EN),output_low(LCD_EN))
// Este código se conectará a un controlador LCD estándar
// Como el Hitachi HD44780. Utiliza la pantalla LCD en modo de 4 bits, con
// El hardware conectado de la siguiente manera (el estándar 14 pin
// Se utiliza el conector LCD):
// Los bits PORTB 0-3 están conectados a los bits de datos LCD 4-7 (nibble alto)
// El bit 2 de PORTA está conectado a la entrada LCD RS (selección de registro)
// El bit PORTA 3 está conectado al bit LCD EN (habilitacion)
// Para utilizar estas rutinas, configure la E / S de puerto (TRISA, TRISB) y luego
// llamar a lcd_init (), luego otras rutinas según sea necesario.
// escribe un byte en la pantalla LCD en modo de 4 bits
void lcd_write(unsigned char c){
   if(c & 0x80) 
      output_high(LCD_D7); 
   else 
      output_low(LCD_D7);
   if(c & 0x40) 
      output_high(LCD_D6); 
   else 
      output_low(LCD_D6);
   if(c & 0x20) 
      output_high(LCD_D5); 
   else 
      output_low(LCD_D5);
   if(c & 0x10) 
      output_high(LCD_D4); 
   else 
      output_low(LCD_D4);
   LCD_STROBE;
   if(c & 0x08) 
      output_high(LCD_D7); 
   else 
      output_low(LCD_D7);
   if(c & 0x04) 
      output_high(LCD_D6); 
   else 
      output_low(LCD_D6);
   if(c & 0x02) 
      output_high(LCD_D5); 
   else 
      output_low(LCD_D5);
   if(c & 0x01) 
      output_high(LCD_D4); 
   else 
      output_low(LCD_D4);
   LCD_STROBE;    
   delay_us(40);
}
// Limpie y encienda el LCD
void lcd_clear(void){
   output_low(LCD_RS);
   lcd_write(0x01);
   delay_ms(2);
}
// Escribe un carácter en la pantalla LCD
void lcd_putch(unsigned char c){
   output_high(LCD_RS);                // Escribir caracteres
   lcd_write(c);
}
// Escribir una cadena de caracteres en la pantalla LCD
void lcd_puts(char *s){
   output_high(LCD_RS);                // Escribir caracteres
   while(*s) 
      lcd_write(*s++);
}
// Ir a la posición especificada
void lcd_goto(unsigned char pos,unsigned char line){
   output_low(LCD_RS);
   if(line==0)
      lcd_write(0x80 + pos);
   else
      lcd_write(0x80 + pos+ 0x40);
}
// Inicializar la pantalla LCD en modo de 4 bits
void lcd_init(void){
   output_low(LCD_RS);                 // Escribir bytes de control
   delay_ms(15);                       // Retraso de encendido
   output_high(LCD_D4);                // Inicializacion segun datasheet    
   output_high(LCD_D5);                // Inicializacion segun datasheet  
   LCD_STROBE;                         // Habilitacion / Deshabilitacion del LCD a demanda
   delay_ms(5);                        // Delay de configuracion de datasheet
   LCD_STROBE;                         // Habilitacion / Deshabilitacion del LCD a demanda   
   delay_us(100);                      // Delay de configuracion de datasheet
   LCD_STROBE;                         // Habilitacion / Deshabilitacion del LCD a demanda  
   delay_ms(5);                        // Delay de configuracion de datasheet
   output_low(LCD_D4);                 // Establecer el modo de 4 bits
   LCD_STROBE;                         // Habilitacion / Deshabilitacion del LCD a demanda 
   delay_us(40);                       // Delay de configuracion de datasheet
   lcd_write(0x28);                    // Modo de 4 bits, 1/16 de servicio, fuente 5x8, 2 líneas
   lcd_write(0x0C);                    // Enciende Display
   lcd_write(0x06);                    // Cursor de avance del modo de entrada
   lcd_write(0x01);                    // Borrar la visualización y restablecer el cursor
}
char *itoa(long int num, char *s){     // Funcion ITOA (Entero to ASCII)
     unsigned long int temp=1;         // Declaracion de valor temporal
     unsigned int8 i, cnt=0;           // Declaracion de indices y contadores
     char c;                           // Declaracion de variable de caracter de salida
     while(temp>0){                    // Rutina de Conversion (Queda invertida)
        temp=(num/10);                 // Conversion de caracter a caracter
        s[cnt]=(num%10)+'0';           // utilizando divisiones y resto
        if(s[cnt]>0x39)                // sumando el offset de la tabla ASCII
           s[cnt]+=0x7;
        cnt++;
        num=temp;
     }
     for(i=0;i<(int8)(cnt/2);i++){     // Rutina para invertir el numero convertido
        c=s[i];                        // Intercambio de variables
        s[i]=s[cnt-i-1];
        s[cnt-i-1]=c;
     }
     s[cnt]='\0';                      // Caracter nulo, fin de la conversion ITOA
     return s;                         // Retorno del valor ASCII
}
void main(){
   setup_adc_ports(sAN0|VSS_VDD);      // Configuracion ADC canal AN0. Vref = VDD
   setup_adc(ADC_CLOCK_DIV_2);         // Velocidad de conversion
   lcd_init();  
   lcd_clear(); 
   char string[4];
   while(true){
      lcd_goto(1,0);  
      lcd_putch("TEST LIB. LCD");  
      lcd_goto(2,1);
      lcd_putch("VAL: "); 
      set_adc_channel(0);
      delay_us(10);
      itoa(read_adc(),string);
      lcd_puts(string); 
      delay_ms(50);
   }
}
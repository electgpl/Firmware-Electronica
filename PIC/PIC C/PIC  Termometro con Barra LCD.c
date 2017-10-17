#include<16f883.h>									//Microcontrolador
#device adc=10										//Resolución del ADC a 10bit
#use delay(int=4000000)								//Oscilador interno a 4MHz
#include<LCD.C> 									//Liberia del LCD 2x16
double map(float valor, float entradaMin, 			//Declaración de función MAP
           float entradaMax, float salidaMin, 		//Variables de ingreso
           float salidaMax){						//Variables de ingreso
   return ((((valor-entradaMin)*					//Ecuación de adaptación
             (salidaMax-salidaMin))/				//Ecuación de adaptación
             (entradaMax-entradaMin))+salidaMin);	//Ecuación de adaptación
}  
void main(){										//Función principal
   setup_adc_ports(sAN0|VSS_VDD);					//Configuración del canal analógico AN0
   setup_adc(ADC_CLOCK_DIV_2);						//Configuración de la velocidad del ADC
   lcd_init();										//Limpiar el LCD
   int16 salida=0, leeAdc=0, salida_A=0, i=0;		//Definición de variables entero de 16bit
   float temp=0;									//Definición de variables punto flotante
   while(true){										//Lazo de iteración infinito
      set_adc_channel(0);							//Activa canal 0 del ADC
      delay_us(10);									//Espera 10us a que termine la conversión
      leeAdc = read_adc();							//Mueve el valor del ADC a leeADC
      temp = (leeADC*5.0/1023.0)*10;				//Convierte ADC a Grados Celsius
      salida = map(temp, 0, 40, 0, 16);				//Aplica función MAP para redimencionar la Barra
      delay_ms(1000);								//Delay de 1s para el refresco de datos
      lcd_gotoxy(12,1);								//Posiciona el cursor en carácter 12 renglón 1
      lcd_putc(0xDF);								//Carácter 11011111 correspondiente a "°"
      lcd_gotoxy(13,1);								//Posiciona el cursor en carácter 13 renglón 1
      lcd_putc(0x43);								//Carácter 01000011 correspondiente a "C"
      lcd_gotoxy(1,1);								//Posiciona el cursor en carácter 1 renglón 1
      printf(lcd_putc,"Temp: %2.1f",temp);			//Muestra el valor XX.X de temperatura
      for(i=1;i<salida+1;i++){						//Lazo for para posicionar la barra en la posición inicial
         lcd_gotoxy(i,2);							//Se incrementa la posición dinámica en el segundo renglón 
         lcd_putc(0xFF);							//Se completa con carácter especial "█"
      }
      if(salida_A<salida){							//Si se incrementa la barra
         lcd_gotoxy(salida,2);						//Se incrementa la posición del segundo renglón
         lcd_putc(0xFF);							//Se completa con carácter especial "█"
      }
      if(salida_A>salida){							//Si se decrementa la barra
         lcd_gotoxy(salida+1,2);					//Se decrementa la posición del segundo renglón
         lcd_putc(0x20);							//Se completa con espacios vacíos
      }
      salida_A = salida;							//Intercambio de variables
   }
}
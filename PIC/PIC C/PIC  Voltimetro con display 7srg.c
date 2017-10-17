#include <16F883.h>                         //Microcontrolador a Elegir
#device adc=10                              //Resolución del ADC a 10Bit
#use delay(clock=4000000)                   //Oscilador Externo a 4MHz
//Vector que aloja los 10 valores numéricos para el display de 7 segmentos
int8 num[10]={0b00111111,0b00000110,0b01011011,0b01001111,0b01100110,
              0b01101101,0b01111101,0b00000111,0b01111111,0b01101111};
#int_timer0                                 //Función del Timer0
//Función que se ejecutara en paralelo al programa principal sobre el Timer0
void timer0_mux(int8 dig4, int8 dig3, int8 dig2, int8 dig1){
   output_c(0b00000001);                    //Selección del display 1
   output_b(num[dig1]);                     //Valor tratado del dígito 1 en la salida B
   output_high(PIN_B7);                     //Prendemos el punto decimal
   delay_ms(2);                             //Delay de multiplexado de 2ms
   output_c(0b00000010);                    //Selección del display 2
   output_b(num[dig2]);                     //Valor tratado del dígito 1 en la salida B
   output_low(PIN_B7);                      //Apagamos el punto decimal
   delay_ms(2);                             //Delay de multiplexado de 2ms
   output_c(0b00000100);                    //Selección del display 3
   output_b(num[dig3]);                     //Valor tratado del dígito 1 en la salida B
   output_low(PIN_B7);                      //Apagamos el punto decimal
   delay_ms(2);                             //Delay de multiplexado de 2ms
   output_c(0b00001000);                    //Selección del display 4
   output_b(num[dig4]);                     //Valor tratado del dígito 1 en la salida B
   output_low(PIN_B7);                      //Apagamos el punto decimal
   delay_ms(2);                             //Delay de multiplexado de 2ms
   set_timer0(0);                           //Seteo del Timer0
}
void convert(int16 valor){                  //Función para convertir entero en dígitos
int uno, diez, cien, mil;                   //Declaración de variables de los dígitos
   mil=valor/1000;                          //Operación para adquirir miles
   cien=(valor/100)%10;                     //Operación para adquirir cientos
   diez=(valor/10)%10;                      //Operación para adquirir decenas
   uno=valor%10;                            //Operación para adquirir unidades
   timer0_mux(uno,diez,cien,mil);           //Llamado a la función de multiplexado
}
void main(){                                //Función principal del programa
   setup_adc_ports(sAN0|VSS_VDD);           //Configuración del canal del ADC
   setup_adc(ADC_CLOCK_DIV_2);              //Configuración de velocidad del ADC
   setup_timer_0(RTCC_INTERNAL|RTCC_DIV_8); //Configuración del divisor del Timer0
   set_timer0(0);                           //Seteo del Timer0
   enable_interrupts(INT_TIMER0);           //Habilitación de interrupción Timer0
   enable_interrupts(GLOBAL);               //Habilitación de interrupciones
   float tension, tensionAnt=0;             //Definición de variables flotantes
   int16 i;                                 //Definición de entero de 16 bit
   while(true){                             //Lazo repetitivo principal
      set_adc_channel(0);                   //Selección del canal 0 del ADC
      delay_ms(5);                          //Delay para la conversión de 5ms
      tension=read_adc()*5.0/1023.0;        //Lee el ADC y convierte en valor de tensión
      for(i=0;i<500;i++){                   //Lazo de 500 iteraciones para sacar promedio
         tensionAnt=tension+tensionAnt;     //Suma del acumulado para sacar promedio
      }        
      convert((tensionAnt/500)*1000);       //Llama a la función que convierte a dígitos
      tensionAnt=0;                         //Inicializa variable de promedio
   }
}
#include<reg51.h>           // Biblioteca de registros 8051
//----------------------------------------------------------------
sbit SALIDA=P2^0;           // Salida en Puerto 2 pin 0
int i=0;                    // Declaración de variable para delay
//----------------------------------------------------------------
// Calculo del tiempo de demora, 65535-15535=50000
// Valor a cargar en el timer 15535 = 3CAF
// Usar cristal de 12MHz para el delay de 50ms
//----------------------------------------------------------------
void delay_50ms(){          // Función de Delay de 50ms con Timer
   TMOD=0x01;               // Timer0 en modo 1
   TH0=0x3C;                // Carga parte alta del timer 3C
   TL0=0xAF;                // Carga parte baja del timer AF
   TR0=1;                   // Timer0 Start
   while(TF0==0);           // Espera Overflow del timer
   TR0=0;                   // Timer0 Stop
   TF0=0;                   // Timer0 Clear
}
//----------------------------------------------------------------
void main(){                // Función Principal
   SALIDA=0;                // Inicializa salida a LOW
   while(1){                // Loop Infinito
   for(i=0;i<10;i++){       // 10 iteraciones para lograr 500ms
      delay_50ms();         // Delay de 50ms
   }
      SALIDA^=1;            // toggle de Salida
   }
}

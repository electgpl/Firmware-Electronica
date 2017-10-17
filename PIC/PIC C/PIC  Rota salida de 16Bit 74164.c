#include <16F628A.h>
#FUSES NOWDT, INTRC, NOMCLR
#use delay(int=4000000)
#define DATO PIN_A1					//Pin de Datos para el 74164
#define CLK  PIN_A0					//Pin de Reloj para el 74164
int8 valor, i;						//Definición de variables
void main(){						//Función Principal
    while(true){                    //Ciclo principal
        valor=1;                    //Inicialicamos valor con 1
        for(i=0;i<16;i++){          //Recorremos las 16 posiciones de la variable i
            output_low(CLK);        //Clock a nivel bajo  
            output_bit(DATO,valor); //Salida de dato para 74164, se asigna el valor
            delay_ms(300);          //Demora de carga de datos
            output_high(CLK);       //Clock a nivel alto
            shift_left(&valor,1,0); //Salta a la izquierda la variable valor, en una posición
        }
    }
}
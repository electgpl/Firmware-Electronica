#include <16F628A.h>
#FUSES NOWDT, HS, NOPUT, MCLR
#use delay(clock=20000000)
int8 x, y;
void main(){
	while(true){					//Ciclo principal
		x=0x01;						//Inicialicamos con 00000001 la variable X
		for(y=0;y<=7;++y){			//Recorremos las 8 posiciones de la variable Y
			output_b(x);			//Asignamos el valor de X al puerto B
			delay_ms(150);			//Espera 150ms
			shift_left(&x,1,0);		//Salta a la izquierda el valor de X, en una posición
		}							//Se recorre de una en una posicion el puerto B hacia la izquierda
        x=0x80;						//Inicialicamos con 10000000 la variable X
		for(y=0;y<=7;++y){			//Recorremos las 8 posiciones de la variable Y
			output_b(x);			//Asignamos el valor de X al puerto B
			delay_ms(150);			//Espera 150ms
			shift_right(&x,1,0);	//Salta a la derecha el valor de X, en una posición  
		}							//Se recorre de una en una posicion el puerto B hacia la derecha
	}
}
//Función para enviar SMS con SIM900 en AT Commands 9600bps
void enviaSMS(){
   printf("AT+CMGF=1\r"); 			//Modo envió de SMS
   delay_ms(100);					//Delay de 100ms
   printf("AT+CMGS=");				//Comando para poner el numero de teléfono
   printf('"');						//El numero va entre comillas "
   printf("+5411********"); 		//Número de teléfono
   printf('"');						//El numero va entre comillas "
   printf("\r");					//Retorno de carro
   printf("Mensaje de ejemplo");	//Mensaje a enviar (aquí va la variable)
   printf(0x1A);					//Comando 0x1A para enviar
   delay_ms(1000);					//Delay de 1s
}
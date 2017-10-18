#include<reg51.h>
void UART_Init(int baudrate){ 
   SCON=0x50;  									      //Asynchronous mode, 8-bit data and 1-stop bit
   TMOD=0x20;  									      //Timer1 in Mode2.
   TH1=256-(11059200UL)/(long)(32*12*baudrate); //carga el timer para generar el baudrate
   TR1=1;      									      //enciende el timer
}
void UART_TxChar(char ch){
   SBUF=ch;      								         //carga el dato a transmitir
   while(TI==0);   								      //espera a que este disponible la transmision
   TI=0;         								         //borra el flag de transmision
}
char UART_RxChar(void){
   while(RI==0);     							      //espera a que el dato sea recibido
   RI=0;             							      //borra la interrupcion del flag de recpcion
   return(SBUF);     							      //retorna el dato recibido
}
int main(){
   char i,a[]={"Welcome to 8051 Serial Comm, Type the char to be echoed: "};
   char ch;
   UART_Init(9600);       						      //inicializa la uart en 9600bps
   for(i=0;a[i]!=0;i++){
      UART_TxChar(a[i]); 						      //transmite el dato
   }
   while(1){
      ch=UART_RxChar(); 						      //recibe el dato
      UART_TxChar(ch);    						      //retransmite el dato a modo eco
   }
}

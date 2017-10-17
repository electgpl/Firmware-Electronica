#include<reg51.h>
void UART_Init(int baudrate){ 
   SCON=0x50;  									// Asynchronous mode, 8-bit data and 1-stop bit
   TMOD=0x20;  									//Timer1 in Mode2.
   TH1=256-(11059200UL)/(long)(32*12*baudrate); // Load timer value for baudrate generation
   TR1=1;      									//Turn ON the timer for Baud rate generation
}
void UART_TxChar(char ch){
   SBUF=ch;      								// Load the data to be transmitted
   while(TI==0);   								// Wait till the data is trasmitted
   TI=0;         								//Clear the Tx flag for next cycle.
}
char UART_RxChar(void){
   while(RI==0);     							// Wait till the data is received
   RI=0;             							// Clear Receive Interrupt Flag for next cycle
   return(SBUF);     							// return the received char
}
int main(){
   char i,a[]={"Welcome to 8051 Serial Comm, Type the char to be echoed: "};
   char ch;
   UART_Init(9600);       						//Initialize the UART module with 9600 baud rate
   for(i=0;a[i]!=0;i++){
      UART_TxChar(a[i]); 						// Transmit predefined string
   }
   while(1){
      ch=UART_RxChar(); 						// Receive a char from serial port
      UART_TxChar(ch);    						// Transmit the received char
   }
}
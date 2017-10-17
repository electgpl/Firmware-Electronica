#include<reg51.h>
#include<stdio.h>
#include<stdlib.h>
// function prototypes
void delay(unsigned int ms);
void putc1( chr);
void windspeed_read( void);
void initialize(void);
void record_wind(unsigned char counter);
void puts1(char* p);
sbit P3_2 = P3^2;
sbit P3_3 = P3^3;
// global variables declared
unsigned char c = 0;
char *ptr;
int i;
char buf[16];
char *p;
// delay function to create a delay of 1 sec
void delay(const unsigned int ms){
   unsigned int x;
   unsigned int y;
   for(x=0;x<ms;x++){
      for(y=0;y<=113;y++);
   }
}
// to print the character in UART and serial window
void putc1(c){
   SBUF=c;
   while(TI==0);            //Wait until the character is completely sent 
   TI=0;                    //Reset the flag 
}
// main function
void main(){
   IE=0x81;
   EX0=1;
   EA=1; 
   while(1){
   }
}
void initialize(){
   SCON=0x50;               //SCON: mode 1, 8-bit UART, enable receive 
   TMOD|=0x20;              //TMOD: timer 1, mode 2, 8-bit
   TH1=0xFD;                //TH1:  for 9600 baud 
   TR1=1;                   //TR1:  timer 1 run
}
void windspeed_read(void) interrupt 0 {
   c++;
   sprintf(buf,"%d",c);
   p=buf;
   while(*p){
      P2=*p;
      putc1(*p++);
   }
}
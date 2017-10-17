//*******************************************************
// Sample echo program for UART0.
//*******************************************************
#include C8051F120.h
typedef unsigned char uchar;
//*******************************************************
// Globals
//*******************************************************
uchar character;
bit charready, stopped;
sbit LED = P1^6;
int count = 0;
//*******************************************************
// config
// Configure the UART and start the interrupt timer.
//*******************************************************
void config(){
	int n;
	SFRPAGE = CONFIG_PAGE;
	WDTCN = 0x07; 					// watchdog timer control
	WDTCN = 0xDE; 					// disable wdt
	WDTCN = 0xAD;
	XBR0 = 0x04; 					// UART0 connects
	XBR2 = 0x40; 					// enable crossbar
	P0MDOUT = 0x01; 				// P0.0 is push-pull TX output
	P1MDIN = 0xFF; 					// input config for P1
									// P0.0 = UART TX0 (push-pull output/input)
									// P0.1 = UART RX0 (open-drain output/input)
	P1MDOUT |= 0x40; 				// enable onboard LED
	OSCXCN = 0x77; 					// external oscillator
	for(n=0;n<3000;n++); 			// startup interval
	while((OSCXCN&0x80)==0); 		// wait for oscillator
	SCON0 = 0x50; 					// serial port control
	SCON0 &= 0xFC; 					// clear pending interrupt flags
	CKCON = 0x08; 					// clock control
	SFRPAGE = TIMER01_PAGE;
	TH1 = 0xDC; 					// Timer1 UART clock
	TL1 = 0x00;
	TMOD = 0x20; 					// Timer mode register
	TCON = 0x40; 					// Timer control register
	SFRPAGE = LEGACY_PAGE;
	IE = 0x90; 						// Interrupt enable
}
//*******************************************************
// initializeUART
// Initialize the UART and set flags.
//*******************************************************
void initializeUART(){
	charready=0;
	stopped=1;
	config();
	LED = 0;
}
//*******************************************************
// UART0int
// UART0 interrupt routine.
//*******************************************************
void UART0int() interrupt 4 using 1 {
	LED=1;
	if(RI0==1){
		character=SBUF0;
		charready=1;
		if(stopped){
			TI0=1;
			stopped=0;
		}
		RI0=0;
	}
	if(TI0==1){
		if(charready){
			SBUF0=character;
			charready=0;
		}else{
			stopped=1;
		}
		TI0=0;
	}
}
//*******************************************************
// main
//*******************************************************
void main(){
	initializeUART();
	for (; / {
	// count++;
	// if (!count)
	// LED = ~LED;
	}
}
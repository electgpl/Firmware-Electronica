//-----------------------------------------------------------------------------
// F85x_Blinky.c
//-----------------------------------------------------------------------------
// Copyright (C) 2013 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// This program flashes the red LED on the C8051F850 UDP MCU board about
// five times a second using the interrupt handler for Timer2.
// How To Test:
//
// 1) Ensure shorting blocks are place on the following:
//    - J27: DS5 P1.0 - P1.0
//    - JP2
// 2) Place the SW1 switch in the +3.3V_VREG position and power the board
//    using the J6 power connector.
// 3) Compile and download code to the 'F85x UDP MCU board.
// 4) Run the code. If the P1.0 (DS5) LED flashes, the program is working.
//
// Target:         C8051F850
// Tool chain:     Generic
// Command Line:   None
//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <compiler_defs.h>             // compiler declarations
#include <C8051F850_defs.h>            // SFR declarations
//-----------------------------------------------------------------------------
// Global CONSTANTS
//-----------------------------------------------------------------------------
#define SYSCLK       24500000/8        // SYSCLK frequency in Hz
SBIT(LED, SFR_P1, 0);                  // DS5 P1.0 LED
//-----------------------------------------------------------------------------
// Function PROTOTYPES
//-----------------------------------------------------------------------------
void Port_Init (void);
void Timer2_Init (int counts);
//-----------------------------------------------------------------------------
// MAIN Routine
//-----------------------------------------------------------------------------
void main (void){
   // Disable the watchdog timer
   WDTCN = 0xDE;
   WDTCN = 0xAD;
   Port_Init ();
   Timer2_Init (SYSCLK / 12 / 10);  // Init Timer2 to generate interrupts
                                    // at a 10 Hz rate.
   EA = 1;                          // Enable global interrupts
   while (1) {}                     // Spin forever
}
//-----------------------------------------------------------------------------
// Port_Init
//-----------------------------------------------------------------------------
//
// Configure the Crossbar and GPIO ports.
//
// P1.0 - LED (push-pull output)
//
void Port_Init (void){
   P1MDOUT |= 0x01;                    // Enable LED as a push-pull output
   XBR2 = 0x40;                        // Enable crossbar and weak pull-ups
}
//-----------------------------------------------------------------------------
// Timer2_Init
//-----------------------------------------------------------------------------
//
// Configure Timer2 to 16-bit auto-reload and generate an interrupt at
// interval specified by <counts> using SYSCLK/48 as its time base.
//
void Timer2_Init (int counts){
   TMR2CN = 0x00;                      // Stop Timer2; Clear TF2;
                                       // use SYSCLK/12 as timebase
   CKCON &= ~0x60;                     // Timer2 clocked based on T2XCLK;
   TMR2RL = -counts;                   // Init reload values
   TMR2 = 0xffff;                      // Set to reload immediately
   ET2 = 1;                            // Enable Timer2 interrupts
   TR2 = 1;                            // Start Timer2
}
//-----------------------------------------------------------------------------
// Interrupt Service Routines
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Timer2_ISR
//-----------------------------------------------------------------------------
// This routine changes the state of the LED whenever Timer2 overflows.
//
INTERRUPT(Timer2_ISR, INTERRUPT_TIMER2){
   TF2H = 0;                              // Clear Timer2 interrupt flag
   LED = !LED;                            // Change state of LED
}
//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------
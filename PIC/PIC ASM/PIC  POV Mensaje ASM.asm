	LIST 	p=16f648a		; tell assembler what chip we are using
	include "P16f648a.inc"		; include the defaults for the chip
	__config 0x3f18 		; sets the configuration settings (oscillator type etc.)

PC 	equ 	0x02
;
	cblock 	0x20 			; start of general purpose registers
	counta 				; used in the delay routine
	countb 				; used in the delay routine
	low_address 			; stores the eproms low address (first 8bits)
	high_address 			; stores the eproms high address (last 8bits)
	pixel_counter 			; stores the total number of horizontal pixels
	frame_counter 			; stores the total number of times to repeat each frame
	total_frames 			; stores the total number of frames in our animation
	endc
;
	org 	0x0000 			; org sets the origin, 0x0000 for the 16F628,
;
	movlw 	0x07
	movwf 	CMCON 			; turn comparators off
;
	bsf 	STATUS,RP0 		; select bank 1
	movlw 	b'00000000' 		; set PORTB all outputs
	movwf 	TRISB 			;
	movlw 	b'00100000' 		; set PORTA all outputs except for bit 5
	movwf 	TRISA 			;
	bcf 	STATUS,RP0 		; select bank 0
;
setup	movlw 	d'178'			; defines how many frames we have -
	movwf 	total_frames 		; in our animation loop
	movlw 	b'10011000' 		; turns led outputs OFF, disables
	movwf 	PORTA 			; led latches and opens the address latches
	clrf 	PORTB 			; sets the sixteen bit address to 00 hex
	bcf 	PORTA,3 		; disables low address latch
	bcf 	PORTA,4 		; disables high address latch
	clrf 	low_address 		; clear the low_address variable
	clrf 	high_address 		; clear the high_address variable
frame_again
	movlw 	d'02' 			; sets it up to repeat each frame however many times
	movwf 	frame_counter 		; you want (the higher the number, the slower the animation.)
;
draw_again
	movlw 	d'85' 			; sets it up for 85 pixels horizontaly - this number is perfect
	movwf 	pixel_counter 		; because 85x3 = 255 which is our lower address bus limit!
	clrf 	low_address 		; reset the low_address variable back to 00 hex
	clrf 	PORTB 			; clears PORTB and then
	bsf 	PORTA,3 		; resets the low address back to 00 hex
	bcf 	PORTA,3 		; I had colour sync problems without this...
;
; upto this point the leds are all OFF, the eeprom has 00000000 00000000 on its address bus
; and all latches are now in their disabled or closed state.
; This next section is the main DRAWING part of the program, it basically grabs
; all the colour data - red, green then blue and then outputs the data to the leds
; it repeats this process untill all 85 columns are drawn, then it will repeat that
; same frame for however many times we stated in the above code. and finally it will
; move onto each successive frame in our animation sequence untill we reach the end.
; then it will reset itself and start all over again!
;
begin	bsf 	PORTA,0 		; outputs 8bits to the red latch and holds it
	bcf 	PORTA,0	 		; closes the red latch
	call 	inc_address 		; increment to the next eprom address
	bsf 	PORTA,1 		; outputs 8bits to the green latch and holds it
	bcf 	PORTA,1 		; closes the green latch
	call 	inc_address 		; increment to the next eprom address
	bsf 	PORTA,2 		; outputs 8bits to the blue latch and holds it
	bcf 	PORTA,2 		; closes the blue latch
	call 	inc_address 		; increment to the next eprom address
	bcf 	PORTA,7 		; enables all led latch outputs which turns on the leds
	call 	delay 			; hold the leds on for just a while...
	bsf 	PORTA,7 		; disables all led latch outputs which turns off the leds
	decfsz 	pixel_counter,1 	; decrements the pixel counter from 85 until 0
	goto 	begin 			; if its not yet zero, keep drawing until 0
wait	btfss 	PORTA,5 		; Check to see if we are at the sensor check point - 
	goto 	wait 			; If not, keep checking until we are there BEFORE we start drawing the next frame.
	decfsz 	frame_counter,1 	; now decrement the frame counter
	goto 	draw_again 		; if the frame_counter is not zero go back and draw it again
	call 	inc_frame 		; call the routine so we can move to the next frame of animation
	decfsz 	total_frames,1 		; decrement the total frames counter by one, reset if it is 0
	goto 	frame_again 		; displays this next frame for however many times we specified.
	goto 	setup 			; start again!
;
; This next routine increments the low 8-bits of the address bus by one.
;
inc_address
	incf 	low_address,f 		; increment the low_address variable by one
	movf 	low_address,w 		; copy it to w register
	movwf 	PORTB 			; now output it to PORTB
	bsf 	PORTA,3 		; latch it to the low address of the eeprom
	bcf 	PORTA,3 		; now close the latch
	return 				; back to the main program
;
; This next bit just increments to the next frame in the animation
;
inc_frame
	incf 	high_address,f 		; increment the high_address variable by one
	movf 	high_address,w 		; copy it to the w register
	movwf 	PORTB 			; now output it to PORTB
	bsf 	PORTA,4 		; latch it to the high address of the eprom
	bcf 	PORTA,4 		; now close the latch
	return 				; back to the main program
;
; Heres a nice and simple delay routine
;
delay	movlw 	d'55'
	movwf 	counta
again_a	decfsz 	counta,1
	goto 	again_a
	return
;
	end
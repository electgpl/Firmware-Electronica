	list 	p=16f877A
	include	<pic16f877a.inc> 
__CONFIG _CP_OFF & _WDT_OFF & _BODEN_OFF & _PWRTE_OFF & _LVP_OFF & 
_PWRTE_ON & _XT_OSC
;
	org 	0x000 
;
	bcf 	STATUS,RP0 
	bcf 	STATUS,RP1 
	movlw 	b'01000001' 
	movwf 	ADCON0 
	bsf 	STATUS,RP0 
	bcf 	STATUS,RP1 
	clrf 	TRISA 
	clrf 	TRISB 
	clrf 	TRISC 
	clrf 	TRISD 
	clrf 	TRISE 
	movlw 	b'00000111' 
	movwf 	OPTION_REG 
	movlw 	b'00001110' 
	movwf 	ADCON1 
	bsf 	TRISA,0 
	bcf 	STATUS,RP0 
	bcf 	STATUS,RP1 
	clrf 	PORTC 
;
bucle 	btfss 	INTCON,T0IF 
	goto 	bucle 
	bcf 	INTCON,T0IF 
	bsf 	ADCON0,GO 
;
espera 	btfsc 	ADCON0,GO 
	goto 	espera 
	movf 	ADRESH,W 
	movwf 	PORTC 
	goto 	bucle 
; 
	end
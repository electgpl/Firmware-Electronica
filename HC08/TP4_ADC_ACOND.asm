;*************************************************************************
;-------------------------------------------------------------------------
$Include 'jl3regs.inc'			;Libreria para el MC68HC908JL3
;-------------------------------------------------------------------------
;- DEFINICION DE CONSTANTES Y VARIABLES                                  -
;-------------------------------------------------------------------------
RamSt	equ	$0080		;Direccion de origen de memoria RAM
RomSt	equ	$EC00		;Direccion de origen de memoria ROM
ResetV	equ	$FFFE		;Vector de Reset
;						
	org	RamSt		;Se apunta al origen de memoria RAM
;
varUni	rmb	1		;Variable para separacion de Unidades
varDec	rmb	1		;Variable para separacion de Decenas
varCen	rmb	1		;Variable para separacion de Centenas
;
	org	RomSt		;Se apunta al origen de memoria ROM
;-------------------------------------------------------------------------
;- CONFIGURACION                                                         -
;-------------------------------------------------------------------------
MAIN	mov	#$11,CONFIG1	;Deshabilita el COP y LVI
	mov	#$FF,DDRA	;Configura puerto A como salidas
	clr	PORTA		;Inicializa el puerto A
	mov	#$FF,DDRB	;Configura puerto B como salidas
	clr	PORTB		;Inicializa el puerto B
	mov	#$FF,DDRD	;Configura puerto D como salidas
	clr	PORTD		;Inicializa el puerto D				
;-------------------------------------------------------------------------
;- INICIO                                                                -
;-------------------------------------------------------------------------
INICIO 	clra
	clrx
	clrh
	lda	#$3C		;Variable de prueba del ADC
	ldx	#$05		;Valor 5
	div			;A/X=A
	ldx	#$02		;Valor 2
	mul			;A*X=A
	jsr	DECTBCD		;Rutina de conversion de Decimal a BCD	
	mov	varUni,PORTA
	mov	varDec,PORTB
	mov	varCen,PORTD
	jmp	INICIO		;Loop del bloque principal							  														 																  																	  											  
;-------------------------------------------------------------------------
;- CONVERSION DE DECIMAL A BCD SEPARADO POR DIGITOS                      -
;-------------------------------------------------------------------------
DECTBCD	ldhx	#$0A		;Carga X con 10
	div			;Divide A/X=A, Resto en H
	sthx	varUni		;Mueve H a varUni
	clrh			;Borra H
	ldhx	#$0A		;Carga X con 10
	div			;Divide A/X=A
	sthx	varDec		;Mueve H a varDec
	sta	varCen		;Mueve A a varCen
	rts			;Retorno de subrutina			    													  		   		
;-------------------------------------------------------------------------
;- FINAL                                                                 -
;-------------------------------------------------------------------------
	org 	ResetV		;Se apunta al Vector de Reset
	dw	MAIN		;Salta a MAIN
	end			;Fin del programa

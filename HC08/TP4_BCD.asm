;******************************************************************
$Include 'jl3regs.inc'		;Libreria para el MC68HC908JL3
;
RamSt	equ	$0080		;Direccion de origen de memoria RAM
RomSt	equ	$EC00		;Direccion de origen de memoria ROM
ResetV	equ	$FFFE		;Vector de Reset
;						
	org	RamSt		;Se apunta al origen de memoria RAM
;
varCen	rmb	1		;Variable 1Byte para Centenas
tempCen	rmb	1		;Variable 1Byte Temporal para Centenas
varDec 	rmb	1		;Variable 1Byte para Decenas
tempDec	rmb	1		;Variable 1Byte Temporal para Decenas
varUni 	rmb	1		;Variable 1Byte para Unidades
valConv	rmb	1		;Variable 1Byte Decimal a convertir
;
	org	RomSt		;Se apunta al origen de memoria ROM
;
MAIN	mov	#$11,CONFIG1	;Deshabilita el COP y LVI
	mov	#$FF,DDRD	;Configura puerto D como salidas
	clr	PORTD		;Inicializa el puerto D
	mov	#$FF,DDRB	;Configura puerto B como salidas
	clr	PORTB		;Inicializa el puerto B
	mov	#$FF,DDRA	;Configura puerto A como salidas
	clr	PORTA		;Inicializa el puerto A				
;
INICIO	mov	#$00,valConv	;Numero 00 a convertir
	jsr	DecTbcd		;Goto rutina de conversion
	mov	varCen,PORTD
	mov 	varDec,PORTB
	mov 	varUni,PORTA
	bra	INICIO
;
DecTbcd	jsr	INIT		;Inicializa Acumuladores
	lda	valConv		;Carga A con valConv
	ldx	#$64		;Carga X con 100
	div			;Se divide A/X=A
	sta	varCen		;Guarda el valor de Centenas
	jsr	INIT		;Inicializa Acumuladores
	lda	valConv		;Carga A con valConv
	ldx	#$0A		;Carga X con 10
	div			;Se divide A/X=A
	sta	varDec		;Guarda el valor temporal
	lda	varCen	  	;Carga A con varCen
	cmp	#$00 	  	;Hay Centenas?
	beq	SIGUE		;Si no hay, salta a SIGUE
	cmp	#$02		;Es 200?
	beq	RES20		;Si es 200, resta 20
	lda	varDec	  	;Carga A con varCen
	sub	#$0A		;Resta 10 al cociente
	sta	varDec		;Guarda el valor de Decenas	
	bra	SIGUE		;Salta a SIGUE	
RES20	lda	varDec	  	;Carga A con varCen
	sub	#$14		;Resta 20 al cociente
	sta	varDec		;Guarda el valor de Decenas
SIGUE	jsr	INIT		;Inicializa Acumuladores
	lda	varCen		;Carga A con varCen
	ldx	#$64		;Carga X con 100
	mul			;Multiplica A*X=A
	sta	tempCen		;Guarda en Temporal Centenas
	jsr	INIT		;Inicializa Acumuladores
	lda	varDec		;Carga A con varDec
	ldx	#$0A		;Carga X con 10
	mul			;Multiplica A*X=A
	sta	tempDec		;Guarda en Temporal Centenas
	jsr	INIT		;Inicializa Acumuladores	
	lda	valConv		;Carga A con valConv
	sub	tempCen		;Resta A-tempCen
	sub	tempDec		;Resta (A-tempCen)-tempDec
	sta	varUni		;Guarda el valor de Unidades				
	rts			;Retorno de subrutina
;		
INIT	clra			;Inicializa A
	clrx			;Inicializa X
	clrh			;Inicializa H
	rts			;Retorno de subrutina
;
	org 	ResetV		;Se apunta al Vector de Reset
	dw	MAIN		;Salta a MAIN
	end			;Fin del programa

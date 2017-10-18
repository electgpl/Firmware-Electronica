;******************************************************************
$Include 'jl3regs.inc'		;Libreria para el MC68HC908JL3
;
RamSt	equ	$0080		;Direccion de origen de memoria RAM
RomSt	equ	$EC00		;Direccion de origen de memoria ROM
ResetV	equ	$FFFE		;Vector de Reset
;						
	org	RamSt		;Se apunta al origen de memoria RAM
;
Var1	rmb	1		;Variable 1Byte para Loop Interno
Var2	rmb	1		;Variable 1Byte para Loop Externo
;
	org	RomSt		;Se apunta al origen de memoria ROM
;
MAIN	mov	#$11,CONFIG1	;Deshabilita el COP y LVI
	mov	#$FF,DDRD	;Configura puerto D como salidas
	clr	PORTD		;Inicializa el puerto D
	mov	#$20,ADCLK	;Divisor por 1, Reloj interno 0010 0000
	mov	#$20,ADSCR	;Conv.Continua, ADC PTB0 x010 0000
;
INICIO	
LADC	brclr	7,ADSCR,LADC	;Espera a que bit COCO sea 1
	lda	ADR		;Guarda conversion en acumulador
	sta	PORTD		;Envia el valor convertido a PORTD		
	jsr	DELA05		;Llama al delay de 500ms
	bra	INICIO		;Repite el ciclo
;		
DELA05	mov	#$B9,Var2	;[5]Se carga el valor 185 a Var2
LOOPE	mov	#$FF,Var1	;[5]Se carga el valor 255 a Var1
	dec	Var2		;[4]Se decrementa Var2
	beq	FIN05		;[3]Si Var2=0 salta a FIN05
LOOPI	dec	Var1		;[4]Se decrementa Var1
	beq	LOOPE		;[3]Si Var1=0 salta a LOOPE (Externo)
	bra	LOOPI		;[3]Salta a LOOPI (Interno)
FIN05	rts			;[4]Retorno de subrutina
;
	org 	ResetV		;Se apunta al Vector de Reset
	dw	MAIN		;Salta a MAIN
	end			;Fin del programa

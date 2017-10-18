;******************************************************************
;* Delay 50ms ((12+20)*(10*146))+13=49933us=+/-50ms
;*****************************************************************
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
	mov	#$FF,DDRA	;Configura puerto A como salidas
	clr	PORTA		;Inicializa el puerto A
	mov	#$FF,DDRD	;Configura puerto D como salidas
	clr	PORTD		;Inicializa el puerto D
	mov	#$20,ADCLK	;Divisor por 1, Reloj interno 0010 0000
	mov	#$20,ADSCR	;Conv.Continua, ADC PTB0 x010 0000
;
INICIO	jsr	LeeADC		;Lectura del ADC
	cmp	#$CC		;Compara ADC con 204 (80°C)
	bhi	GT80		;Si es mayor, salta a GT80
	cmp	#$99		;Compara ADC con 153 (60°C)
	bhi	GT60		;Si es mayor, salta a GT60
	cmp	#$66		;Compara ADC con 102 (40°C)
	bhi	GT40		;Si es mayor, salta a GT40
	cmp	#$34		;Compara ADC con 52  (20°C)
	bhi	GT20		;Si es mayor, salta a GT20	
	jsr	E0y20		;Salta a rutina Entre  0 y 20
	bra	INICIO		;Repite el ciclo
GT20	jsr	E20y40		;Salta a rutina Entre 20 y 40
	bra	INICIO		;Repite el ciclo
GT40	jsr	E40y60		;Salta a rutina Entre 40 y 60
	bra	INICIO		;Repite el ciclo
GT60	jsr	E60y80		;Salta a rutina Entre 60 y 80
	bra	INICIO		;Repite el ciclo
GT80	jsr	E80y10		;Salta a rutina Entre 80 y 100
	bra	INICIO		;Repite el ciclo
;
LeeADC	
LADC	brclr	7,ADSCR,LADC	;Espera a que bit COCO sea 1
	lda	ADR		;Guarda conversion en acumulador
;	jsr	DELA050		;Llama al delay de 50ms
	rts			;Retorno de subrutina
;
E0y20	mov	#$01,PORTD	;PORTD = 0000 0001						
	rts			;Retorno de subrutina
;
E20y40	mov	#$03,PORTD	;PORTD = 0000 0011							
	rts			;Retorno de subrutina
;
E40y60	mov	#$07,PORTD	;PORTD = 0000 0111							
	rts			;Retorno de subrutina
;
E60y80	mov	#$0F,PORTD	;PORTD = 0000 1111							
	rts			;Retorno de subrutina
;
E80y10 	mov	#$1F,PORTD	;PORTD = 0001 1111							
	rts			;Retorno de subrutina
;		
DELA050	mov	#$14,Var2	;[5]Se carga el valor 20  a Var2
LOOPE	mov	#$92,Var1	;[5]Se carga el valor 146 a Var1
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

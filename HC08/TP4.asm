;*************************************************************************
;-------------------------------------------------------------------------
$Include 'jl3regs.inc'		;Libreria para el MC68HC908JL3
;-------------------------------------------------------------------------
;- DEFINICION DE CONSTANTES Y VARIABLES                                  -
;-------------------------------------------------------------------------
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
valLm35	rmb	1		;Variable 1Byte para conversion de LM35
valADC	rmb	1		;Variable 1Byte para el valor del ADC
Var1	rmb	1		;Variable 1Byte para Loop Interno
Var2	rmb	1		;Variable 1Byte para Loop Externo
;
	org	RomSt		;Se apunta al origen de memoria ROM
;-------------------------------------------------------------------------
;- CONFIGURACION                                                         -
;-------------------------------------------------------------------------
MAIN	mov	#$11,CONFIG1	;Deshabilita el COP y LVI
	mov	#$FF,DDRA	;Configura puerto A como salidas
	clr	PORTA		;Inicializa el puerto A
	mov	#$FF,DDRD	;Configura puerto D como salidas
	clr	PORTD		;Inicializa el puerto D
	mov	#$20,ADCLK	;Divisor por 1, Reloj interno 0010 0000
	mov	#$20,ADSCR	;Conv.Continua, ADC PTB0 x010 0000					
;-------------------------------------------------------------------------
;- PRINCIPAL                                                             -
;-------------------------------------------------------------------------
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
;-------------------------------------------------------------------------
;- LEE EL ADC Y GUARDA EL VALOR EN ACUMULADOR A                          -
;-------------------------------------------------------------------------
LeeADC	nop			;No realiza ninguna tarea										  
LADC	brclr	7,ADSCR,LADC	;Espera a que bit COCO sea 1
	lda	ADR		;Guarda conversion en acumulador
	sta	valADC		;Guarda conversion en valADC
	jsr	DELA050		;Llama al delay de 50ms
	rts			;Retorno de subrutina
;-------------------------------------------------------------------------
;- SI EL VALOR SE ENCUENTRA ENTRE 0 y 20                                 -
;-------------------------------------------------------------------------
E0y20	mov	valADC,valConv	;Mueve valADC a valConv
	jsr	CONLM35		;Rutina de conversion LM35
	jsr	DECTBCD		;Llama a rutina de conversion Dec a BCD
	jsr	MASKDIS		;Llama a rutina para mascara de display
	jsr	SWDISP		;Llama a rutina para Hab o DesHab Display
	rts			;Retorno de subrutina
;-------------------------------------------------------------------------
;- SI EL VALOR SE ENCUENTRA ENTRE 20 y 40                                -
;-------------------------------------------------------------------------
E20y40	mov	valADC,valConv	;Mueve valADC a valConv
	jsr	CONLM35		;Rutina de conversion LM35
	jsr	DECTBCD		;Llama a rutina de conversion Dec a BCD	
	jsr	MASKDIS		;Llama a rutina para mascara de display
	jsr	SWDISP		;Llama a rutina para Hab o DesHab Display						
	rts			;Retorno de subrutina
;-------------------------------------------------------------------------
;- SI EL VALOR SE ENCUENTRA ENTRE 40 y 60                                -
;-------------------------------------------------------------------------
E40y60	mov	valADC,valConv	;Mueve valADC a valConv	
	jsr	CONLM35		;Rutina de conversion LM35
	jsr	DECTBCD		;Llama a rutina de conversion Dec a BCD
	jsr	MASKDIS		;Llama a rutina para mascara de display
	jsr	SWDISP		;Llama a rutina para Hab o DesHab Displays							
	rts			;Retorno de subrutina
;-------------------------------------------------------------------------
;- SI EL VALOR SE ENCUENTRA ENTRE 60 y 80                                -
;-------------------------------------------------------------------------
E60y80	mov	valADC,valConv	;Mueve valADC a valConv
	jsr	CONLM35		;Rutina de conversion LM35
	jsr	DECTBCD		;Llama a rutina de conversion Dec a BCD	
	jsr	MASKDIS		;Llama a rutina para mascara de display
	jsr	SWDISP		;Llama a rutina para Hab o DesHab Display							
	rts			;Retorno de subrutina
;-------------------------------------------------------------------------
;- SI EL VALOR SE ENCUENTRA ENTRE 80 y 100                               -
;-------------------------------------------------------------------------
E80y10 	mov	valADC,valLm35	;Mueve valADC a valLm35
	jsr	CONLM35		;Rutina de conversion LM35
	jsr	DECTBCD		;Llama a rutina de conversion Dec a BCD	
	jsr	MASKDIS		;Llama a rutina para mascara de display
	jsr	SWDISP		;Llama a rutina para Hab o DesHab display				
	rts			;Retorno de subrutina
;-------------------------------------------------------------------------
;- DELAY DE 50ms                                                         -
;-------------------------------------------------------------------------	
DELA050	mov	#$14,Var2	;[5]Se carga el valor 20  a Var2
LOOPE	mov	#$92,Var1	;[5]Se carga el valor 146 a Var1
	dec	Var2		;[4]Se decrementa Var2
	beq	FIN05		;[3]Si Var2=0 salta a FIN05
LOOPI	dec	Var1		;[4]Se decrementa Var1
	beq	LOOPE		;[3]Si Var1=0 salta a LOOPE (Externo)
	bra	LOOPI		;[3]Salta a LOOPI (Interno)
FIN05	rts			;[4]Retorno de subrutina
;-------------------------------------------------------------------------
;- RUTINA DE CONVERSION DE TENSION A GRADOS CELCIUS                      -
;-------------------------------------------------------------------------
CONLM35	jsr	INIT		;Inicializa Acumuladores
	lda	valLm35		;Cargo A con el valor del ADC	
	ldx	#$0A   		;Cargo X con el valor 10	
	div			;Divide por 10								  
	ldx	#$04   		;Cargo X con el valor 4
	mul			;Multiplica por 4
	sta	valConv		;Guarda el valor en valConv
	rts			;Retorno de subrutina
;-------------------------------------------------------------------------
;- CONVERSION DE DECIMAL A BCD                                           -
;-------------------------------------------------------------------------		
DECTBCD	jsr	INIT		;Inicializa Acumuladores
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
;-------------------------------------------------------------------------
;- INICIALIZACION DE ACUMULADORES A, X y H                               -
;-------------------------------------------------------------------------	
INIT	clra			;Inicializa A
	clrx			;Inicializa X
	clrh			;Inicializa H
	rts			;Retorno de subrutina
;-------------------------------------------------------------------------
;- APLICA MASCARA AL PUERTO D, PARTE BAJA UNIDADES, PARTE ALTA DECENAS   -
;-------------------------------------------------------------------------	
MASKDIS	jsr	INIT		;Inicializa Acumuladores
	lda	varDec		;Carga acumulador A con Decenas
	ldx	#$10		;Carga acumulador X con valor 0001 0000
	mul			;Multiplica A*X=A
	add	varUni		;Suma a la multiplicaicon varUni
	sta	PORTD		;Mueve LSB y MSB al puerto D
	rts		
;-------------------------------------------------------------------------
;- ENCIENDE O APAGA DIGITOS EN FUNCION DEL VALOR                         -
;-------------------------------------------------------------------------	
SWDISP  clra			;Inicializa A
	lda	varCen		;Carga acumulador con Centenas			
	cmp	#$01		;Compara con uno
	blo	CENOFF		;Si es menor a uno apaga el display
	mov	#$07,PORTA	;C=1, D=1, U=1
	bra 	FMUES		;Salta al final de la subrutina
CENOFF	mov	#$03,PORTA	;C=0, D=1, U=1
	clra			;Inicializa A
	lda	varDec		;Carga acumulador con Decenas
	cmp	#$01		;Compara con uno
	blo	DECOFF		;Si es menor a uno apaga el display
	mov	#$03,PORTA	;C=0, D=1, U=1
	bra 	FMUES		;Salta al final de la subrutina
DECOFF	mov	#$01,PORTA	;C=0, D=0, U=1				
	clra			;Inicializa A
	lda	varUni		;Carga acumulador con Unidades
	cmp	#$01		;Compara con uno
	blo	UNIOFF		;Si es menor a uno apaga el display
	mov	#$01,PORTA	;C=0, D=0, U=1
	bra	FMUES		;Salta al final de la subrutina
UNIOFF	mov	#$00,PORTA	;C=0, D=0, U=0				
FMUES	rts			;Retorno de subrutina		
;-------------------------------------------------------------------------
;- FINAL                                                                 -
;-------------------------------------------------------------------------
	org 	ResetV		;Se apunta al Vector de Reset
	dw	MAIN		;Salta a MAIN
	end			;Fin del programa

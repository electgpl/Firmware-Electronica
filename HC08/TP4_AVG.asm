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
longTab	rmb	1		;Variable de tabla para el promedio
proTemp	rmb	1		;Variable de tabla para el promedio
indTemp	rmb	1		;Variable de tabla para el promedio
ramTab	rmb	1		;Variable de tabla para el promedio
;
	org	RomSt		;Se apunta al origen de memoria ROM
;-------------------------------------------------------------------------
;- CONFIGURACION                                                         -
;-------------------------------------------------------------------------
MAIN	mov	#$11,CONFIG1	;Deshabilita el COP y LVI
	mov	#$FF,DDRD	;Configura puerto D como salidas
	clr	PORTD		;Inicializa el puerto D	
;-------------------------------------------------------------------------
;- INICIO                                                                -
;-------------------------------------------------------------------------
INICIO	jsr	PROMED		;Llama a rutina de promedio										  
	sta	PORTD		;Muestra en PTD el valor promedio
	bra	INICIO		;Salta a INICIO																														  
;-------------------------------------------------------------------------
;- PROMED                                                                -
;-------------------------------------------------------------------------
PROMED	clrx  			;Inicializa el acumulador X
	clr	proTemp		;Inicializa la vairable proTemp
	mov	#$03,longTab	;Longitud de la tabla de 5 registros
UPDATE 	jsr  	BUSCAVAL  	;Busca el valor y lo deja en el acumulador A
	stx	indTemp		;Guarda el valor del indice
	ldx	longTab		;Valor divisor del promedio = LongTab
	div			;Divide A/X=A
	clrh			;Borra el valor del resto H
	ldx	indTemp		;Restaura el valor del indice
	sta  	RamTab,X  	;Escribe en RamTab con indice X
	incx  			;Incrementa el índice
	cpx	longTab		;Compara el registro X con LongTab
	beq	SIGUE		;Si X=longTab Salta a SIGUE
	bra	UPDATE		;Caso contrario Salta a UPDATE
SIGUE	clrx  			;Inicializa en 00 el acumulador X
READ 	lda  	RamTab,X  	;Lee en RamTab con indice X		
	add	proTemp		;Suma el contenido de A con proTemp
	sta	proTemp		;Guarda el valor temporal 
	incx  			;Incrementa el índice
	cpx	longTab		;Compara el registro X con LongTab
	beq	FINPRO		;Si X=longTab Salta a FINPRO	
	bra	READ		;Caso contrario Salta a READ
FINPRO	rts			;Retorno de subrutina
;-------------------------------------------------------------------------
;- INICIO                                                                -
;-------------------------------------------------------------------------
BUSCAVAL																  
	lda	#$0A													  
	rts																		    													  		   		
;-------------------------------------------------------------------------
;- FINAL                                                                 -
;-------------------------------------------------------------------------
	org 	ResetV		;Se apunta al Vector de Reset
	dw	MAIN		;Salta a MAIN
	end			;Fin del programa

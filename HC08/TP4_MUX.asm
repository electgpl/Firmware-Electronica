;*************************************************************************
;-------------------------------------------------------------------------
$Include 'jl3regs.inc'			;Libreria para el MC68HC908JL3
;-------------------------------------------------------------------------
;- DEFINICION DE CONSTANTES Y VARIABLES                                  -
;-------------------------------------------------------------------------
RamSt	equ		$0080			;Direccion de origen de memoria RAM
RomSt	equ		$EC00			;Direccion de origen de memoria ROM
ResetV	equ		$FFFE			;Vector de Reset
;						
		org		RamSt			;Se apunta al origen de memoria RAM
;
varUni	rmb		1				;Variable para separacion de Unidades
varDec	rmb		1				;Variable para separacion de Decenas
varCen	rmb		1				;Variable para separacion de Centenas
varADC	rmb		1				;Variable para el valor leido por ADC
Var1	rmb		1				;Variable 1Byte para Loop Interno
Var2	rmb		1				;Variable 1Byte para Loop Externo
;
		org		RomSt			;Se apunta al origen de memoria ROM
;-------------------------------------------------------------------------
;- CONFIGURACION                                                         -
;-------------------------------------------------------------------------
MAIN	mov		#$11,CONFIG1	;Deshabilita el COP y LVI
		mov		#$FF,DDRA		;Configura puerto A como salidas
		clr		PORTA			;Inicializa el puerto A
		mov		#$FF,DDRD		;Configura puerto D como salidas
		clr		PORTD			;Inicializa el puerto D
		mov		#$20,ADCLK		;Divisor por 1, Reloj interno 0010 0000
		mov		#$20,ADSCR		;Conv.Continua, ADC PTB0 x010 0000					
;-------------------------------------------------------------------------
;- INICIO                                                                -
;-------------------------------------------------------------------------
INICIO 	jsr		LEEADC			;Rutina de lectura de ADC
		jsr		DECTBCD			;Rutina de conversion de Decimal a BCD	
		jsr		MUXDISP			;Rutina de Multiplexado de displays		
		jmp		INICIO			;Loop del bloque principal							  														 																  																	  											  
;-------------------------------------------------------------------------
;- LEER ADC EN PUERTO PTB0                                               -
;-------------------------------------------------------------------------
LEEADC	brclr	7,ADSCR,LEEADC	;Espera a que bit COCO sea 1
		lda		ADR				;Guarda conversion en acumulador
		sta		varADC			;Guarda el valor leido por ADC en varADC
		rts						;Retorno de subrutina	
;-------------------------------------------------------------------------
;- CONVERSION DE DECIMAL A BCD SEPARADO POR DIGITOS                      -
;-------------------------------------------------------------------------
DECTBCD	ldhx	#$0A			;Carga X con 10
		div						;Divide A/X=A, Resto en H
		sthx	varUni			;Mueve H a varUni
		clrh					;Borra H
		ldhx	#$0A			;Carga X con 10
		div						;Divide A/X=A
		sthx	varDec			;Mueve H a varDec
		sta		varCen			;Mueve A a varCen
		rts						;Retorno de subrutina																  
;-------------------------------------------------------------------------
;- MULTIPLEXADO DE DISPLAYS                                              -
;-------------------------------------------------------------------------
MUXDISP	clrh
		ldx		varCen			;Carga el valor de Centenas en X										  
		lda		TABLA,X			;Busca en la tabla el equivalente
		bset	0,PORTA			;Prendo Display en PTA0
		sta		PORTD			;Carga el PORTD con el valor de la tabla
		bclr	1,PORTA			;Apago Display en PTA1
		bclr	2,PORTA			;Apago Display en PTA2
;		jsr		DELA050			;Llama a rutina de Delay 50ms
		ldx		varDec			;Carga el valor de Decenas en X	
		lda		TABLA,X			;Busca en la tabla el equivalente
		bset	1,PORTA			;Prendo Display en PTA1
		sta		PORTD			;Carga el PORTD con el valor de la tabla
		bclr	0,PORTA			;Apago Display en PTA0
		bclr	2,PORTA			;Apago Display en PTA2
;		jsr		DELA050			;Llama a rutina de Delay 50ms
		ldx		varUni			;Carga el valor de Unidades en X
		lda		TABLA,X			;Busca en la tabla el equivalente
		bset	2,PORTA			;Prendo Display en PTA1
		sta		PORTD			;Carga el PORTD con el valor de la tabla
		bclr	0,PORTA			;Apago Display en PTA0
		bclr	1,PORTA			;Apago Display en PTA2
;		jsr		DELA050			;Llama a rutina de Delay 50ms
		rts						;Retorno de subrutina	
;-------------------------------------------------------------------------
;- RUTINA DE DELAY DE 50ms                                               -
;-------------------------------------------------------------------------
DELA050	mov		#$14,Var2		;[5]Se carga el valor 20  a Var2
LOOPE	mov		#$92,Var1		;[5]Se carga el valor 146 a Var1
		dec		Var2			;[4]Se decrementa Var2
		beq		FIN05			;[3]Si Var2=0 salta a FIN05
LOOPI	dec		Var1			;[4]Se decrementa Var1
		beq		LOOPE			;[3]Si Var1=0 salta a LOOPE (Externo)
		bra		LOOPI			;[3]Salta a LOOPI (Interno)
FIN05	rts						;[4]Retorno de subrutina
;-------------------------------------------------------------------------
;- TABLA DE VALORES PARA EL DISPLAY DE 7 SEGMENTOS                       -
;- Bits del display XGFEDCBA		   	 								 -
;-------------------------------------------------------------------------
TABLA  db %00111111  			;Numero 0										       
       db %00000110  			;Numero 1	
       db %01011011  			;Numero 2	
       db %01001111  			;Numero 3	
       db %01100110  			;Numero 4	
       db %01101101  			;Numero 5	
       db %01111100  			;Numero 6	
       db %00000111  			;Numero 7	
       db %01111111  			;Numero 8	
       db %01100111  			;Numero 9													    													  		   		
;-------------------------------------------------------------------------
;- FINAL                                                                 -
;-------------------------------------------------------------------------
		org 	ResetV			;Se apunta al Vector de Reset
		dw		MAIN			;Salta a MAIN
		end						;Fin del programa
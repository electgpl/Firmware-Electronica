;*************************************************************************
; TERMOSATO ELECTRONICO CON DISPLAY DE MONITOREO Y ALARMA
; SENSOR DE TEMPERATURA LM35 10mV/°C EN PTB4
; DISPLAY TRIPLE DE 7 SEGMENTOS EN CATODO COMUN SOBRE EL PUERTO D
; TRANSISTORES ECG123AP DE CONMUTACION DE MULTIPLEXADO DE PTB0 a PTB2
; SALIDA PARA LEDS DE MONITOREO DE ESTADO DE PTA2 a PTA4
; SALIDA DE AISLACION PARA INTERFASE DE CALFEACTOR PTA1
; SALIDA DE AISLACION PARA INTERFASE DE REFRIGERACION PTA0
; MICRCONTROLADOR MC68HC908JL3 CON CRISTAL DE 4MHz
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
varUni	rmb	1		;Variable para separacion de Unidades
varDec	rmb	1		;Variable para separacion de Decenas
varCen	rmb	1		;Variable para separacion de Centenas
varADC	rmb	1		;Variable para el valor leido por ADC
varDel	rmb	1		;Variable para Loop del Delay
varMue	rmb	1		;Variable para el muestreo del ADC
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
	mov	#$FF,DDRA	;Configura puerto A como salidas
	clr	PORTA		;Inicializa el puerto A
	mov	#$FF,DDRB	;Configura puerto B como salidas
	clr	PORTB		;Inicializa el puerto B
	mov	#$FF,DDRD	;Configura puerto D como salidas
	clr	PORTD		;Inicializa el puerto D	
	mov	#$20,ADCLK	;Divisor por 1, Reloj interno 0010 0000
	mov	#$24,ADSCR	;Conv.Continua, ADC PTB0 x010 0100					
;-------------------------------------------------------------------------
;- INICIO                                                                -
;-------------------------------------------------------------------------
INICIO 	clrx			;Inicializa acumulador X	
	clrh			;Inicializa acumulador H	
	dec	varMue		;Decrementa variable de muestreo
	beq	ACTUA		;Actualiza medicion
	bra	NOACTUA		;Continua sin actualizar
ACTUA	mov	#$3F,varMue	;Carga varMue con 63
	jsr	PROMED		;Rutina de adquisicion de datos
	ldx	#$05		;Valor 5
	div			;A/X=A
	ldx	#$02		;Valor 2
	mul			;A*X=A	
	sta	varADC		;Guarda el valor leido por ADC en varADC
NOACTUA	jsr	DECTBCD		;Rutina de conversion de Decimal a BCD	
	jsr	MUXDISP		;Rutina de Multiplexado de displays		
	jsr	COMPARA		;Rutina para comparar temperatura
	jmp	INICIO		;Loop del bloque principal				
;-------------------------------------------------------------------------
;- PROMED                                                                -
;-------------------------------------------------------------------------
PROMED	clrx  			;Inicializa el acumulador X
	clr	proTemp		;Inicializa la vairable proTemp
	mov	#$08,longTab	;Longitud de la tabla de 8 registros
UPDATE 	jsr	LEEADC		;Rutina de lectura de ADC
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
;- LEER ADC EN PUERTO PTB0                                               -
;-------------------------------------------------------------------------
LEEADC	brclr	7,ADSCR,LEEADC	;Espera a que bit COCO sea 1
	lda	ADR		;Guarda conversion en acumulador
	rts			;Retorno de subrutina	
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
;- MULTIPLEXADO DE DISPLAYS                                              -
;-------------------------------------------------------------------------
MUXDISP	clrh
	ldx	varCen		;Carga el valor de Centenas en X										  
	lda	TABLA,X		;Busca en la tabla el equivalente
	mov	#$01,PORTB	;xxxxx001, Disp1=1,Disp2=0,Disp3=0
	sta	PORTD		;Carga el PORTD con el valor de la tabla
	jsr	DELA001		;Llama a rutina de Delay 1ms
	ldx	varDec		;Carga el valor de Decenas en X	
	lda	TABLA,X		;Busca en la tabla el equivalente
	mov	#$02,PORTB	;xxxxx010, Disp1=0,Disp2=1,Disp3=0
	sta	PORTD		;Carga el PORTD con el valor de la tabla
	jsr	DELA001		;Llama a rutina de Delay 1ms
	ldx	varUni		;Carga el valor de Unidades en X
	lda	TABLA,X		;Busca en la tabla el equivalente
	mov	#$04,PORTB	;xxxxx100, Disp1=0,Disp2=0,Disp3=1
	sta	PORTD		;Carga el PORTD con el valor de la tabla
	jsr	DELA001		;Llama a rutina de Delay 1ms
	rts			;Retorno de subrutina	
;-------------------------------------------------------------------------
;- RUTINA DE DELAY DE 2,5ms                                              -
;- (9+(10*99))+5=1004us=+/-1ms                                           -
;-------------------------------------------------------------------------
DELA001	mov	#$63,varDel	;[5]Se carga el valor 99 a varDel
LOOP	dec	varDel		;[4]Se decrementa varDel
	beq	FIN05		;[3]Si varDel=0 salta a FIN05 
	bra	LOOP		;[3]Salta a LOOP
FIN05	rts			;[4]Retorno de subrutina
;-------------------------------------------------------------------------
;- TABLA DE VALORES PARA EL DISPLAY DE 7 SEGMENTOS                       -
;- Bits del display XGFEDCBA		   	 			 -
;-------------------------------------------------------------------------
TABLA  	db 	%00111111  	;Numero 0										       
       	db 	%00000110  	;Numero 1	
       	db 	%01011011  	;Numero 2	
       	db 	%01001111  	;Numero 3	
       	db 	%01100110  	;Numero 4	
       	db 	%01101101  	;Numero 5	
       	db 	%01111100  	;Numero 6	
       	db 	%00000111  	;Numero 7	
       	db 	%01111111  	;Numero 8	
       	db 	%01100111  	;Numero 9	
;-------------------------------------------------------------------------
;- COMPARA VALORES DE TEMPERATURA PARA EL CONTROL EXTERNO                -
;-------------------------------------------------------------------------	
COMPARA	lda	varADC		;Carga valor ADC en Acumulador A																																											
	cmp	#$50		;Compara ADC con 80
	bhi	GT80		;Si es mayor, salta a GT80
	cmp	#$3C		;Compara ADC con 60 
	bhi	GT60		;Si es mayor, salta a GT60
	cmp	#$28		;Compara ADC con 40 
	bhi	GT40		;Si es mayor, salta a GT40
	cmp	#$14		;Compara ADC con 20  
	bhi	GT20		;Si es mayor, salta a GT20	
	mov	#$12,PORTA	;xxx10010, R=1,A=0,V=0,C=1,F=0
	jmp	FINCOM		;Sale de la subrutina
GT20	mov	#$0A,PORTA	;xxx01010, R=0,A=1,V=0,C=1,F=0
	jmp	FINCOM		;Sale de la subrutina
GT40	mov	#$04,PORTA	;xxx00100, R=0,A=0,V=1,C=0,F=0
	jmp	FINCOM		;Sale de la subrutina
GT60	mov	#$09,PORTA	;xxx01001, R=0,A=1,V=0,C=0,F=1
	jmp	FINCOM		;Sale de la subrutina
GT80	mov	#$11,PORTA	;xxx10001, R=1,A=0,V=0,C=0,F=1
	jmp	FINCOM		;Sale de la subrutina
FINCOM  rts			;Retorno de subrutina											    													  		   		
;-------------------------------------------------------------------------
;- FINAL                                                                 -
;-------------------------------------------------------------------------
	org 	ResetV		;Se apunta al Vector de Reset
	dw	MAIN		;Salta a MAIN
	end			;Fin del programa

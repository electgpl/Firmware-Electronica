;-------------------------------------------------------------------------
;- MONITOR CARDIACO CON MODULO TIM                      -
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
varDel	rmb		1				;Variable para Loop del Delay
varCont	rmb		1				;Variable para el contador de pulsos
varMue	rmb		1				;Variable para el muestreo
;
		org		RomSt			;Se apunta al origen de memoria ROM
;-------------------------------------------------------------------------
;- CONFIGURACION                                                         -
;-------------------------------------------------------------------------
MAIN	mov		#$11,CONFIG1	;Deshabilita el COP y LVI
		mov		#$FF,DDRA		;Configura puerto A como salidas
		clr		PORTA			;Inicializa el puerto A
		mov		#$FF,DDRB		;Configura puerto B como salidas
		clr		PORTB			;Inicializa el puerto B	
;-------------------------------------------------------------------------
;- CONFIGURACION DEL TIMER Y CAPTURA                                     -
;-------------------------------------------------------------------------
CONFIG  bset    5,TSC           ;Timer (STOP)
        mov     #$04,TSC0       ;Captura por flanco de subida ELS0A
		clra					;inicializo A
		mov		#$00,varCont	;Inicializa varCont
		mov		#$03,varMue		;Inicializa varMue
		jsr		DEC2BCD			;Rutina de conversion de Decimal a BCD
;-------------------------------------------------------------------------
;- INICIO                                                                -
;-------------------------------------------------------------------------	
INICIO	bset    4,TSC           ;reset de contador TCNT y prescaler
        mov     #$26,TSC        ;Inicializa timer (STOP), prescaler:64
        bclr    5,TSC           ;Timer (START)       
		clra					;Inicializa A
PULSO   brclr   7,TSC0,SIGUE    ;Espera flanco de subida TOF 
        bclr    7,TSC0          ;Baja flag              
        inc     varCont         ;Sumo varCont=varCont+1		
SIGUE	jsr		MUXDISP			;Rutina de Multiplexado de displays
   		brclr   7,TSC,PULSO     ;Espera a overflow
        bclr    7,TSC           ;Baja flag
		dec		varMue			;Decrementa varMue		
		beq		UPDATE			;Si varMue=0 salta a UPDATE
		bra		INICIO			;Si no es igual a 0 salta a INICIO
UPDATE	mov		#$03,varMue		;Asigna valor 3 a varMue
		ldx		#$04			;Cargo 4 en X
		lda		varCont			;Cargo varCont en A
		mul						;A*X=A
		jsr		DEC2BCD			;Rutina de conversion de Decimal a BCD
		mov		#$00,varCont	;Inicializa varCont
		jmp		INICIO			;Salta a INICIO
;-------------------------------------------------------------------------
;- CONVERSION DE DECIMAL A BCD SEPARADO POR DIGITOS                      -
;-------------------------------------------------------------------------
DEC2BCD	ldhx	#$0A			;Carga X con 10
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
		mov		#$01,PORTA		;xxxxx001, Disp1=1,Disp2=0,Disp3=0
		sta		PORTB			;Carga el PORTB con el valor de la tabla
		jsr		DELA100			;Llama a rutina de Delay 100us
		ldx		varDec			;Carga el valor de Decenas en X	
		lda		TABLA,X			;Busca en la tabla el equivalente
		mov		#$02,PORTA		;xxxxx010, Disp1=0,Disp2=1,Disp3=0
		sta		PORTB			;Carga el PORTB con el valor de la tabla
		jsr		DELA100			;Llama a rutina de Delay 100us
		ldx		varUni			;Carga el valor de Unidades en X
		lda		TABLA,X			;Busca en la tabla el equivalente
		mov		#$04,PORTA		;xxxxx100, Disp1=0,Disp2=0,Disp3=1
		sta		PORTB			;Carga el PORTB con el valor de la tabla
		jsr		DELA100			;Llama a rutina de Delay 100us
		rts						;Retorno de subrutina	
;-------------------------------------------------------------------------
;- RUTINA DE DELAY DE 100us                                              -
;- (9+(10*9))+5=104us                                                    -
;-------------------------------------------------------------------------
DELA100	mov		#$09,varDel		;[5]Se carga el valor 9 a varDel
LOOP	dec		varDel			;[4]Se decrementa varDel
		beq		FIN05			;[3]Si varDel=0 salta a FIN05 
		bra		LOOP			;[3]Salta a LOOP
FIN05	rts						;[4]Retorno de subrutina		
;-------------------------------------------------------------------------
;- TABLA DE VALORES PARA EL DISPLAY DE 7 SEGMENTOS                       -
;- Bits del display XGFEDCBA		   	 								 -
;-------------------------------------------------------------------------
TABLA  	db %00111111  			;Numero 0										       
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
		end						;Fin del programaa
;******************************************************************
;* Programa para realizar un Semaforo Transito y Peatonal con Timer
;* Se utilizan los 5 bit LSB del PTD para gobernar los leds
;* Rojo, Amarillo, Verde, Blanco(peatonal) y habilitacion del 4511.
;* Se utilizan los 4 biy LSB del PTA para interconectar un
;* decodificador 4511 BCD to 7Segment, La habilitacion del display
;* se realizara con el bit 4 de PTD.
;* Delay 500ms ((12+185)*(10*255))+13=499813us=+/-500ms
;******************************************************************
$Include 'jl3regs.inc'		;Libreria para el MC68HC908JL3
;
RamSt	equ	$0080		;Direccion de origen de memoria RAM
RomSt	equ	$EC00		;Direccion de origen de memoria ROM
ResetV	equ	$FFFE		;Vector de Reset
DispL	equ	$01		;Se carga valor 1 a DispL
DispH	equ	$0A		;Se carga valor 10 a DispH
;						
	org	RamSt		;Se apunta al origen de memoria RAM
;
Var1	rmb	1		;Variable 1Byte para Loop Interno
Var2	rmb	1		;Variable 1Byte para Loop Externo
Del2	rmb	1		;Variable 1Byte para Delay de 2s
Del5	rmb	1		;Variable 1Byte para Delay de 5s
Del20	rmb	1		;Variable 1Byte para Delay de 20s		
Del30	rmb	1		;Variable 1Byte para Delay de 30s
VarDisp	rmb	1		;Variable 1Byte para Loop Display
;
	org	RomSt		;Se apunta al origen de memoria ROM
;
MAIN	mov	#$11,CONFIG1	;Deshabilita el COP y LVI
	mov	#$FF,DDRD	;Configura puerto D como salidas
	clr	PORTD		;Inicializa el puerto D
	mov	#$FF,DDRA	;Configura puerto A como salidas
	clr	PORTA		;Inicializa el puerto A
;
INICIO	mov 	#$02,PTD	;D=0,R=0,A=0,V=1,B=0    0000 0010 02
	jsr	DELA30		;Llama al delay de 30s
	mov 	#$04,PTD	;D=0,R=0,A=1,V=0,B=0    0000 0100 04
	jsr	DELA2		;Llama al delay de 2s
	mov 	#$08,PTD	;D=0,R=1,A=0,V=0,B=0    0000 1000 08
	jsr	DELA5		;Llama al delay de 5s
	mov 	#$09,PTD	;D=0,R=1,A=0,V=0,B=1    0000 1001 09
	jsr	DELA20		;Llama al delay de 20s
	mov 	#$18,PTD	;D=1,R=1,A=0,V=0,B=0    0001 1000 18
	jsr	DISP		;Llama a la rutina Display Regresivo
	mov 	#$0C,PTD	;D=0,R=1,A=1,V=0,B=0    0000 1100 0C
	jsr	DELA2		;Llama al delay de 2s
	bra	INICIO		;Repite el ciclo
;	
DISP	lda	#DispH		;Carga el Acumulador con 10
LOOPDIS	deca			;Decrementa el Acumulador
	sta	PTA		;Copia en LSB del PTA el valor Acum
 	cmp	#DispL		;Compara Si el Acumulador=1
	beq     INDIS		;Si son iguales retorna a subrutina
	jsr	DELA05		;Llama a demora de 500ms
	bra	LOOPDIS		;Salta a LOOPDIS
FINDIS	rts			;Retorno de subrutina		
;		
DELA30	mov	#$3C,Del30	;Se carga 60 a Del30
LOOP30	dec	Del30		;Se decrementa Del30
	beq	FIN30		;Si Del30=0 salta a FIN30
	jsr	DELA05		;Se llama a la rutina DELA05 de 500ms
	bra	LOOP30		;Salta a LOOP30
FIN30	rts			;Retorno de subrutina
;		
DELA20	mov	#$28,Del20	;Se carga 40 a Del20
LOOP20	dec	Del20		;Se decrementa Del20
	beq	FIN20		;Si Del20=0 salta a FIN20
	jsr	DELA05		;Se llama a la rutina DELA05 de 500ms
	bra	LOOP20		;Salta a LOOP20
FIN20	rts			;Retorno de subrutina
;		
DELA5	mov	#$0A,Del5	;Se carga 10 a Del5
LOOP5	dec	Del5		;Se decrementa Del5
	beq	FIN5		;Si Del5=0 salta a FIN5
	jsr	DELA05		;Se llama a la rutina DELA05 de 500ms
	bra	LOOP5		;Salta a LOOP5
FIN5	rts			;Retorno de subrutina
;		
DELA2	mov	#$04,Del2	;Se carga 4 a Del2
LOOP2	dec	Del2		;Se decrementa Del2
	beq	FIN2		;Si Del2=0 salta a FIN2
	jsr	DELA05		;Se llama a la rutina DELA05 de 500ms
	bra	LOOP2		;Salta a LOOP2
FIN2	rts			;Retorno de subrutina
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

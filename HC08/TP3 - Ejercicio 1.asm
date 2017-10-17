;******************************************************************
;* Programa que prende y apaga un led con un periodo de 1Hz
;* con ciclo de trabajo del 50%. (tON=500ms, tOFF=500ms)
;* LED en salida PTD0.
;* Delay 500ms ((12+185)*(10*255))+13=499813us=+/-500ms
;******************************************************************
$Include 'jl3regs.inc'			;Libreria para el MC68HC908JL3
;
RamSt	equ		$0080			;Direccion de origen de memoria RAM
RomSt	equ		$EC00			;Direccion de origen de memoria ROM
ResetV	equ		$FFFE			;Vector de Reset
;						
		org		RamSt			;Se apunta al origen de memoria RAM
;
Var1	rmb		1				;Variable de 1 byte
Var2	rmb		1				;Variable de 1 byte
;
		org		RomSt			;Se apunta al origen de memoria ROM
;
MAIN	mov		#$11,CONFIG1	;Deshabilita el COP y LVI
		mov		#$FF,DDRD		;Configura puerto D como salidas
		clr		PORTD			;Inicializa el puerto D
;
INICIO	bset	0,PORTD			;Setea a 1 la salida 0 del puerto D
		bsr		DELAY			;[4]Llama al delay de 499.8ms
		bclr	0,PORTD			;Setea a 0 la salida 0 del puerto D
		bsr		DELAY			;[4]Llama al delay de 499.8ms
		bra		INICIO			;Repite el ciclo
;				
DELAY	mov		#$B9,Var2		;[5]Se carga el valor 185 a Var2
LOOP2	mov		#$FF,Var1		;[5]Se carga el valor 255 a Var1
		dec		Var2			;[4]Se decrementa Var2
		beq		FIN				;[3]Si Var2=0 salta a FIN
LOOP1	dec		Var1			;[4]Se decrementa Var1
		beq		LOOP2			;[3]Si Var1=0 salta a LOOP2 (Externo)
		bra		LOOP1			;[3]Salta a LOOP1 (Interno)
FIN		rts						;[4]Retorno de subrutina
;
		org 	ResetV			;Se apunta al Vector de Reset
		dw		MAIN			;Salta a MAIN
		end						;Fin del programa
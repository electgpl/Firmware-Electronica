;******************************************************************
; Programa de prueba de modo Captura. Se ingresa una señal de ciclo 
; útil variable por el pin TCH0. Si el ciclo útil es menor o igual 
; al 50% se pone a uno PTA1, si el ciclo útil es mayor del 50% se 
; pone a cero el PTA1. El tiempo mínimo entre flancos debe ser de 
; 32 ciclos. 
;******************************************************************
$Include 'jl3regs.inc'		;Libreria para el MC68HC908JL3
;
RamSt	equ	$0080		;Direccion de origen de memoria RAM
RomSt	equ	$EC00		;Direccion de origen de memoria ROM
ResetV	equ	$FFFE		;Vector de Reset
;						
	org	RamSt		;Se apunta al origen de memoria RAM
;
	org	RomSt		;Se apunta al origen de memoria ROM
;
MAIN	mov	#$11,CONFIG1	;Deshabilita el COP y LVI
	mov	#$FF,DDRB	;Configura puerto B como salidas
	clr	PORTB		;Inicializa el puerto B
;
CONFIG  bset    5,TSC           ;Timer (STOP)
        mov     #$F4,TMODH      ;Cargo limites de cuenta para el timer
        mov     #$24,TMODL      ;valor F424=62500. 62500/15625Hz=4s
        mov     #$04,TSC0       ;Captura por flanco de subida ELS0A
INICIO  bset    4,TSC           ;reset de contador TCNT y prescaler
        mov     #$26,TSC        ;Inicializa timer (STOP), prescaler:64
        bclr    5,TSC           ;Timer (START)       
        clra                    ;Inicializa acumulador A
PULSO   brclr   7,TSC0,SIGUE    ;Espera flanco de subida TOF 
        bclr    7,TSC0          ;Baja flag              
        inca                    ;Sumo A=A+1
SIGUE   brclr   7,TSC,PULSO     ;Espera a overflow
        bclr    7,TSC           ;Baja flag
	ldx	#$0F		;Cargo 15 en X
	mul			;A*X=A
        sta     PORTB           ;Muestra valor A en PORTB
	clra			;Inicializa acumulador A
        jmp     INICIO          ;Salta a INICIO
;
	org 	ResetV		;Se apunta al Vector de Reset
	dw	MAIN		;Salta a MAIN
	end			;Fin del programa

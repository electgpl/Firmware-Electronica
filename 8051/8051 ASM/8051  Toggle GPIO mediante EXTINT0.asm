;-------------------------------------------------------------------------------------
;Programa que realiza el toggle de una salida cuando entra una interrupción externa
;en el canal de interrupción 0 por flanco.    
;
;                Registro IE       
; EA | -- | -- | ES | ET1 | EX1 | ET0 | EX0 |  
;  0    0    0    0     0     0     0     1  
;   
;                Registro IP       
; -- | -- | PT2 | PS | PT1 | PX1 | PT0 | PX0 |  
;  0    0     0    0     0     0     0     0     
;
;                Registro TCON       
; TF1 | TR1 | TF0 | TR0 | IE1 | IT1 | IE0 | IT0 |  
;  0     0     0     0     0     0     0     1    
;                 
;Archivo: toggleEXTINT0.s03 
;Fecha de realización: 19/06/2017
;Autor: Sebastian Caccavallo - electgpl.blogspot.com.ar
;Modificación:
;-------------------------------------------------------------------------------------
            ORG     0000H       ;Dirección del origen del programa
            JMP     SETUP       ;Salta al Inicio del programa
;            
            ORG     0003H       ;Dirección de interrupción externa INT0
            JMP     EXT_INT0    ;Salta a subrutina de interrupción 
;
FLAG        EQU     50H         ;Flag donde de presencia de flanco
;            
SETUP:      MOV     TCON,#01H   ;Configura IT0 externa por flanco
            MOV     IE,#01H     ;Configura EX0 Interrupción Externa 0
            MOV     IP,#00H     ;Des-habilita prioridad de interrupt
            SETB    EA          ;Habilita interrupción global
            CLR     FLAG        ;Flag a FALSE o 0
;            
LOOP:       JNB     FLAG,LOOP   ;Espera a que FLAG sea TRUE
            JMP     $           ;Queda aquí de forma indeterminada

EXT_INT0:   SETB    FLAG        ;Pone FLAG a TRUE al entrar en la interrupción  
            CPL     P2.2        ;Toggle en la salida P2.2
            RETI                ;Retorno de interrupción
;            
            END                 ;Fin del programa
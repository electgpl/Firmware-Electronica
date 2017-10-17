;-------------------------------------------------------------------------------------
;Programa de demostración de multiplexado de 4 displays mediante delay bloqueante y 
;valores estáticos predefinidos por el usuario. con 8051 @ 8952.                    
;Archivo: Demo4Disp.s03
;Fecha de realización: 24/07/2017
;Autor: Sebastian Caccavallo - electgpl.blogspot.com.ar
;Modificación:
;-------------------------------------------------------------------------------------
            ORG     0000H           ;Dirección del origen del programa
            JMP     SETUP           ;Salta al Inicio del programa
;        
SETUP:      CLR		P1				;Inicializa P1
			CLR		P3				;Inicializa P3 
;                        
LOOP:       MOV     P3,#00000001B   ;Enciende Display x1
            MOV     P1,#00000110B   ;Valor del Display "1"
            CALL    DELAY           ;Llama a la demora
            MOV     P3,#00000010B   ;Enciende Display x10
            MOV     P1,#01011011B   ;Valor del Display "2"
            CALL    DELAY           ;Llama a la demora
            MOV     P3,#00000100B   ;Enciende Display x100
            MOV     P1,#01001111B   ;Valor del Display "3"
            CALL    DELAY           ;Llama a la demora
            MOV     P3,#00001000B   ;Enciende Display x1000
            MOV     P1,#01100110B   ;Valor del Display "4"
            CALL    DELAY           ;Llama a la demora                    
            JMP     LOOP            ;Vuelve a comenzar el ciclo
;
DELAY:      MOV     R0,#255         ;Carga R0 con 255
            DJNZ    R0,$            ;Decremento R0
            RET                     ;Retorna subrutina
;             
            END                     ;Fin del programa
;-------------------------------------------------------------------------------------
;Programa que compara un valor constante (en este caso 82h) contra el valor
;que ingresa por el puerto P0, si el valor es menor, igual o mayor, 
;encenderá el led correspondiente. con 8051 @ 8052.                  
;Archivo: ledMenIguMay.s03 
;Fecha de realización: 17/06/2017
;Autor: Sebastian Caccavallo - electgpl.blogspot.com.ar
;Modificación:
;-------------------------------------------------------------------------------------
            ORG     0000H           ;Dirección del origen del programa
            JMP     SETUP           ;Salta al Inicio del programa    
;
CONST       EQU     50H             ;Definición de variable CONST
;        
SETUP:      MOV     CONST,#82H      ;Cargamos el valor constante que comparamos  
            CLR     P1              ;Borramos el puerto P1
;    
LOOP:       MOV     A,P0            ;Cargamos el valor del puerto P0 en el Acumulador
            CALL    COMPARA         ;Llama a la subrutina de comparación   
            SJMP    LOOP            ;Realiza el loop infinito 
;    
COMPARA:    MOV     A,P0            ;Cargamos el valor del puerto P0 en el Acumulador
            CJNE    A,CONST,NOTEQ   ;Si A != CONST, si es distinto salta a NOTEQ
            SJMP    IGUAL           ;Si A = CONST, si es igual salta a IGUAL
NOTEQ:      JC      MENOR           ;Si Carry = 1, salta a MENOR
            SJMP    MAYOR           ;Si Carry = 0, salta a MAYOR
IGUAL:      SETB    P1.1            ;Enciende led igual 
            CLR     P1.0            ;Apaga led mayor
            CLR     P1.2            ;Apaga led menor
            RET                     ;Retorno de subrutina
MAYOR:      SETB    P1.0            ;Enciende led mayor 
            CLR     P1.2            ;Apaga led menor
            CLR     P1.1            ;Apaga led igual
            RET                     ;Retorno de subrutina 
MENOR:      SETB    P1.2            ;Enciende led menor   
            CLR     P1.0            ;Apaga led igual
            CLR     P1.1            ;Apaga led mayor
            RET                     ;Retorno de subrutina
;
            END                     ;Fin del programa
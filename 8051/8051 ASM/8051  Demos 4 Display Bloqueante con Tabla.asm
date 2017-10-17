;-------------------------------------------------------------------------------------
;Programa de demostración de multiplexado de 4 displays mediante delay bloqueante y 
;valores estáticos predefinidos por el usuario utilizando tabla de datos. con 8051 @ 8952.                    
;Archivo: Demo4DispTab.s03
;Fecha de realización: 24/07/2017
;Autor: Sebastian Caccavallo - electgpl.blogspot.com.ar
;Modificación:
;-------------------------------------------------------------------------------------
            ORG     0000H           ;Dirección del origen del programa
            JMP     SETUP           ;Salta al Inicio del programa
;        
SETUP:      CLR     P1              ;Inicializa P1
            CLR     P3              ;Inicializa P3 
            MOV     DPTR,#TABLE
;                        
LOOP:       MOV     P3,#00000001B   ;Enciende Display x1 
            MOV     A,#01H          ;Carga el numero 1 a traer de la tabla
            MOVC    A,@A+DPTR       ;Trae el valor cargado en acum desde la tabla
            MOV     P1,A            ;Muestra el valor del acum en el puerto
            CALL    DELAY           ;Llama a la demora
            MOV     P3,#00000010B   ;Enciende Display x10 
            MOV     A,#02H          ;Carga el numero 1 a traer de la tabla
            MOVC    A,@A+DPTR       ;Trae el valor cargado en acum desde la tabla
            MOV     P1,A            ;Muestra el valor del acum en el puerto
            CALL    DELAY           ;Llama a la demora
            MOV     P3,#00000100B   ;Enciende Display x100   
            MOV     A,#03H          ;Carga el numero 1 a traer de la tabla
            MOVC    A,@A+DPTR       ;Trae el valor cargado en acum desde la tabla
            MOV     P1,A            ;Muestra el valor del acum en el puerto
            CALL    DELAY           ;Llama a la demora  
            MOV     P3,#00001000B   ;Enciende Display x100   
            MOV     A,#04H          ;Carga el numero 1 a traer de la tabla
            MOVC    A,@A+DPTR       ;Trae el valor cargado en acum desde la tabla
            MOV     P1,A            ;Muestra el valor del acum en el puerto
            CALL    DELAY           ;Llama a la demora                
            JMP     LOOP            ;Vuelve a comenzar el ciclo
;
DELAY:      MOV     R0,#255         ;Carga R0 con 255
            DJNZ    R0,$            ;Decremento R0
            RET                     ;Retorna subrutina
;
TABLE:      DB      00111111B       ;Numero 0 en Display de 7 segmentos  
            DB      00000110B       ;Numero 1 en Display de 7 segmentos 
            DB      01011011B       ;Numero 2 en Display de 7 segmentos 
            DB      01001111B       ;Numero 3 en Display de 7 segmentos 
            DB      01100110B       ;Numero 4 en Display de 7 segmentos 
            DB      01101101B       ;Numero 5 en Display de 7 segmentos 
            DB      01111101B       ;Numero 6 en Display de 7 segmentos 
            DB      00000111B       ;Numero 7 en Display de 7 segmentos 
            DB      01111111B       ;Numero 8 en Display de 7 segmentos 
            DB      01101111B       ;Numero 9 en Display de 7 segmentos        
; 
            END                     ;Fin del programa
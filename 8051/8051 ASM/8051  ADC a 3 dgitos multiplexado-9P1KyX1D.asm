;-------------------------------------------------------------------------------------
;Programa que muestra el valor del P2 en 3 display de 7 segmentos multiplexados
;Se puede reemplazar el P2 por un ADC de 8 bit para mostrar de 0 a 255. con 8051 @ 8952.                    
;Archivo: ADC7SegMux.s03
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
            CALL    DEC2BCD         ;Llama a rutina de conversión DEC a BCD
            MOV     A,R2            ;Carga acum con el valor a buscar en tabla
            MOVC    A,@A+DPTR       ;Trae el valor cargado en acum desde la tabla 
            MOV     P1,A            ;Muestra el valor del acum en el puerto 
            CALL    DELAY           ;Llama a la demora
            MOV     P3,#00000010B   ;Enciende Display x10      
            CALL    DEC2BCD         ;Llama a rutina de conversión DEC a BCD
            MOV     A,R1            ;Carga acum con el valor a buscar en tabla
            MOVC    A,@A+DPTR       ;Trae el valor cargado en acum desde la tabla 
            MOV     P1,A            ;Muestra el valor del acum en el puerto
            CALL    DELAY           ;Llama a la demora
            MOV     P3,#00000100B   ;Enciende Display x100   
            CALL    DEC2BCD         ;Llama a rutina de conversión DEC a BCD
            MOV     A,R0            ;Carga acum con el valor a buscar en tabla
            MOVC    A,@A+DPTR       ;Trae el valor cargado en acum desde la tabla 
            MOV     P1,A            ;Muestra el valor del acum en el puerto
            CALL    DELAY           ;Llama a la demora              
            JMP     LOOP            ;Vuelve a comenzar el ciclo 
;       
DEC2BCD:    MOV     A,P2            ;Carga acum con el valor del ADC del puerto P2
            MOV     B,#10D          ;Carga B con 10
            DIV     AB              ;Divide A/B=A, Resto en B
            MOV     R0,B            ;Mueve Resto B a R0 
            MOV     B,#10D          ;Carga B con 10
            DIV     AB              ;Divide A/B=A, Resto en B
            MOV     R1,B            ;Mueve Resto B a R1
            MOV     R2,A            ;Mueve Resultado A a R2      
            RET                     ;Retorna subrutina
;
DELAY:      MOV     R7,#255         ;Carga R7 con 255
            DJNZ    R7,$            ;Decremento R7
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
;-------------------------------------------------------------------------------------
;Programa que envía una cadena de caracteres por UART cada vez que recibe 
;un dato por UART, con 8051 @ 8952.
;El dato que es recibido por la UART es re envidado por la misma.
;Se requiere de un cristal de 11.059MHz para el baudrate de 9600bps.
;Utilización de UART en Modo 1 y Timer 1 en Modo 2.   
;Archivo: Untitled1.s03 
;Fecha de realización: 12/06/2017
;Autor: Sebastian Caccavallo - electgpl.blogspot.com.ar
;Modificación:
;-------------------------------------------------------------------------------------
            ORG 0000H               ;Dirección del origen del programa
            JMP INICIO              ;Salta al Inicio del programa
;
            ORG 0023H               ;Dirección de interrupción
            JMP UARTINT             ;Salta a la interrupción
;        
INICIO:     MOV     SCON, #50h      ;UART en MODO 1 (8BIT), REN=1
            ORL     TMOD, #20h      ;TIMER 1 en MODO 2
            MOV     TH1, #0FDh      ;9600bps @ 11.059MHz
            MOV     TL1, #0FDh      ;9600bps @ 11.059MHz
            SETB    ES              ;Habilitación de Interrupción Serial
            SETB    EA              ;Habilitación de Interrupciones Globales
            SETB    TR1             ;Activa TIMER 1                           
            JMP     $               ;Loop cerrado de espera para interrupción   
;
UARTINT:    MOV     DPTR,#TABLA     ;Puntero a tabla de datos   
SIGUE:      CLR     A               ;Limpiamos el acumulador A
            MOVC    A,@A+DPTR       ;Obtiene el Caracter 
            JZ      SALE            ;Si es el último caracter, sale   
            MOV     SBUF,A          ;Cargo el dato en Buffer de salida Serie
EOB:        JNB     TI,EOB          ;Envia hasta el ultimo bit     
            CLR     TI              ;Borra el flag de transmicion
            INC     DPTR            ;Incrementa el indice del puntero
            SJMP    SIGUE           ;Sigue recorriendo la cadena
SALE:       CLR     RI              ;Borra el flag de recepcion 
            RETI                    ;Retorno de Interrupción
; 
TABLA:      DB      "UTN INSPT ",0  ;Tabla de 10 datos a enviar por UART
;
            END                     ;Fin del programa
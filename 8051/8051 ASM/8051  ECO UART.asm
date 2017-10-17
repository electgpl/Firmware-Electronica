;-------------------------------------------------------------------------------------
;Programa que realiza el ECO del puerto serie con 8051 @ 8952.
;El dato que es recibido por la UART es re envidado por la misma.
;Se requiere de un cristal de 11.059MHz para el baudrate de 9600bps.
;Utilización de UART en Modo 1 y Timer 1 en Modo 2.
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
UARTINT:    JNB     RI,DELTI        ;Pregunta si hay Recepción
            CLR     RI              ;Borra el flag de recepción para la siguiente
            MOV     A,SBUF          ;Lee el dato desde la UART
            MOV     SBUF,A          ;Escribe el dato en la UART
            LJMP    ENDINT          ;Fin de subrutina de interrupción
DELTI:      CLR     TI              ;Borra el flag de transmisión para la siguiente
;
ENDINT:     RETI                    ;Retorno de Interrupción
;
            END                     ;Fin del programa
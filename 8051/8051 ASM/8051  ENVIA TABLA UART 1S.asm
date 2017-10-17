;-------------------------------------------------------------------------------------
;Programa que envía una cadena de caracteres por UART cada 1s. con 8051 @ 8952.
;El dato es envidado por la UART cada 1s bloqueante.
;Se requiere de un cristal de 11.059MHz para el baudrate de 9600bps.
;Utilización de UART en Modo 1 y Timer 1 en Modo 2.                      
;Archivo: EnviaUART.s03 
;Fecha de realización: 13/06/2017
;Autor: Sebastian Caccavallo - electgpl.blogspot.com.ar
;Modificación:
;-------------------------------------------------------------------------------------
            ORG     0000H           ;Dirección del origen del programa
            JMP     SETUP           ;Salta al Inicio del programa
;        
SETUP:      MOV     SCON, #50h      ;UART en MODO 1 (8BIT), REN=1
            ORL     TMOD, #20h      ;TIMER 1 en MODO 2
            MOV     TH1, #0FDh      ;9600bps @ 11.059MHz
            MOV     TL1, #0FDh      ;9600bps @ 11.059MHz
            SETB    TR1             ;Activa TIMER 1   
;                        
LOOP:       ACALL   SEND            ;Rutina de envió UART
            SJMP    LOOP            ;Loop principal
;
SEND:       MOV     DPTR,#TABLA     ;Puntero a tabla de datos   
SIGUE:      CLR     A               ;Limpiamos el acumulador A
            MOVC    A,@A+DPTR       ;Obtiene el Caracter 
            JZ      SALE            ;Si es el último caracter, sale   
            MOV     SBUF,A          ;Cargo el dato en Buffer de salida Serie
EOB:        JNB     TI,EOB          ;Envía hasta el ultimo bit     
            CLR     TI              ;Borra el flag de transmisión
            INC     DPTR            ;Incrementa el indice del puntero  
            ACALL   DELAY           ;Rutina de Delay de 1s   
            CPL     P1.0            ;Toggle LED en P1.0 
            SJMP    SIGUE           ;Sigue recorriendo la cadena
SALE:       RET                     ;Retorno de Interrupción
;       
DELAY:      MOV     R7,#6D          ;[2]Cargamos R7 con 6
WAIT1:      MOV     R6,#185D        ;[2]Cargamos R6 con 185   
WAIT2:      MOV     R5,#200D        ;[2]Cargamos R5 con 200 
WAIT3:      DJNZ    R5,WAIT3        ;[2]Decrementamos R5 y Salta a WAIT3
            DJNZ    R6,WAIT2        ;[2]Decrementamos R6 y Salta a WAIT2         
            DJNZ    R7,WAIT1        ;[2]Decrementamos R7 y Salta a WAIT1 
            RET                     ;[2]Retorno de Subrutina  
; 
TABLA:      DB      "UTN INSPT ",0  ;Tabla de 10 datos a enviar por UART
;
            END                     ;Fin del programa
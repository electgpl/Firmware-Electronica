;-------------------------------------------------------------------------------------
;Programa que envía una cadena de caracteres por UART cada 1s. con 8051 @ 8952.
;El dato es envidado por la UART cada 1s bloqueante.
;Se requiere de un cristal de 11.059MHz para el baudrate de 9600bps.
;Utilización de UART en Modo 1 y Timer 1 en Modo 2.                      
;Archivo: EnviaUART30h.s03 
;Fecha de realización: 13/06/2017
;Autor: Sebastian Caccavallo - electgpl.blogspot.com.ar
;Modificación:
;-------------------------------------------------------------------------------------
            ORG     0000H           ;Dirección del Reset      
            JMP     SETUP           ;Salta al Inicio del programa
;            
SETUP:      MOV     SCON, #50h      ;UART en MODO 1 (8BIT), REN=1
            ORL     TMOD, #20h      ;TIMER 1 en MODO 2
            MOV     TH1, #0FDh      ;9600bps @ 11.059MHz
            MOV     TL1, #0FDh      ;9600bps @ 11.059MHz
            SETB    TR1             ;Activa TIMER 1   
;                        
LOOP:       ACALL   LOAD            ;Carga de datos
            ACALL   SEND            ;Rutina de envió UART
            SJMP    LOOP            ;Loop principal     
;
LOAD:       MOV     R0,#0AH         ;Cargo 10 en R0
            MOV     DPTR,#30H       ;Cargo 30h en DPTR (Origen de la tabla)  
            MOV     R1,#41H         ;Cargo el numero 41h en R1
SIGUE1:     MOV     A,R1            ;Cargo en el acumulador el valor de R1
            MOVX    @DPTR,A         ;Cargo el valor apuntado en el acumulador 
            INC     DPTR            ;Incrementamos DPRT 
            INC     R1              ;Incremento R1             
            DJNZ    R0,SIGUE1       ;Itera hasta que se complete 10 veces           
            RET                     ;Retorno de Interrupción 
;       
SEND:       MOV     R0,#0AH         ;Cargo 10 en R0
            MOV     DPTR,#30H       ;Cargo 30h en DPTR (Origen de la tabla)
SIGUE2:     MOVX    A,@DPTR         ;Cargo el valor apuntado en el acumulador 
            MOV     SBUF,A          ;Envio el valor del acumulador por UART 
EOB:        JNB     TI,EOB          ;Envía hasta el ultimo bit     
            CLR     TI              ;Borra el flag de transmisión
            INC     DPTR            ;Incrementamos DPRT              
            ACALL   DELAY           ;Rutina de Delay de 1s   
            CPL     P1.0            ;Toggle LED en P1.0  
            DJNZ    R0,SIGUE2       ;Itera hasta que se complete 10 veces           
            RET                     ;Retorno de Interrupción
;       
DELAY:      MOV     R7,#6D          ;[2]Cargamos R7 con 6
WAIT1:      MOV     R6,#185D        ;[2]Cargamos R6 con 185   
WAIT2:      MOV     R5,#200D        ;[2]Cargamos R5 con 200 
WAIT3:      DJNZ    R5,WAIT3        ;[2]Decrementamos R5 y Salta a WAIT3
            DJNZ    R6,WAIT2        ;[2]Decrementamos R6 y Salta a WAIT2         
            DJNZ    R7,WAIT1        ;[2]Decrementamos R7 y Salta a WAIT1 
            RET                     ;[2]Retorno de Subrutina  
; 
            END                     ;Fin del programa
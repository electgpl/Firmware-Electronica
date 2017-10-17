;-------------------------------------------------------------------------------------
;Programa que muestra periodo entre dos interrupciones externas por flanco, también
;realiza un calculo para sacar una longitud tomando el valor del período elevándolo
;al cuadrado y dividiéndolo por 4.
;Estos valores son mostrados por separado en dos puertos del MCU.
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
;NOTA: TiempoOscilador: fXLS/Div = 12MHz/12=1us
;Valor del delay Tiempo[us]/TiempoOscilador[us]=10ms/1us=10000
;Calculando TLO y THO: 65536-10000=55536 -> D8F0
;
;                Registro TMOD      
; -------TIMER1--------|--------TIMER0--------|
; GATE | C/T | M1 | M0 | GATE | C/T | M1 | M0 |  
;   0     0    0    0      0     0    0    1    
;
; Modo  M1  M0
;   0   0   0   13 bit Timer
;   1   0   1   16 bit Timer
;   2   1   0   8 bit Auto-Reload
;   3   1   1   Split Timer
;                
;Archivo: penduloLongPer.s03
;Fecha de realización: 05/07/2017
;Autor: Sebastian Caccavallo - electgpl.blogspot.com.ar
;Modificación:
;-------------------------------------------------------------------------------------
            ORG     0000H           ;Dirección del origen del programa
            JMP     SETUP           ;Salta al Inicio del programa
;
            ORG     0003H           ;Dirección de interrupción externa INT0
            JMP     EXT_INT0        ;Salta a subrutina de interrupción
;  
FLAG        EQU     50H             ;Reserva de dirección para el FLAG.
;
SETUP:      MOV     TMOD,#01H       ;Configuramos TMOD con 1 (TMR0 - Modo 0) 16bits
            MOV     TCON,#01H       ;Configura IT0 externa por flanco
            MOV     IE,#01H         ;Configura EX0 Interrupción Externa 0
            MOV     IP,#00H         ;Des-habilita prioridad de interrupt
            SETB    EA              ;Habilita interrupción global
;
LOOP:       CLR     FLAG            ;Ponemos a 0 el FLAG
ESPERA1:    JNB     FLAG,ESPERA1    ;Espera a que el FLAG sea 1
            CALL    TICKTMR         ;Recogemos el valor del contador
            MOV     R0,TH0          ;Guardamos solo la parte Alta
            CLR     FLAG            ;Ponemos a 0 el FLAG
ESPERA2:    JNB     FLAG,ESPERA2    ;Espera a que el FLAG sea 1
            CALL    TICKTMR         ;Recogemos el valor del contador
            MOV     R0,TH0          ;Guardamos solo la parte Alta
            CALL    CALC            ;Procesa el calculo
            MOV     P0,R1           ;Muestra el valor de Longitud en el puerto P0
            MOV     P1,R0           ;Muestra el valor de Período en el puerto P1
            JMP     LOOP            ;Vuelve a iniciar el Loop de forma indeterminada
;
TICKTMR:    MOV     TL0,#0F0H       ;Configuramos TL0 con F0h
            MOV     TH0,#0D8H       ;Configuramos TH0 con D8h  
            SETB    TR0             ;Start Timer
ESPERA:     JNB     TF0,ESPERA      ;Espera al flag TF0=1 (espera que se desborde)
            CLR     TR0             ;Stop Timer
            CLR     TF0             ;Borra flag TF0                  
            RET                     ;Retorno de Subrutina
;
CALC:       MOV     A,R0            ;Guardamos valor Leído en el acumulador               
            MOV     B,A             ;Se asigna el valor A en B para hacer el cuadrado.
            MUL     AB              ;Se realiza el cuadrado y se guarda en A.
            MOV     B,#04D          ;Cargamos B con 4 como divisor
            DIV     AB              ;Se realiza la división (A*B)/4
            MOV     R1,A            ;Se guarda el resultado en R1 (Valor del Longitud)
            RET                     ;Retorno de Subrutina
;
EXT_INT0:   SETB    FLAG            ;Ponemos a 1 el Flag de entrada de INT
            RETI                    ;Retorno de Interrupción
;
            END                     ;Fin del programa
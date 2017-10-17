;-------------------------------------------------------------------------------------
;Programa que genera un delay de 100us con el uso del Timer 0. 
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
;Archivo: toggle10ms.s03 
;Fecha de realización: 19/06/2017
;Autor: Sebastian Caccavallo - electgpl.blogspot.com.ar
;Modificación:
;-------------------------------------------------------------------------------------
            ORG     0000H           ;Dirección del origen del programa
            JMP     SETUP           ;Salta al Inicio del programa
;        
SETUP:      MOV     TMOD,#01H       ;Configuramos TMOD con 1 (TMR0 - Modo 0) 16bits
;    
LOOP:       CPL     P1.0            ;Toggle pin P1.0
            CALL    DELAY           ;Llama a la subrutina de delay de 10ms   
            SJMP    LOOP            ;Realiza el loop infinito 
;    
DELAY:      MOV     TL0,#0F0H       ;Configuramos TL0 con F0h
            MOV     TH0,#0D8H       ;Configuramos TH0 con D8h
            SETB    TR0             ;Start Timer
ESPERA:     JNB     TF0,ESPERA      ;Espera al flag TF0=1 (espera que se desborde)
            CLR     TR0             ;Stop Timer
            CLR     TF0             ;Borra flag TF0
            RET                     ;Retorno de subrutina
;
            END                     ;Fin del programa
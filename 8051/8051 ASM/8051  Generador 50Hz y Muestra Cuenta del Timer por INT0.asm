;-------------------------------------------------------------------------------------
;Programa que realiza el toggle de una salida a 50Hz, si entra una interrupción 
;externa se mostrara el valor alto y bajo de la cuenta del timer en ese momento
;en los puertos P0 y P1.    
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
;Archivo: gen50HzEXTINT0.s03 
;Fecha de realización: 19/06/2017
;Autor: Sebastian Caccavallo - electgpl.blogspot.com.ar
;Modificación:
;-------------------------------------------------------------------------------------
            ORG     0000H       ;Dirección del origen del programa
            JMP     SETUP       ;Salta al Inicio del programa
;            
            ORG     0003H       ;Dirección de interrupción externa INT0
            JMP     EXT_INT0    ;Salta a subrutina de interrupción 
;            
SETUP:      MOV     TMOD,#01H   ;Configuramos TMOD con 1 (TMR0 - Modo 0) 16bits
            MOV     TCON,#01H   ;Configura IT0 externa por flanco
            MOV     IE,#01H     ;Configura EX0 Interrupción Externa 0
            MOV     IP,#00H     ;Des-habilita prioridad de interrupt
            SETB    EA          ;Habilita interrupción global          
;            
LOOP:       MOV     TL0,#0F0H   ;Configuramos TL0 con F0h
            MOV     TH0,#0D8H   ;Configuramos TH0 con D8h   
            SETB    TR0         ;Start Timer
ESPERA:     JNB     TF0,ESPERA  ;Espera al flag TF0=1 (espera que se desborde)
            CLR     TR0         ;Stop Timer
            CLR     TF0         ;Borra flag TF0                   
            CPL     P2.2        ;Toggle en la salida P2.2 
            JMP     LOOP        ;Vuelve a iniciar el Loop de forma indeterminada

EXT_INT0:   MOV     P0,TL0      ;Muestra el valor actual de la cuenta TL0 en P0
            MOV     P1,TH0      ;Muestra el valor actual de la cuenta TH0 en P1
            RETI                ;Retorno de interrupción
;            
            END                 ;Fin del programa
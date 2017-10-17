;-------------------------------------------------------------------------------------
;Programa que busca en una tabla de 200 registros, el valor máximo y el mínimo
;una vez encontrados, guarda el valor con su dirección en la tabla.             
;Archivo: calcMaxMin.s03
;Fecha de realización: 19/06/2017
;Autor: Sebastian Caccavallo - electgpl.blogspot.com.ar
;Modificación:
;-------------------------------------------------------------------------------------    
            ORG     0000H           ;Dirección del origen del programa
            JMP     SETUP           ;Salta al Inicio del programa    
;            
CONT        EQU     50H             ;Reserva de dirección para el contador
VALMAX      EQU     51H             ;Reserva de dirección para el valor máximo
VALMIN      EQU     52H             ;Reserva de dirección para el valor mínimo
DIRMAX      EQU     53H             ;Reserva de dirección para la dirección máxima
DIRMIN      EQU     54H             ;Reserva de dirección para la dirección mínima
;                                   
SETUP:      MOV     VALMAX,#0D      ;Variable de Valor Máximo inicial
            MOV     VALMIN,#255D    ;Variable de Valor Mínimo inicial
            MOV     DIRMAX,#0D      ;Variable donde se guardara la dirección
            MOV     DIRMIN,#0D      ;Variable donde se guardara la dirección
; 
LOOP:       CALL    CARGA           ;Carga contador y puntero
            CALL    BUSCMAX         ;Llama a la subrutina para buscar el máximo
            CALL    CARGA           ;Carga contador y puntero
            CALL    BUSCMIN         ;Llama a la subrutina para buscar el mínimo 
            SJMP    LOOP            ;Realiza el loop infinito   
;  
CARGA:      MOV     R0,#60H         ;Cargamos el puntero de origen para la tabla   
            MOV     CONT,#20D       ;Cargamos el contador con la cantidad de iteraciones
            RET                     ;Retorno de la subrutina 
;           
BUSCMAX:    MOV     A,@R0           ;Cargamos el Acumulador con el valor apuntado por R0
            SUBB    A,VALMAX        ;Calculamos A - VALMAX
            JC      CARRY0          ;Si JC=1 (es menor), salta a Carry 
            MOV     VALMAX,@R0      ;Si JC=0 (es mayor), lo guarda en VALMAX
            MOV     DIRMAX,R0       ;Al mismo tiempo guarda la dirección en DIRMAX
CARRY0:     INC     R0              ;Incrementa valor del puntero en la tabla
            DJNZ    CONT,BUSCMAX    ;Realiza las iteraciones preestablecidas
            RET                     ;Retorno de la subrutina  
;
BUSCMIN:    MOV     A,@R0           ;Cargamos el Acumulador con el valor apuntado por R0
            SUBB    A,VALMIN        ;Calculamos A - VALMIN
            JNC     CARRY1          ;Si JC=0 (es mayor), salta a Carry 
            MOV     VALMIN,@R0      ;Si JC=1 (es menor), lo guarda en VALMIN
            MOV     DIRMIN,R0       ;Al mismo tiempo guarda la dirección en DIRMIN
CARRY1:     INC     R0              ;Incrementa valor del puntero en la tabla
            DJNZ    CONT,BUSCMIN    ;Realiza las iteraciones preestablecidas
            RET                     ;Retorno de la subrutina
;
            END                     ;Fin del programa
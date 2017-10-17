        LIST            P=16F84A
        INCLUDE         <P16F84A.INC>
        __CONFIG        _CP_OFF &  _WDT_OFF & _PWRTE_ON & _XT_OSC
;
        CBLOCK  0x0C                    ;Inicio del bloque de RAM
        Contador                        ;Variable Contador    
		Limite							;Variable Limite
        ENDC                            ;Fin del bloque de RAM
;
#DEFINE Pulsador PORTB,0                ;Se redefine RB0 como Pulsador
;
        org     0                       ;Inicio de la memoria
        goto    Inicio                  ;Salta a Inicio
        org     4                       ;Vector de interrupcion
        goto    Interrupcion            ;Salta a Interrupcion
Inicio  movlw	b'00011111'				;Cargo W con 32
		movwf	Limite					;Cargo la variable Limite con 32
		bsf     STATUS,RP0              ;Acceso al Banco 1
        bsf     Pulsador                ;RB0/INT se configura como entrada.
        bsf     OPTION_REG,INTEDG       ;Interrupcion INT se activa por flanco de subida
        bcf     STATUS,RP0              ;Acceso al Banco 0
        clrf    Contador                ;Inicializa el contador
        movwf   PORTA                   ;Mueve el valor del contador al puerto A
        movlw   b'10010000'             ;Habilita la interrupcion INT
        movwf   INTCON                  ;Habilita la interrupcion General
;
Main    sleep                           ;Pasa a modo de bajo consumo
        goto    Principal               ;Salta a Main
;
Interrupcion
        btfss   Pulsador                ;Comprueba si se preciona el pulsador
        goto    FinInterrupcion         ;Si no esta presionado salta a FinInterrupcion
        incf    Contador,F              ;Si esta presionado incrementa el contador
		sublw 	Limite					;Resta Limite a W (El contador)
		btfsc 	STATUS,Z				;Si la resta es 0 llego a 32 y salta una posicion
		goto    FinInterrupcion         ;Si no llego a 32, sigue a FinInterrupcion
        clrf	Contador				;Reinicia a 0 el contador
;
FinInterrupcion
        bcf     INTCON,INTF             ;Limpia flag de reconocimiento (INTF)
        retfie                          ;Retorna y rehabilita las interrupciones (GIE=1)
;
        end
main	movf	PORTA,W			;Lee Puerto A desde RA0 a RA2
        andlw	b'00000111'		;Aplica la mascara de los 3 bit LSB
		call	Tabla			;Llama a la tabla y trae el valor correspondiente a dirección 2
		movwf	PORTB			;Mueve el valor que la tabla tiene en dirección 2
		goto 	main			;Repite el ciclo
Tabla	addwf	PCL,F			;Suma el valor de W (dijimos 2) al contador PCL y salta a esa dirección
		retlw 	b'00001001'		;Retorna el valor de la tabla 00001001
		retlw 	b'00001100'		;Retorna el valor de la tabla 00001100
		retlw 	b'00000110'		;Retorna el valor de la tabla 00000110
		retlw 	b'00000110'		;Retorna el valor de la tabla 00000110
		retlw 	b'00001100'  	;Retorna el valor de la tabla 00001100
		retlw 	b'00001001'		;Retorna el valor de la tabla 00001001
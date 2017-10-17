	CLR	$00A1	;Borrado de la dirección 00A1
	LDAA	$A104	;Carga acumulador A con A104
	ASLA		;Desplaza a la izquierda A con ceros
	BCS	carry: 	;Si Carry=1 salta a Carry:
	ASLA		;Desplaza a la izquierda A con ceros
	BCS	carry: 	;Si Carry=1 salta a Carry:
	STAA	$A0	;Guarda en la dirección A0 el valor de A
	SWI		;Interrupción de Software
carry: 	COM	$00A1	;Complemento a 1 de 00A1
	CLR	$00A0	;Borrado de 00A0
	SWI		;Interrupción de Software

EC00	7F	-
EC01	00	- CLR	$00A1
EC02	A1	-
EC03	B6	|
EC04	A1	| LDAA	$A104
EC05	04	|
EC06	48	- ASLA
EC07	25	|
EC08	05	| BCS	carry:
EC09	48	- ASLA
EC0A	25	|
EC0B	03	| BCS	carry:
EC0C	97	-
EC0D	A0	- STAA	$A0
EC0E	3F	| SWI
EC0F	73	-
EC10	00	- COM	$00A1
EC11	A1	-	
EC12	7F	|
EC13	00	| CLR	$00A0
EC14	A0	|
EC15	3F	- SWI
....
FFFE	EC	|PCH
FFFF	00	-PCL


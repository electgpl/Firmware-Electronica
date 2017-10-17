	LDAA	$F0	;Carga acumulador A con F0
	ANDA	$A1FE	;And entre A y A1FE
	STAA	$A200	;Guarda contenido de A en A200
	LDAA	$F0	;Carga acumulador A con F0
	ANDA	$A1FF	;And entre A y A1FF
	ORAA	$A200	;Or entre A y A200
	STAA	$A200	;Carga acumulador A con A200
	SWI		;Interrupción de Software		

EC00	B6	|
EC01	F0	| LDAA	$07
EC02	B4	-
EC03	A1	- ANDA	$A1FE
EC04	FE	-
EC05	B7	|
EC06	A2	| STAA	$A200
EC07	00	|
EC08	B6	-
EC09	F0	- LDAA	$07
EC0A	B4	|
EC0B	A1	| ANDA	$A1FF
EC0C	FF	|
EC0D	BA	- 
EC0E	A2 	- ORAA	$A200
EC0F	00	- 
EC10	B7	|
EC11	A2	| STAA	$A200
EC12	00	|	
EC13	3F	- SWI
....
FFFE	EC	|PCH
FFFF	00	-PCL


	LDAA	$07	;Carga acumulador A con 07
	CLR	$A201	;Borrado de A201
Rotar:	ROL	$A200	;Rota a la izquiera A200
	BCS	Comp1: 	;Si Carry=1 salta a Comp1:
Dec:	DECA		;Decrementa A
	BNE	Rotar:	;Si Z=0 salta Rotar:
	SWI		;Interrupción de Software
Comp1:	COM	$A201	;Complemento a 1 de A201
	BRA	Dec:	;Salta a Dec:

EC00	B6	|
EC01	07	| LDAA	$07
EC02	7F	-
EC03	A2	- CLR	$A201
EC04	01	-
EC05	69	|
EC06	A2	| ROL	$A200
EC07	00	|
EC08	25	- 
EC09	04	- BCS	Comp1:
EC0A	4A	| DECA
EC0B	26	- 
EC0C	F8 	- BNE	Rotar:
EC0D	3F	| SWI
EC0E	73	-
EC0F	A2	- COM	$A201
EC10	01	-	
EC11	20	| 
EC12	F7	| BRA	Dec:
....
FFFE	EC	|PCH
FFFF	00	-PCL


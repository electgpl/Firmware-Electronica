delay			;[8]Llamado a rutina Delay con JRS
	LDAA	$E7 	;[3]Carga acumulador A con 231 
	LDAB	$FF 	;[3]Carga acumulador B con 255
LoopA:	DECA 		;[2]Decrementa A 
	BEQ	Fin: 	;[4]Si Z=1 finaliza demora 
LoopB:	DECB 		;[2]Decrementa B 
	COM 	$0000	;[6]Complementa 
	COM 	$0000 	;[6]Complementa 
	COM 	$0000	;[6]Complementa  
	COM 	$0000	;[6]Complementa  
	BEQ	LoopA: 	;[4]Si Z=1 salta a LoopA: 
	BRA	LoopB: 	;[4]Si Z=0 salta a LoopB: 
Fin:	SWI 		;[12]Interrupción de Software

(((34*255)+6)*231)+26=2004182us

EC00	96	| LDAA
EC01	E7	| $E7
EC02	D6	- LDAB
EC03	FF	- $FF
EC04	4A	| DECA
EC05	27	- BEQ
EC06	11  	- Salta a EC10
EC07	5A	| DECB
EC08	73	- COM
EC09	00	-
EC0A	00	-
EC0B	73	| COM
EC0C	00	|
EC0D	00	|
EC0E	73	- COM
EC0F	00	-
EC10	00	-
EC11	73	| COM
EC12	00	|
EC13	00	|
EC14	27	- BEQ
EC15	EE  	- Salta a EC04
EC16	20  	| BRA
EC17	EF	| Salta a EC07
EC18	3F	- SWI
....
FFFE	EC	|PCH
FFFF	00	-PCL



15	00010101
ca1	11101010
+
04	00000100

	11101110  EE

17	00010111
ca1	11101000
+
07	00000111

	11101111  EF
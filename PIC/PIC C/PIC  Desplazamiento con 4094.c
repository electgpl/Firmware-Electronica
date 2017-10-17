#define LARGO 65//Longitud de la tabla
#define DATA pin_a0
#define CLOCK pin_a1
#define STROBE pin_a2
void escribir_4094(int16 caracter);
signed int16 bitMask[16]={0b0111111111111111,
                          0b1011111111111111,
                          0b1101111111111111,
                          0b1110111111111111,
                          0b1111011111111111,
                          0b1111101111111111,
                          0b1111110111111111,
                          0b1111111011111111,
                          0b1111111101111111,
                          0b1111111110111111,
                          0b1111111111011111,
                          0b1111111111101111,
                          0b1111111111110111,
                          0b1111111111111011,
                          0b1111111111111101,
                          0b1111111111111110};
int const tabla[LARGO+16]={0b00000000, //Espacio
                           0b00000000, //Espacio
                           0b00000000, //Espacio
                           0b00000000, //Espacio
                           0b00000000, //Espacio
                           0b00000000, //Espacio
                           0b00000000, //Espacio
                           0b00000000, //Espacio
                           0b00000000, //Espacio
                           0b00000000, //Espacio
                           0b00000000, //Espacio
                           0b00000000, //Espacio
                           0b00000000, //Espacio
                           0b00000000, //Espacio
                           0b00000000, //Espacio
                           0b00000000, //Espacio
                           0b00000000, //Espacio
                           0b00000000, //Espacio
                           0b00000000, //Espacio
                           0b00000000, //Espacio
                           0b10000001, //Dato de tabla I
                           0b10000001,
                           0b11111111,
                           0b10000001,
                           0b10000001,
                           0b00000000, //Espacio
                           0b11111111, //Dato de tabla P
                           0b00010001,
                           0b00010001,
                           0b00010001, 
                           0b00001110, 
                           0b00000000, //Espacio
                           0b11111111, //Dato de tabla E
                           0b10001001, 
                           0b10001001,
                           0b10001001,
                           0b10000001
                           0b00000000, //Espacio
                           0b00000001, //Dato de tabla T
                           0b00000001, 
                           0b11111111, 
                           0b00000001,
                           0b00000001,
                           0b00000000, //Espacio
                           0b00000000, //Espacio
                           0b00000000, //Espacio
                           0b11100001, //Dato de Tabla 2
                           0b10010001, 
                           0b10010001,
                           0b10010001,
                           0b10001110,
                           0b00000000, //Espacio
                           0b10001111, //Dato de tabla 5
                           0b10001001, 
                           0b10001001, 
                           0b10001001,
                           0b01110001,
                           0b00000000, //Espacio
                           0b01111110, //Dato de Tabla 0
                           0b10010001, 
                           0b10001001,
                           0b10000101,
                           0b01111110,
                           0b00000000, //Espacio
                           0b00000000, //Espacio
                           0b00000000, //Espacio
                           0b00000000, //Espacio
                           0b00000000, //Espacio
};
void main(){
   set_tris_b (0x00);
   set_tris_a(0x00);
   int i,j,k; //Variables contadores
   while(TRUE){ //Ciclo infinito
      for(i=0;i<LARGO;i++){ //Recorrido de tabla
         for(k=0;k<30;k++){ //Refresco de matriz
            for(j=0;j<16;j++){//Barrido de Columnas
               escribir_4094(j); //Columnas ponderadas
               output_b(tabla[j+i]); //Desplazamiento de tabla
               delay_us(1); //Demora de multiplexado
            } 
         }
      }
   } 
} 
void escribir_4094(int16 caracter){ //Subrutina que carga los Datos en el CD4094
   int contador_8;
   output_low(DATA);
   output_low(STROBE);
   output_low(CLOCK);
   for(contador_8=0;contador_8<16;contador_8++){
      output_bit(DATA,bit_test(bitMask[caracter],contador_8)); 
      output_high(CLOCK);
      delay_cycles(1);
      output_low(CLOCK);
   }
   output_high(STROBE);
   delay_cycles(1);
   output_low(STROBE);
}
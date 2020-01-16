#include<stdio.h>
#include<stdlib.h>
#define CANTIDAD_PART 13

char participantes[CANTIDAD_PART][30] = {"Let001ar","Damián chaile","Matias CHIAIA","EmIs","Agustin Varela ",
                                         "Victor Claudio Juarez","Manuel Bonino","Guido Rodriguez",
                                         "Fernando (ferl87)","Hugo Martínez","Migue107","Pablitox","Nicolas Rasitt"};

int main(){
   printf("\n\n>>> P A R T I C I P A N T E S <<<\n\n");
   for(int i = 0; i < CANTIDAD_PART; i++){
	  printf("%s \n", participantes[i]);
   }
   printf("\n\n>>> G A N A D O R <<<\n\n");
   srand (time(NULL));
   int ganador = rand()%CANTIDAD_PART;
   printf(">>> %s <<<\n", participantes[ganador]);
   return 0;
}

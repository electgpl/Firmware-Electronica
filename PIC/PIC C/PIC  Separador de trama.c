#include <stdio.h>
#include <string.h>
char linea[]="string1 29873219487 19824723 4980192 3123";
char separador[]=" ";
char *trama=NULL;
main(){
    trama=strtok(linea,separador);
    while(trama!=NULL){
        printf("%s \n",trama);
        trama=strtok(NULL,separador);
    }
}
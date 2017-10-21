/*          LISTA06.C

Manejo de listas, tiene la funcion que inserta un dato en una lista ordenada
por precio. Debe tener en cuenta las distintas posibilidades:

1) Lista Vacia
2) Inserta antes del primero
3) Inserta luego del ultimo
4) Inserta al medio de la lista

Tiene funciones para ordenar segun los datos

*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_CHAR 30

typedef struct {
    int cod;
    float pre;
    char *descr;
} DATOS;

typedef struct _NODO {
    DATOS *pd;
    struct _NODO *proximo;
} NODO;

typedef struct {
    NODO *ultimo;
    NODO *primero;
} LISTA;

//-----------------------------------------------------

#define NIL (NODO *)0

void iniciar_cola(LISTA *pt) {
    pt -> primero = NIL;
    pt -> ultimo = NIL;
    return;
}

void poner_ultimo(LISTA *cola, int codi, float preci, char *s) {
    NODO *aux;
    char *auxstr;

    if((aux = (NODO *)malloc(sizeof(NODO))) == NIL) {
        printf("Sin memoria para NODO");
        exit(1);
    }

    if((aux -> pd = (DATOS *)malloc(sizeof(DATOS))) == NULL) {
        printf("Sin memoria para DATOS");
        exit(1);
    }

    if((auxstr = (char *)malloc(strlen(s))) == NULL) {
        printf("Sin memoria para Strings");
        exit(1);
    }

    aux -> pd -> cod = codi;
    aux -> pd -> pre = preci;
    strcpy(auxstr,s);
    aux -> pd -> descr = auxstr;

    if(cola -> primero == NIL)
        cola -> primero = aux;
    else
        cola -> ultimo -> proximo = aux;

    cola -> ultimo = aux;
    aux -> proximo = NIL;
    return;
}

NODO *sacar_primero(LISTA *cola) {
    NODO *msg;
    
    if(cola -> primero == NIL)
        return(NIL);
    
    msg = cola -> primero;
    cola -> primero = msg -> proximo;

    if(cola -> ultimo == msg)
        cola -> ultimo = NIL;

    return(msg);
}

void listar_cola(LISTA *cola) {
    int n = 0;
    NODO *l;

    for(l = cola -> primero ; l != NIL ; l = l -> proximo) {
        printf("%-15s : %d      %4d     %6.2f \n", l -> pd -> descr, n, l -> pd -> cod, l -> pd -> pre );
        n++;
    }

    return;
}

void ord_list_pre(LISTA *cola) {
    NODO *j, *i;
    DATOS *temp;
    
    for(j = cola -> primero ; j != NIL ; j -> proximo)
        for(i = j -> proximo ; i != NIL ; i = i -> proximo)
            if(j -> pd -> pre > i -> pd -> pre){
                temp = j -> pd;
                j -> pd = i -> pd;
                i -> pd = temp;
            }
    
    return;
}

void ord_list_cod(LISTA *cola) {
    NODO *j, *i;
    DATOS *temp;
    for(j = cola -> primero ; j != NIL ; j = j -> proximo)
        for(i = j -> proximo ; i != NIL ; i = i -> proximo)
            if(j -> pd -> cod > i -> pd -> cod) {
                temp = j -> pd;
                j -> pd = i -> pd;
                i -> pd = temp;
            }

    return;
}

void ord_list_descr(LISTA *cola) {
    NODO *j, *i;
    DATOS *temp;
    for(j = cola -> primero ; j != NIL ; j = j -> proximo)
        for(i = j -> proximo ; i != NIL ; i = i -> proximo)
            if(strcmp(j -> pd -> descr , i -> pd -> descr) > 0) {
                temp = j -> pd;
                j -> pd = i -> pd;
                i -> pd = temp;
            }

    return;
}

void insertar_precio(LISTA *cola, int co, float pr, char *sn) {
    NODO *j, *jant;
    NODO *aux;
    char *auxstr;

    if((aux = (NODO *)malloc(sizeof(NODO))) == NIL) {
        printf("Sin memoria para NODO");
        exit(1);
    }

    if((aux -> pd = (DATOS *)malloc(sizeof(DATOS))) == NULL) {
        printf("Sin memoria para DATOS");
        exit(1);
    }

    if((auxstr = (char *)malloc(strlen(sn)+2)) == NULL) {
        printf("Sin memoria para Strings");
        exit(1);
    }

    aux -> pd -> cod = co;
    aux -> pd -> pre = pr;
    strcpy(auxstr,sn);
    aux -> pd -> descr = auxstr;

    if(j == NIL) {
        cola -> primero = cola -> ultimo = aux;
        aux -> proximo = NIL;
    } 
    else if( pr < j -> pd -> pre) {
             aux -> proximo = j;
             cola -> primero = aux;
         }
         else if( pr > cola -> ultimo -> pd -> pre) {
                  cola -> ultimo -> proximo = aux;
                  aux -> proximo = NIL;
                  cola -> ultimo = aux;
              }
              else {
                  while (j -> pd -> pre < pr) {
                      jant = j;
                      j = j -> proximo;
                  }
                  aux -> proximo = jant -> proximo;
                  jant -> proximo = aux;
              }

    return;
}

void main(void) {
    int codigo[] = {102, 103, 133, 124, 152, 127, 100, 0};
    float precio[] = {10.3, 12.4, 4.02, 5.7, 2.3, 6.2, 12.5, 0};
    char *descrip[] = {"Mecha", "Calibre", "Guias", "M01", "G7", "Sep", "Motor", "*"};

    int i;
    NODO *ptr;
    NODO *sacar_primero();
    LISTA *cola1;

    clrscr();

    if((cola1 = (LISTA *)malloc(sizeof(LISTA))) == NULL) {
        printf("Sin memoria para Header");
        exit(1);
    }

    iniciar_cola(cola1);

    for(i = 0 ; codigo[i] != 0 ; i++) {
        poner_ultimo(cola1, codigo[i], precio[i], descrip[i]);
    }

    printf("Vemos la lista... \n\n");
    listar_cola(cola1);
    ord_list_pre(cola1);
    printf("\nLista ordenada por precio...: \n\n");
    listar_cola(cola1);
    ord_list_cod(cola1);
    printf("\nLista ordenada por codigo...: \n\n");
    listar_cola(cola1);
    ord_list_descr(cola1);
    printf("\nLista ordenada por descripcion...: \n\n");
    listar_cola(cola1);
    printf("\nSacamos el primero y agregamos ordenando por precio: \n\n");
    sacar_primero(cola1);
    insertar_precio(cola1, 323, 7.55, "Nuevo1");
    insertar_precio(cola1, 444, 1.55, "Nuevo2");
    insertar_precio(cola1, 883, 27.55, "Nuevo3");
    listar_cola(cola1);

    exit(0);
    return;
}
# Listas con enlace simple :+1:

Una lista con enlace simple o lista dinámica es una estructura de datos que reserva espacio de memoria a medida que el programa se ejecuta y cuyos nodos se enlazan mediante un único puntero que apunta al siguiente nodo.

## Concepto de Nodo.

Para comenzar a trabajar con una estructura dinámica de datos es necesario declarar una estructura con los datos que vamos a tratar y un puntero para poder apuntar al siguiente conjunto de datos. Esto conforma un único elemento de la lista al que llamamos nodo.

Un nodo de una lista dinámica lo declararemos como una estructura de datos con el siguiente esquema:

```c
struct nodo {
   tipo_dato1 identificador1;
   tipo_dato2 identificador2;
    ...
   struct nodo *siguiente_elemento;
}
```

¿Qué significa esto? Pues que un nodo con ésta estructura contendrá una serie de datos con sus correspondientes tipos y además contendrá un puntero que apuntará a la dirección de memoria donde se encuentra el siguiente nodo con la misma estructura. De ésta forma no hay límite en la cantidad de información a almacenar, cosa que si pasa con los arrays.

Además es importante tener en cuenta en la declaración que debemos declarar ciertos elementos adicionales para poder guardar información necesaria para trabajar con la lista, como por ejemplo el puntero al primer nodo, éste es muy importante debido a que si no almacenamos su valor no podremos localizar el inicio de la lista y por lo tanto la perderíamos.

La declaración de la estructura quedaría así:

```c
typedef struct _nodo {
   int dato;
   struct _nodo *siguiente;
} tipoNodo;
```

Un dato importante es que al declarar el primer nodo de una lista hay que inicializarlo a null para especificar que comenzamos una lista vacía.

La declaración completa de una lista de una estructura de enteros quedaría así:

```c
struct Nodo {
   int dato1;
   int dato2;
   Nodo *siguiente;
};

int main (void) {
   struct Nodo *lista=NULL;
}
```

Con esto tenemos declarada la estructura y el inicio de una lista, ahora vamos a ver las operaciones más frecuentes:

## Comienzo de una lista.

Lo primero que debemos hacer para comenzar una lista y asignar los primeros valores es reservar un espacio de memoria, para ello utilizaremos la librería stdlib.h y su función malloc:

```c
struct Nodo {
   int dato1;
   int dato2;
   Nodo *siguiente;
};

int main (void) {
   struct Nodo *lista=NULL;
   lista = malloc( sizeof(struct Nodo) );
}
```

Con esto ya hemos iniciado la lista, ahora vamos a hacer que las variables enteras tengan datos y que el puntero apunte a null (ya que aún no tenemos ningún nodo que continúe).

```c
struct Nodo {
   int dato1;
   int dato2;
   Nodo *siguiente;
};

int main (void) {
   struct Nodo *lista=NULL;
   lista = malloc( sizeof(struct Nodo) );
   lista -> dato1=10;
   lista -> dato2=25;
   lista -> siguiente =NULL;
}
```

Ya tenemos el nodo relleno de datos, ahora podremos hacer cualquier operación con ellos, como si de variables se tratase. Fíjate que para acceder al dato he utilizado el operador ->, esto es debido a que lista es del tipo de dato struct Nodo, lo cual quiere decir que es un puntero, gracias al operador -> podemos acceder al campo que deseemos directamente.

## Añadir un nodo a la cabeza.

Ahora vamos a ver como añadir un nuevo nodo al principio de la lista. El planteamiento no es complicado, debemos crear un nuevo nodo, rellenar sus datos y hacer que el puntero *siguiente apunte al nodo cuyos datos son dato1 = 10 y dato2=25, es decir, el nodo único que tenemos originalmente.

Existe un gran problema a la hora de trabajar con listas dinámicas de datos, el problema no es otro que perder la referencia del siguiente nodo de la lista, bien porque modifiquemos la información del puntero *siguiente o bien porque lo vaciemos. En cualquier caso es importante tener en cuenta la siguiente: Si se pierde la referencia al siguiente nodo de la lista, es imposible volver a recuperarla, por lo que perderemos todos los elementos de la lista a partir de ese nodo.

Para evitar esto usaremos un puntero auxiliar, para no perder la referencia mientras añadimos el nodo nuevo.

El programa quedará así:

```c
struct Nodo {
   int dato1;
   int dato2;
   Nodo *siguiente;
};

int main (void) {
   struct Nodo *lista=NULL, * aux;
   lista = malloc( sizeof(struct Nodo) );
   lista -> dato1=10;
   lista -> dato2=25;
   lista -> siguiente =NULL;

   aux = lista ;

   lista = malloc( sizeof(struct Nodo) );
   lista -> dato1=3;
   lista -> dato2=5;
   lista-> siguiente = aux ;
}
```

¿Con esto qué hemos hecho? Primero hemos declarado nuestro nodo inicial, le hemos dado valores, después utilizamos el puntero *aux para apuntar a este nodo y así no perder la referencia. Creamos el segundo nodo y rellenamos sus datos, después hacemos que el puntero *siguiente tenga el mismo valor que el puntero *aux, con lo cual gracias a esto el puntero *siguiente nuevo contiene la misma dirección de memoria que el puntero *aux, que es la de nuestro nodo original, quedando así los dos nodos de la lista perfectamente enlazados.

¿Qué hacemos con *aux ahora que ya no nos hace falta y sigue apuntando al segundo nodo? Podemos dejarlo tal y como está, siempre nos hará falta un puntero auxiliar para trabajar con listas dinámicas, tarde o temprano cambiará de valor.

El puntero *aux es el puntero más importante que usaremos, puesto que gracias a él podremos trabajar con las listas sin perder las referencias.

Éste código podemos guardárnoslo perfectamente para posteriores usos en forma de un procedimiento o una función perteneciente a un programa o una librería propia.

## Añadir un nodo al final.

El caso que nos ocupa ahora es también sencillo si entendemos correctamente la teoría de listas dinámicas.

Para añadir un nodo al final de la lista basta con encontrar el nodo cuyo puntero *siguiente apunte a NULL. Realizar esto de forma manual mediante una sucesión de -> siguiente -> siguiente es completamente innecesario e ineficiente. Para recorrer una lista lo mejor es usar un bucle while de la forma:

while (aux->siguiente != NULL)

Una vez encontremos el nodo cuyo puntero *siguiente apunta a NULL procedemos a añadir el nuevo nodo. Necesitaremos de un nuevo nodo aparte del *aux para poder conservar los datos (o rellenarlos a posteriori, esto es una buena práctica).

El código para añadir un nodo al final de una lista es el siguiente:

```c
struct Nodo {
   int dato1;
   int dato2;
   Nodo *siguiente;
};

int main (void) {
   struct Nodo *lista=NULL, * aux, *nuevo;
   lista = malloc( sizeof(struct Nodo) );
   lista -> dato1=10;
   lista -> dato2=25;
   lista -> siguiente =NULL;

   aux = lista ;

   while (aux->siguiente != NULL) {
      aux = aux-> siguiente;
   }

   nuevo = malloc( sizeof(struct Nodo) );
   nuevo-> dato1=2;
   nuevo-> dato2=6;
   nuevo-> siguiente = NULL;
   aux-> siguiente = nuevo ;
}
```

Mediante el bucle while y usando el puntero *aux vamos avanzando en la lista, el puntero *aux va apuntando a donde apunte *siguiente hasta que *siguiente sea NULL, de ésta forma *aux está posicionado en el último nodo y está listo para apuntar al nuevo nodo, que se quedaría en el final.

Aquí se puede comprobar la importancia del uso del puntero *aux, si en vez usar el puntero *aux, usáramos *lista, con cada vuelta del bucle while estaríamos perdiendo la referencia al nodo anterior, por lo que no podríamos referenciar el inicio de la lista y la perderíamos.

Nuevamente éste código se puede guardar para su uso posterior.

## Borrar el primer nodo de una lista.

Borrar el primer nodo de una lista no debería ser complicado ya que según hemos podido ver, en cuanto perdemos la referencia de un nodo, éste es irrecuperable. Con lo cual si quisiéramos borrar el primer nodo de una lista nada más fácil que hacer lo siguiente:

lista=lista -> siguiente;

Esto teóricamente hace lo que deseamos, ya que al hacer que el puntero *lista apunte al nodo siguiente en vez de al primer nodo perdemos la referencia y nos hemos librado de ese nodo. No obstante presenta un problema, el primer nodo sigue existiendo en la memoria, lo cual no debería suceder ya que está consumiendo recursos de la máquina.

Para evitar esto usaremos la función free incluida también en la librería stdlib.h:

lista=lista->siguiente;
free(lista);

Error, esto provocaría un verdadero problema, ya que hemos borrado un nodo ubicado en la lista y hemos perdido referencia al siguiente elemento y al anterior, teniendo como resultado dos listas separadas y perdidas en la memoria sin posibilidad de recuperarlas. Hagámoslo al revés:

free(lista);
lista=lista->siguiente;

Esto tampoco es correcto, intentamos avanzar en la lista mediante un puntero que ya no apunta a nada porque hemos liberado la memoria, perdiendo así el resto de la lista.

La solución, como va a ser costumbre, viene de la mano del puntero *aux, el programa sería así:

```c
int main(void) {
   struct Nodo * lista = NULL, * aux, * nuevo;

   aux = lista->sig ;
   free(lista);
   lista = aux;

   return 0;
}
```

¿Qué hemos hecho con ésto? Hemos hecho que el puntero *aux apunte al siguiente nodo de la lista, liberamos la memoria del puntero *lista (que siempre será nuestro punto de partida) y hacemos que *lista apunte a *aux, gracias a esto volvemos a tener un punto de inicio de la lista y hemos liberado en memoria el que antes era el primer nodo.

## Borrar el último nodo de la lista.

El último código de hoy en teoría parece también sencillo, si para borrar el primer elemento de la lista tan sólo se necesita un auxiliar para no perderlo todo, eliminar el último elemento debe ser igualmente fácil, tan sólo tendríamos que posicionarnos en el último elemento de la lista y liberar su memoria.

```c
while (aux->siguiente != NULL) {
   aux = aux-> siguiente;
}
free(aux);
```

Esto está bien a medias, porque ahora necesitamos que el penúltimo nodo (que ahora es el último) apunte a NULL, pero esto no es posible hacerlo debido a que no podemos determinar cual es el último nodo de la lista, ya que ninguna apunta a NULL, todos los nodos apuntan a un elemento.

¿Cómo solucionarlo? Pues de una forma muy sencilla, si con un auxiliar no tenemos suficiente, usemos dos.

Mediante un segundo auxiliar podemos determinar el nodo anterior al nodo que apunta *aux, de ésta forma cuando liberemos *aux podemos decir que *aux2 -> siguiente apunte a NULL, de ésta forma tenemos de nuevo la lista confeccionada correctamente.

El código es el siguiente:

struct Nodo * lista = NULL, * aux, * atras;

```c
atras=NULL;
aux=lista;

while (aux->siguiente != NULL) {
   atras=aux;
   aux = aux-> siguiente;
}

free(aux);
atras->siguiente = NULL;
```

Ahora sí que hace la función para la cual está pensada, el último nodo se ha eliminado y la lista queda perfectamente preparada para trabajar. 

No obstante se presenta un pequeño caso que nos puede pasar perfectamente, el caso de que la lista tenga sólo un elemento, por lo tanto no tiene sentido hacer atrás->siguiente=NULL, el único nodo que existiría es igualmente el nodo de cola, corrijamos éste detalle:

struct Nodo * lista = NULL, * aux, * atras;

```c
atras=NULL;
aux=lista;

while (aux->siguiente != NULL) {
   atras=aux;
   aux = aux-> siguiente;
}

free(aux);

if (atrás==NULL) {
   lista=NULL;
} else {
   atras->siguiente = NULL;
}
```

Con esto tenemos contemplado el caso de que la lista sólo contenga un elemento, el código está listo para usarse.

Para finalizar voy a poner los cuatro códigos que hemos visto hoy, pero antes os plantearé un pequeño caso y cómo afrontarlo, además de modificar todos los códigos y así tenerlos perfectamente para usarlos.

## Caso de la lista vacía.

Imaginemos que deseamos trabajar con listas pero ésta está vacía, los algoritmos que hemos planteado no sirven para listas vacías debido a que no tenemos nada que recorrer o que asignar a los punteros, esto es sencillo de controlar gracias a la estructura if, controlaremos que la lista no esté vacía y así podremos trabajar según qué casos.

## Códigos finales.

Los siguientes códigos son los códigos que hemos visto pero teniendo en cuenta el caso de la lista vacía. Ésta sería su versión definitiva.

Estructura utilizada:

```c
struct Nodo {
   int dato1;
   int dato2;
   Nodo *siguiente;
};

Añadir nodo en el principio:

{
struct Nodo * aux;

if (lista == NULL) {
   lista = malloc(sizeof(struct Nodo));
   lista->dato1 = 1;
   lista->dato2 = 2;
   lista->siguiente = NULL;
} else {
   aux = lista ;
   lista = malloc( sizeof(struct Nodo) );
   lista -> dato1=3;
   lista -> dato2=4;
   lista-> siguiente = aux ;
   }
}

Añadir nodo al final:

{
struct Nodo * aux, *nuevo;

if (lista == NULL) {
   lista = malloc(sizeof(struct Nodo));
   lista->info = valor;
   lista->siguiente = NULL;
} else {
   aux = lista ;
   while (aux->siguiente != NULL) {
      aux = aux-> siguiente;
   }
   nuevo = malloc( sizeof(struct Nodo) );
   nuevo-> dato1=2;
   nuevo-> dato2=6;
   nuevo-> siguiente = NULL;
   aux-> siguiente = nuevo ;
   }
}

Borrar nodo al principio:

{
struct Nodo * aux;
if (lista != NULL) {
   aux = lista->sig ;
   free(lista);
   lista = aux;
   }
}

Borrar nodo al final:

{
struct Nodo * aux, * atras;
if (lista != NULL) {
   atras=NULL;
   aux=lista;
   while (aux->siguiente != NULL) {
      atras=aux;
      aux = aux-> siguiente;
   }
   free(aux);
   if (atrás==NULL) {
      lista=NULL;
   } else {
      atras->siguiente = NULL;
   }
}
}
```

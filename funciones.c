#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include "headers/funciones.h"
#include "headers/treemap.h"
#include "headers/hashmap.h"
#include "headers/list.h"


struct Documento {
    char* nombre; // Nombre del documento.
    int cantidadPalabras; // Cantidad de palabras que contiene.
    int cantidadCaracteres; // Cantidad de caracteres que contiene.
    HashMap* palabras; // Mapa de palabras globables del documento.
    void *top; // Lista que contiene al Top 10.
    int topSize; // Cantidad de datos de la lista Top.
};

struct Palabra {
    char* palabra; // Nombre de la palabra.
    int repeticionesDocumento; // Cantidad de veces que aparece en un documento.
    List* apariciones; // Lista de documentos en donde se encuentra la palabra.
};

struct ArrayOrden{
    char* nombre; // Nombre del array.
    float frecuencia; // Valor de la frecuencia (calculada) de una palabra.
    int repeticiones; // Cantidad de veces que aparece una palabra.
};

struct Relevancia {
    Palabra* palabra; // Palabra ligada a su relevancia.
    float relevancia; // Valor de la relevancia (calculada) de una palabra.
};

// Convierte un int a puntero a int.
int *_intdup(int n) {
    int *aux = malloc(sizeof(int));
    *(aux) = n;

    return aux;
}

// Función que cuenta la cantidad de documentos existentes.
int contarDocumentos(TreeMap *documentos) {
    int cont = 0;

    Pair *documentoPair = firstTreeMap(documentos);

    while(documentoPair) {
        cont++;
        documentoPair = nextTreeMap(documentos);
    }

    return cont;
}

// Funcion que busca los documentos que contienen la palabra buscada y
// retorna la cantidad de documentos que la contienen.
int documentosConPalabra(HashMap *palabrasGlobales, char *palabra) {
    int cont = 0;

    HashMapPair *documentoPair = firstMap(palabrasGlobales);

    if (!documentoPair)
        return cont;
    
    List *auxList = documentoPair->value;
    Documento *documentoAux = first(auxList);

    while(documentoAux) {
        cont++;
        documentoAux = next(auxList);
    }

    return cont;
}

// Convierte un string a minúscula.
char *convertirAMinusculas(char *palabra) {
    char *aux = calloc(strlen(palabra)+1, sizeof(char));

    for (int i = 0; i < strlen(palabra); ++i) 
        aux[i] = tolower(palabra[i]);

    return aux;
}

// Funcion que crea un struct palabra y guarda su posicion en el documento y 
// la cantidad de repeticiones en este.
void *crearPalabra(char *palabra, int pos) {
    Palabra *new = malloc(sizeof(Palabra));

    new->palabra = (char*) malloc(strlen(palabra) + 1);
    strcpy(new->palabra, palabra);

    new->repeticionesDocumento = 1;

    new->apariciones = create_list();
    push_back(new->apariciones, &pos);

    return new;
}

// Funcion que crea un documento, cuenta las palabras y caracteres de este y guarda
// los datos en sus respectivas variables
void* crearDocumento(FILE *archivo, HashMap *palabrasGlobales, char* fileName) {
    Documento *documento = malloc(sizeof(Documento));

    HashMap *palabras = createMap(100);
    int cantPalabras = 0;
    int cantCaracteres = 0;

    char buffer[1024];
    int pos, *posPtr;

    while(1){
        if(fscanf(archivo, "%s", buffer) == EOF) break;
        pos = ftell(archivo);
        posPtr = _intdup(pos);
        strcpy(buffer, convertirAMinusculas(buffer));
        HashMapPair *palabraDocPair = searchMap(palabras, buffer);

        if (palabraDocPair != NULL) {
            Palabra *palabra = (Palabra*) palabraDocPair->value;
            palabra->repeticionesDocumento += 1;
            push_back(palabra->apariciones, posPtr);
        } else {
            Palabra *palabra = crearPalabra(buffer, *(posPtr));
            insertMap(palabras, palabra->palabra, palabra);

            HashMapPair *palabraGlobPair = searchMap(palabrasGlobales, palabra->palabra);
            if (palabraGlobPair == NULL) {
                List *docsConPalabra = create_list();
                push_back(docsConPalabra, documento);
                push_back(palabra->apariciones, posPtr);

                insertMap(palabrasGlobales, palabra->palabra, docsConPalabra);
            } else {
                List *docsConPalabra = palabraGlobPair->value;
                push_back(docsConPalabra, documento);
                push_back(palabra->apariciones, posPtr);
            }
        }

        cantPalabras++;
        cantCaracteres += strlen(buffer);
    }

    // Se declaran las variables para lograr obtener los 10 elementos con mayor frecuencia en el doc
    int topSize = 0, posMenor, valorMenor;
    HashMapPair *palabraPair = firstMap(palabras);
    Palabra *top = calloc(10, sizeof(Palabra));


    while(palabraPair) {
        Palabra *palabraAux = palabraPair->value;

        if (topSize < 10) {
            top[topSize] = *(palabraAux);
            topSize++;
        }
        else {
            posMenor = 0;
            valorMenor = top[0].repeticionesDocumento;

            for (int i = 1; i < topSize; i++) {
                if(top[i].repeticionesDocumento < valorMenor) {
                    posMenor = i;
                    valorMenor = top[i].repeticionesDocumento;
                }
            }

            if (palabraAux->repeticionesDocumento > valorMenor)
                top[posMenor] = *(palabraAux); 
        }

        palabraPair = nextMap(palabras);
    }

    // Se ordena el arreglo con los top 10 valores con mayor frecuencia.
    for (int i = 0; i < topSize; i++) {
        for (int j = i + 1; j < topSize; j++) {
            if (top[i].repeticionesDocumento < top[j].repeticionesDocumento) {
                Palabra temp = top[i];
                top[i] = top[j];
                top[j] = temp;
            }
        }
    }

    // Se asignan las variables correspondientes al documento.
    documento->nombre = (char*) malloc(strlen(fileName) + 1);
    strcpy(documento->nombre, fileName);
    documento->cantidadCaracteres = cantCaracteres;
    documento->cantidadPalabras = cantPalabras;
    documento->top = top;
    documento->topSize = topSize;
    documento->palabras = palabras;

    return documento;
}

// Funcion que recibe el nombre del documento (extraido de un arreglo de strings) y verificada
// la existencia del documento, se llama a la funcion que crea el documento y llena los mapas.
void cargarDocumento(TreeMap* documentos, HashMap* palabrasGlobales, char* fileName) {
    char *path, *folder = "documentos/";
    
    path = calloc(strlen(folder) + strlen(fileName) + 1, sizeof(char));
    strcat(path, folder);
    strcat(path, fileName);
    printf("%s...\n", path);

    FILE *archivo = fopen(path, "r");

    if (archivo == NULL)
        printf("No se pudo encontrar ningun archivo con el nombre %s :(\n", fileName);
    else {
        Pair *documentoPair = searchTreeMap(documentos, fileName);

        if (documentoPair != NULL)
            printf("El documento %s ya se encuentra agregado en el programa.\n", fileName);
        else {
            Documento* documento = crearDocumento(archivo, palabrasGlobales, fileName);

            insertTreeMap(documentos, fileName, documento);
            printf("El documento %s fue agregado con exito al programa :)\n", fileName);
        }   
    }
    printf("\n");
}


// Función que muestra los documentos de forma ordenada.
void mostrarDocumentos(TreeMap* documentos) {
    Pair *documentoPair = firstTreeMap(documentos);
    Documento *documento = NULL;

    if (documentoPair == NULL) {
        printf("No hay ningun documento cargado en el programa :(\n\n");
        return;
    }

    int cont = 1;

    printf("Los documentos son los siguientes:\n");
    while (documentoPair) {
        documento = documentoPair->value;

        printf("%d. %s. Palabras: %d. Caracteres: %d.\n", cont, documento->nombre, documento->cantidadPalabras, documento->cantidadCaracteres);
        cont++;

        documentoPair = nextTreeMap(documentos);
    }
}


// Función que recibe un arreglo de strings el cual es llenado con nombres de archivos
// para luego retornar un contador con la cantidad de archivos ingresados.
int leerInput(char** arregloSubStrings)
{
    int cont = 0;
    char stringInput[1024];
    printf("Ingrese los nombres de los documentos:\n");
    printf("(cada uno .txt al final)\n");
    
    gets(stringInput); 
    printf("\n");
    char *token = strtok(stringInput, "."); 

    while(token != NULL)
    {
        // Se copia el nombre del archivo sin el ".txt" en el arreglo de strings
        strcpy(arregloSubStrings[cont], token);
        // Se añade .txt para la posterior apertura y comparaciones del archivo
        strcat(arregloSubStrings[cont], ".txt");
        token = strtok(NULL, ".");
        // Se aumenta en 4 el recorrido del token, para que no guarde el "txt " residual que dejan los otros nombres
        // de archivos en el stringInput
        if(token) {
            token += 4;
            cont++;
        } 

    }

    return cont;
}

// Función comparar para la función qsort usada en la función 3.
// Ordena el campo repeticiones del struct ArrayOrden de mayor a menor.
int cmp_rep(const void *pivot, const void *elemento)
{
    ArrayOrden* ptrPivot = (ArrayOrden *) pivot;
    ArrayOrden* ptrElemento = (ArrayOrden *) elemento;
    if (ptrPivot->repeticiones < ptrElemento->repeticiones) 
        return 1;
    else
        return -1; 
}

// Función que muestra los documentos en donde está una palabra dada por el usuario y la cantidad de veces que aparece en cada documento.
void buscarPorPalabra(HashMap* palabrasGlobales)
{
    // Se crean las variables a utilizar para la búsqueda
    Palabra* auxPalabra = NULL;
    HashMapPair* auxPair = NULL;
    Documento* auxDocumento = NULL;
    HashMapPair* pairDocumento = NULL;
    char* palabraBuscada = (char*)malloc(sizeof(char) * 100);

    printf("Ingrese la palabra que desea buscar: \n");
    gets(palabraBuscada);
    fflush(stdin);
    // Se copia la palabra transformada en minúsculas, para evitar errores por falta de alguna mayuscula o minuscula
    strcpy(palabraBuscada, convertirAMinusculas(palabraBuscada));  

    // Se busca la palabra ingresada en el mapa global
    auxPair = searchMap(palabrasGlobales, palabraBuscada); 
    if(auxPair == NULL)
        printf("No se encontro la palabra en ningun documento :(\n");
    else
    {   // Se crea una variable del tipo ArrayOrden para organizar las palabras en orden de aparicion descendente, 
        // guardando las repeticiones de la palabra y el nombre del documento.
        ArrayOrden* apariciones = (ArrayOrden*)malloc(sizeof(ArrayOrden) * size(auxPair->value));
        printf("La palabra '%s' se encuentra en los documentos:\n", auxPair->key);   
        
        for(int i = 0 ; i < size(auxPair->value) ; i++) {                                           
            if(i == 0)
                auxDocumento = first(auxPair->value);
            else
                auxDocumento = next(auxPair->value); 

            pairDocumento = searchMap(auxDocumento->palabras, palabraBuscada); 
            auxPalabra = pairDocumento->value;  
            apariciones[i].nombre = (char*)malloc(strlen(auxDocumento->nombre) * sizeof(char));
            strcpy(apariciones[i].nombre, auxDocumento->nombre);
            apariciones[i].repeticiones = auxPalabra->repeticionesDocumento; 
        }
        // Se llama a la función qsort para que organice las apariciones de mayor a menor
        qsort(apariciones, size(auxPair->value), sizeof(ArrayOrden), cmp_rep); 

        for(int i = 0 ; i < size(auxPair->value) ; i++) 
            printf("%d. %s. Aparece %d veces. \n", i+1, apariciones[i].nombre, apariciones[i].repeticiones);
        
        printf("\n");

        free(apariciones);
    }
}

// Función que muestra el contexto de una palabra en un documento (ambos ingresados por el usuario)
// El contexto es una linea de 7 palabras en donde se encuentra la palabra inserta en el documento. 
void buscarPalabraEnDocumento(TreeMap* documentos, char* nombreDoc)
{
    // Se busca el documento en el arbol de documentos
    Pair* auxPair = searchTreeMap(documentos, nombreDoc);
    if(auxPair == NULL) {
        printf("El documento que ingreso no existe dentro del programa.\n");
        printf("Importe el documento o revise que exista entre nuestros documentos (opcion 2)\n");
    }
    else {
        char* palabraBuscar = (char*)malloc(sizeof(char) * 25);
        char x[1024];
        Documento* auxDoc = auxPair->value;

        // Se arma un string con el nombre del documento y la carpeta donde se encuentra ubicado para verificar su existencia
        char *path = calloc(strlen("documentos/") + strlen(auxDoc->nombre) + 1, sizeof(char));
        strcat(path, "documentos/");
        strcat(path, auxDoc->nombre);
        FILE *file = fopen(path, "r");

        if(!file) {
            printf("No se encontro el archivo");
            return;
        }

        // Si el archivo existe, se pide al usuario que ingrese la palabra a buscar y verifica la existencia de esta
        printf("Nombre del doc : %s\n", auxDoc->nombre);
        printf("\n\n");
        printf("Ingrese la palabra que desea buscar\n");
        gets(palabraBuscar);
        fflush(stdin);
        HashMapPair* auxHashPalabras = searchMap(auxDoc->palabras, palabraBuscar);
        
        if(auxHashPalabras == NULL)
            printf("La palabra no existe dentro del documento\n");
        else {
            int i = 0;
            Palabra* auxPalabra = auxHashPalabras->value;
            int *posicionPalabra;

            posicionPalabra = (int*) first(auxPalabra->apariciones);
            printf(" ");
            while(posicionPalabra != NULL) {

                // Intentamos empezar a mostrar el contexto 15 carácteres antes
                int posicionContexto = *(posicionPalabra) - 15, cantPalabras = 0;

                // Se verifica en caso de que intentemos a acceder a una posición inválida
                if (posicionContexto < 0)
                    posicionContexto = *(posicionPalabra);

                // Se setea la posición y se guarda la palabra
                fseek(file, posicionContexto, SEEK_SET);
                fscanf(file, "%1023s", x);

                // Avanzamos la posicion hasta la siguiente palabra, para evitar palabras que hayan quedado cortadas
                posicionContexto += strlen(x) + 1;
                if(i != 0)
                    printf("%d. ", i);

                // Se intenta mostrar una cantidad arbitraria de palabras (por eso el 7, elegido random)     
                while(cantPalabras < 7) { 
                    fscanf(file, "%1023s", x);
                    printf("%s ", x);
                    cantPalabras++;
                }
                printf("\n");
                i++; 
                posicionPalabra = (int*) next(auxPalabra->apariciones);
            }
        }
    }
} 

// Función que muestra las 10 palabras más frecuentes de un documento ingresado por el usuario.
// También muestra cuantas veces aparece cada una de ellas y su valor de frecuencia.
void mostrarPalabrasFrecuentes(char * nombreDoc, TreeMap * documentos)
{ 
    Pair* auxPairTree = searchTreeMap(documentos, nombreDoc);
    if(auxPairTree != NULL) {
        Documento *auxDocumento = auxPairTree->value;
        Palabra *top = auxDocumento->top;

        printf("Palabras con mayor frecuencia: \n");
        for(int i = 0 ; i < auxDocumento->topSize ; i++) {
            Palabra aux = top[i];
            float frecuencia = (float) aux.repeticionesDocumento / (float) auxDocumento->cantidadPalabras;
            printf("%d. Palabra: %s. Aparece %d veces. Frecuencia: %f. \n", i+1, aux.palabra, aux.repeticionesDocumento, frecuencia);
        }
        printf("\n");
    } else
        printf("No se ha podido encontrar el documento:(\n\n"); 
}

// Función que muestra las 10 palabras más relevantes de un documento ingresado por el usuario.
// También muestra el valor de relevancia de cada una de ellas.
 void palabrasRelevantes(char * nombreDoc, TreeMap *documentos, HashMap *palabrasGlobales) 
 { 
    Pair* auxPairTree = searchTreeMap(documentos, nombreDoc);
    if(auxPairTree != NULL) {
        Documento * auxDocumento = auxPairTree->value;

        HashMapPair *palabraPair = firstMap(auxDocumento->palabras);
        int totalPalabras = auxDocumento->cantidadPalabras; 

        int topSize = 0, posMenor, valorMenor;

        // Se declara un arreglo de struct tipo relevancia, en el cuál se almacenarán las palabras junto a su relevancia.
        Relevancia *top = calloc(10, sizeof(Relevancia));

        // Se recorren todas las palabras del documento, manteniendo en la variable *top el top 10 de las palabras
        // con mayor relevancia.
        while(palabraPair) {
            Palabra *palabraAux = palabraPair->value;
            float frecuencia = (float) palabraAux->repeticionesDocumento / (float) auxDocumento->cantidadPalabras;
            int cantDocumentos = contarDocumentos(documentos);
            int docsConPalabra = documentosConPalabra(palabrasGlobales, palabraAux->palabra);
            
            // Si es un documento, la relevancia siempre mostrará 0.00000 porque log(1) = 0
            if(cantDocumentos == 1){ 
                printf("Se necesitan mas documentos cargados para calcular la relevancia :(\n");
                printf("Intentelo denuevo.\n");
                return;
            }

            // Dado que nos interesa el top 10, si tenemos menos de 10 valores almacenados, se almacena inmediatamente.
            if (topSize < 10) {
                Relevancia *relevanciaAux = malloc(sizeof(Relevancia));
                relevanciaAux->palabra = palabraAux;
                relevanciaAux->relevancia = frecuencia * (log(cantDocumentos / docsConPalabra));

                top[topSize] = *(relevanciaAux);
                topSize++;
            }
            else {
                posMenor = 0;
                valorMenor = top[0].relevancia;
                float relevancia = frecuencia * (log(cantDocumentos / docsConPalabra));

                // Se busca el elemento con menor relevancia dentro del arreglo
                for (int i = 1; i < topSize; i++) {
                    if(top[i].relevancia < valorMenor) {
                        posMenor = i;
                        valorMenor = top[i].relevancia;
                    }
                }

                // Si la relevancia de la palabra es mayor que alguna dentro del arreglo del top 10, se reemplaza.
                if (relevancia > valorMenor) {
                    Relevancia *relevanciaAux = malloc(sizeof(Relevancia));
                    relevanciaAux->palabra = palabraAux;
                    relevanciaAux->relevancia = relevancia;
                    
                    free(&(top[posMenor]));
                    top[posMenor] = *(relevanciaAux); 
                }
            }

            palabraPair = nextMap(auxDocumento->palabras);
        }

        // Se ordena el arreglo con los top 10 valores con mayor relevancia.
        for (int i = 0; i < topSize; i++) {
            for (int j = i + 1; j < topSize; j++) {
                if (top[i].relevancia < top[j].relevancia) {
                    Relevancia temp = top[i];
                    top[i] = top[j];
                    top[j] = temp;
                }
            }
        }

        // Por último, luego de ordenar, se printea por consola el top de palabras relevantes.
        for (int i = 0; i < topSize; i++) {
            printf("%d. Palabra: %s. Relevancia: %f\n", i+1, top[i].palabra->palabra, top[i].relevancia);
        }
    }
 }
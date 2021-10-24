#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "headers/funciones.h"
#include "headers/treemap.h"
#include "headers/hashmap.h"
#include "headers/list.h"


struct Documento {
    char* nombre;
    int cantidadPalabras;
    int cantidadCaracteres;
    HashMap* palabras;
};

struct Palabra {
    char* palabra;
    int repeticionesDocumento;
    List* apariciones;
};

struct ArrayOrden{
    char* nombre;
    float frecuencia;
    int repeticiones;
};

int *_intdup(int n) {
    int *aux = malloc(sizeof(int));
    *(aux) = n;

    return aux;
}

char *convertirAMinusculas(char *palabra) {
    char *aux = calloc(strlen(palabra)+1, sizeof(char));

    for (int i = 0; i < strlen(palabra); ++i) 
        aux[i] = tolower(palabra[i]);

    return aux;
}

void *crearPalabra(char *palabra, int pos) {
    Palabra *new = malloc(sizeof(Palabra));

    new->palabra = (char*) malloc(strlen(palabra) + 1);
    strcpy(new->palabra, palabra);

    new->repeticionesDocumento = 1;

    new->apariciones = create_list();
    push_back(new->apariciones, &pos);

    return new;
}

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

                insertMap(palabrasGlobales, palabra->palabra, docsConPalabra);
            } else {
                List *docsConPalabra = palabraGlobPair->value;
                push_back(docsConPalabra, documento);
            }
        }

        cantPalabras++;
        cantCaracteres += strlen(buffer);
    }

    documento->nombre = (char*) malloc(strlen(fileName) + 1);
    strcpy(documento->nombre, fileName);
    documento->cantidadCaracteres = cantCaracteres;
    documento->cantidadPalabras = cantPalabras;
    documento->palabras = palabras;

    return documento;
}

void cargarDocumento(TreeMap* documentos, HashMap* palabrasGlobales, char* fileName) {
    char *path, *folder = "documentos/";
    
    path = calloc(strlen(folder) + strlen(fileName) + 1, sizeof(char));
    strcat(path, folder);
    strcat(path, fileName);
    printf("%s...\n", path);

    FILE *archivo = fopen(path, "r");

    if (archivo == NULL)
        printf("No se pudo encontrar ningun archivo con el nombre % :(\n", fileName);
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


void mostrarDocumentos(TreeMap* documentos) {
    Pair *documentoPair = firstTreeMap(documentos);
    Documento *documento = NULL;

    int cont = 1;

    printf("Los documentos son los siguientes:\n");
    while (documentoPair) {
        documento = documentoPair->value;

        printf("%d. %s. Palabras: %d. Caracteres: %d.\n", cont, documento->nombre, documento->cantidadPalabras, documento->cantidadCaracteres);
        cont++;

        documentoPair = nextTreeMap(documentos);
    }
}

int leerInput(char** arregloSubStrings)
{
    int cont = 0;
    char stringInput[256];
    printf("Ingrese los nombres de los documentos:\n");
    printf("(cada uno .txt al final)\n");
    gets(stringInput);
    printf("\n");
    char *token = strtok(stringInput, ".");

    while(token != NULL)
    {
        strcpy(arregloSubStrings[cont], token);
        strcat(arregloSubStrings[cont], ".txt");
        token = strtok(NULL, ".");

        if(token) {
            token += 4;
            cont++;
        } 

    }

    return cont;
}

int cmp_ascendente_repeticiones(const void *pivot, const void *elemento)
{
    ArrayOrden* ptrPivot = (ArrayOrden *) pivot;
    ArrayOrden* ptrElemento = (ArrayOrden *) elemento;
    if (ptrPivot->repeticiones < ptrElemento->repeticiones) 
        return 1;
    else
        return -1; 
}

int cmp_ascendente_frecuencia(const void *pivot, const void *elemento)
{
    ArrayOrden* ptrPivot = (ArrayOrden *) pivot;
    ArrayOrden* ptrElemento = (ArrayOrden *) elemento;
    if (ptrPivot->frecuencia < ptrElemento->frecuencia) 
        return 1;
    else
        return -1; 
}

void buscarPorPalabra (HashMap* palabrasGlobales)
{
    Palabra* auxPalabra = NULL;
    HashMapPair* auxPair = NULL;
    Documento* auxDocumento = NULL;
    HashMapPair* pairDocumento = NULL;
    char* palabraBuscada = (char*)malloc(sizeof(char) * 100);

    printf("Ingrese la palabra que desea buscar: \n");
    gets(palabraBuscada);
    fflush(stdin);
    strcpy(palabraBuscada, convertirAMinusculas(palabraBuscada));  

    auxPair = searchMap(palabrasGlobales, palabraBuscada); 
    if(auxPair == NULL)
        printf("No se encontro la palabra en ningun documento :(\n");
    else
    {
        ArrayOrden* apariciones = (ArrayOrden*)malloc(sizeof(ArrayOrden) * size(auxPair->value));
        apariciones->nombre = (char*)malloc(sizeof(char) * 100);
        
        printf("La palabra '%s' se encuentra en los documentos:\n", auxPair->key);   
        
        for(int i = 0 ; i < size(auxPair->value) ; i++) 
        {                                           
            if(i == 0)
                auxDocumento = first(auxPair->value);
            else
                auxDocumento = next(auxPair->value); 

            pairDocumento = searchMap(auxDocumento->palabras, palabraBuscada); 
            auxPalabra = pairDocumento->value;  
            apariciones[i].nombre= auxDocumento->nombre;
            apariciones[i].repeticiones = auxPalabra->repeticionesDocumento; // falta solo la 5 entera, no?
                                                                            
        qsort(apariciones, size(auxPair->value), sizeof(ArrayOrden), cmp_ascendente_repeticiones); 

        for(int i = 0 ; i < size(auxPair->value) ; i++)
            printf("%d. %s. Aparece %d veces. \n", i+1, apariciones[i].nombre, apariciones[i].repeticiones);
        
        printf("\n");

        free(apariciones);
        }
    }
}

void buscarPalabraEnDocumento(TreeMap* documentos, char* nombreDoc)
{
    printf("...\n");
    Pair* auxPair = searchTreeMap(documentos, nombreDoc);
    if(auxPair == NULL) {
        printf("El documento que ingreso no existe dentro del programa.\n");
        printf("Importe el documento o revise que exista entre nuestros documentos (opcion 2)\n");
    }
    else {
        char* palabraBuscar = (char*)malloc(sizeof(char) * 25);
        char x[1024];
        Documento* auxDoc = auxPair->value;

        char *path = calloc(strlen("documentos/") + strlen(auxDoc->nombre) + 1, sizeof(char));
        strcat(path, "documentos/");
        strcat(path, auxDoc->nombre);
        FILE *file = fopen(path, "r");

        if(!file) {
            printf("No se encontro el archivo");
            return;
        }
            
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
            printf("--Nombre del doc = %s\n", auxDoc->nombre);
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
                    printf("%d.", i);
                     
                while(cantPalabras < 7) { // Se intenta mostrar una cantidad arbitraria de palabras (por eso el 7, random)
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


 void mostrarPalabrasFrecuentes(char * nombreDoc, TreeMap * documentos)
 { 
    Pair* auxPairTree = searchTreeMap(documentos, nombreDoc);
    if(auxPairTree != NULL)
    {
        Documento * auxDocumento = auxPairTree->value;
        HashMap * mapPalabras = auxDocumento->palabras;

        int totalPalabras = auxDocumento->cantidadPalabras;
       
        ArrayOrden* frecuentes = (ArrayOrden*)malloc(sizeof(ArrayOrden) * totalPalabras);
        frecuentes->nombre = (char*)malloc(sizeof(char) * 100);
       
        HashMapPair * palabraPair = firstMap(mapPalabras); 

        int i = 0;
        while(palabraPair != NULL)
        {
            Palabra * auxPalabra = palabraPair->value;

            frecuentes[i].frecuencia = (float)auxPalabra->repeticionesDocumento / (float)totalPalabras;
            frecuentes[i].repeticiones = auxPalabra->repeticionesDocumento;
            frecuentes[i].nombre = malloc(strlen(auxPalabra->palabra) + 1);
            strcpy(frecuentes[i].nombre, auxPalabra->palabra);
            
            palabraPair = nextMap(mapPalabras);

            i++; 
        }

        qsort(frecuentes, totalPalabras, sizeof(ArrayOrden), cmp_ascendente_frecuencia);

        printf("Palabras con mayor frecuencia: \n");
        for(int i = 0 ; i < 10 ; i++)
        {
            printf("%d. Palabra: %s. Aparece %d veces. Frecuencia: %f. \n", i+1, frecuentes[i].nombre, frecuentes[i].repeticiones, frecuentes[i].frecuencia);
        }
        printf("\n");
        
        free(frecuentes);
    }
    /*else
    {

    }*/
     

 }

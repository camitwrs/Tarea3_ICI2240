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

void *crearDocumento(FILE *archivo, HashMap *palabrasGlobales, char* fileName) {
    Documento *documento = malloc(sizeof(Documento));

    HashMap *palabras = createMap(100);
    int cantPalabras = 0;
    int cantCaracteres = 0;

    char buffer[1024];
    int pos;

    while(1){
        if(fscanf(archivo, "%s", buffer) == EOF) break;

        pos = ftell(archivo);
        strcpy(buffer, convertirAMinusculas(buffer));
        HashMapPair *palabraDocPair = searchMap(palabras, buffer);

        if (palabraDocPair != NULL) {
            Palabra *palabra = (Palabra*) palabraDocPair->value;
            palabra->repeticionesDocumento += 1;
        } else {
            Palabra *palabra = crearPalabra(buffer, pos);
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

    FILE *archivo = fopen(path, "r");

    if (archivo == NULL)
        printf("No se pudo encontrar ningun archivo con ese nombre");
    else {
        Pair *documentoPair = searchTreeMap(documentos, fileName);

        if (documentoPair != NULL)
            printf("El documento %s ya se encuentra agregado en el programa\n", fileName);
        else {
            Documento* documento = crearDocumento(archivo, palabrasGlobales, fileName);

            insertTreeMap(documentos, fileName, documento);
            printf("El documento %s fue annadido con exito al programa\n", fileName);
        }   
    }
}


void mostrarDocumentos(TreeMap* documentos) {
    Pair *documentoPair = firstTreeMap(documentos);
    Documento *documento = NULL;

    int cont = 1;

    while (documentoPair) {
        documento = documentoPair->value;

        printf("%d. %s. Palabras: %d. Caracteres: %d\n", cont, documento->nombre, documento->cantidadPalabras, documento->cantidadCaracteres);
        cont++;

        documentoPair = nextTreeMap(documentos);
    }
}

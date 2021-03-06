#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "headers/menu.h"
#include "headers/funciones.h"
#include "headers/treemap.h"
#include "headers/hashmap.h"
#include "headers/list.h"

#define CANT_DOCS 59

/*char * _strdup(const char * str) {
    char * aux = (char *)malloc(strlen(str) + 1);
    strcpy(aux, str);
    return aux;
}*/

int lower_than_string(void* key1, void* key2){
    char* k1=(char*) key1;
    char* k2=(char*) key2;
    if(strcmp(k1,k2)<0) return 1;
    return 0;
}

int getChoice() {
    int validInput = 0;
    char inputChoice[3];

    while(validInput == 0) {
        scanf("%2s", inputChoice);
        fflush(stdin);
    
        if(isdigit(*inputChoice)){
            return atoi(inputChoice);
        } else {
            printf("La opcion que ingresaste no es valida.");
        }
    }
}

void initMenu() {
    TreeMap* documentos = createTreeMap(lower_than_string);  // Key: nombre de documento | Value: puntero a struct Documento
    HashMap* palabrasGlobales = createMap(100);  // Key: palabra | Value: lista con punteros a todos los documentos que contienen la palabra

    int choice;

    while (choice != 0) {
        printf(" ---------------------------------------------------------------------- \n");
        printf("|                                                                      |\n");
        printf("|         MENU OPCIONES                                                |\n");
        printf("|                                                                      |\n");
        printf("|         1: Cargar documentos                                         |\n");
        printf("|         2: Mostrar documentos ordenados                              |\n");
        printf("|         3: Buscar documentos por palabra                             |\n");
        printf("|         4: Mostrar palabras frecuentes de un documento               |\n");
        printf("|         5: Mostrar palabras mas relevantes de un documento           |\n");
        printf("|         6: Mostrar contexto de una palabra en un documento           |\n");
        printf("|         0: Salir                                                     |\n");
        printf("|                                                                      |\n");
        printf(" ----------------------------------------------------------------------\n");

        choice = getChoice();

        switch (choice) {
            case 1:
                cargarDocumentoIU(documentos, palabrasGlobales);
                break;
            case 2:
                mostrarDocumentos(documentos);
                break;
            case 3:
                buscarPorPalabra(palabrasGlobales);
                break;
            case 4: 
                palabrasFrecuentesIU(documentos);
                break;
            case 5: 
                palabrasRelevantesIU(documentos, palabrasGlobales);
                break;
            case 6: 
                buscarPalabraEnDocumentoIU(documentos);
                break;
        }
    }
}

void cargarDocumentoIU(TreeMap* documentos, HashMap* palabrasGlobales)
{
    int contador = 0;

    // Creaci??n de array y reserva de memoria del array en s?? y de las palabras.
    char** arregloSubStrings = (char**)calloc(CANT_DOCS, sizeof(char*));
    for(int i = 0 ; i < CANT_DOCS ; i++)
        arregloSubStrings[i] = (char *)calloc(50, sizeof(char));
    
    // Lee la l??nea de input ingresada por el usuario, separa esa l??nea en sub-strings (cada archivo), los almacena en un array
    // y retorna el tama??o del array (cantidad de archivos/sub-strings le??dos de la l??nea inicial)
    contador = leerInput(arregloSubStrings);

    // Se recorre el array y para cada archivo (cada ??ndice del array) se van cargando los documentos.
    for(int i = 0 ; i < contador ; i++)
        cargarDocumento(documentos, palabrasGlobales, arregloSubStrings[i]);

    // Se libera la memoria del array.
    free(arregloSubStrings);
}

void palabrasFrecuentesIU(TreeMap* documentos)
{
    char nombreDoc[100];
    printf("Ingrese el nombre del documento: (con .txt al final)\n");
    gets(nombreDoc);
    printf("\n");

    mostrarPalabrasFrecuentes(nombreDoc, documentos);
}

void buscarPalabraEnDocumentoIU(TreeMap* documentos) 
{   
    char* nombreDoc = (char*)malloc(sizeof(char) * 100);
    printf("Ingrese el nombre del documento donde desea realizar la busqueda (recuerde agregar el .txt al final)\n");
    gets(nombreDoc);
    fflush(stdin);
    printf("\n");

    buscarPalabraEnDocumento(documentos, nombreDoc);
}

void palabrasRelevantesIU(TreeMap * documentos, HashMap* palabrasGlobales)
{
    printf("Ingrese el nombre del documento (recuerde usar .txt al final de este): \n");
    char* nombreDoc = (char*)malloc(sizeof(char) * 100);
    gets(nombreDoc);
    fflush(stdin); 
    printf("\n");

    palabrasRelevantes(nombreDoc, documentos, palabrasGlobales);
}
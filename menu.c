#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "headers/funciones.h"
#include "headers/treemap.h"
#include "headers/hashmap.h"
#include "headers/list.h"


char * _strdup(const char * str) {
    char * aux = (char *)malloc(strlen(str) + 1);
    strcpy(aux, str);
    return aux;
}

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
    TreeMap* documentos = createTreeMap(lower_than_string); 
    HashMap* palabrasGlobales = createMap(100);

    int choice;

    while (choice != 0) {
        printf(" ---------------------------------------------------------------------- \n");
        printf("|                                                                      |\n");
        printf("|         MENU OPCIONES                                                |\n");
        printf("|                                                                      |\n");
        printf("|         1: Cargar lista de documentos                                |\n");
        printf("|         2: Mostrar documentos ordenados                              |\n");
        printf("|         3: Buscar por palabra                                        |\n");
        printf("|         4: Buscar palabras con mayor frecuencia                      |\n");
        printf("|         5: Buscar palabras mas relevantes                            |\n");
        printf("|         6: Buscar palabras en documento                              |\n");
        printf("|         7: Mostrar todos los productos                               |\n");
        printf("|         0: Salir                                                     |\n");
        printf("|                                                                      |\n");
        printf(" ----------------------------------------------------------------------\n");

        choice = getChoice();

        switch (choice) {
            case 1:
                cargarDocumento(documentos, palabrasGlobales, "16-0.txt");
                cargarDocumento(documentos, palabrasGlobales, "76-0.txt");
                break;
            case 2:
                mostrarDocumentos(documentos);
                break;
            case 3: break;
            case 4: break;
            case 5: break;
            case 6: break;
            case 7: break;
        }
    }
}

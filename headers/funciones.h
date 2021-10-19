#include "treemap.h"
#include "hashmap.h"

typedef struct Documento Documento;

typedef struct Palabra Palabra;

typedef struct ArrayOrden ArrayOrden;

void cargarDocumento(TreeMap* documentos, HashMap* palabrasGlobales, char* nombre);

void mostrarDocumentos(TreeMap* documentos);

int leerInput(char** arregloSubStrings);

void cargarDocumentoIU (TreeMap* documentos, HashMap* palabrasGlobales);

void buscarPorPalabra (HashMap* palabrasGlobales);
#include "treemap.h"
#include "hashmap.h"

typedef struct Documento Documento;

typedef struct Palabra Palabra;

void cargarDocumento(TreeMap* documentos, HashMap* palabrasGlobales, char* nombre);

void mostrarDocumentos(TreeMap* documentos);
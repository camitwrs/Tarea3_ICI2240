#include "list.h"
#include "hashmap.h"
#include "treemap.h"

void initMenu();

void cargarDocumentoIU(TreeMap* documentos, HashMap* palabrasGlobales);

void palabrasFrecuentesIU(TreeMap* documentos);

void buscarPalabraEnDocumentoIU(TreeMap* documentos);

void palabrasRelevantesIU(TreeMap *documentos, HashMap* palabrasGlobales);

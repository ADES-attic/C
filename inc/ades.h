#include <ds.h>

extern int readPSVFile(char *fn, observationBatch ** o, char *schema);
extern int writeXMLFile(observationBatch * o, char *fn);

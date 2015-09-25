#include <ds.h>

extern int readPSVFile(char *fn, observationBatch ** o, char *schema);
extern int readXMLFile(char *fn, observationBatch ** o, char *schema);

extern int writePSVFile(observationBatch * o, char *fn);
extern int writeXMLFile(observationBatch * o, char *fn);

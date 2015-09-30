#include <ds.h>

int readXMLFile(char *fn, observationBatch ** o, char *schema);
int readPSVFile(char *fn, observationBatch ** o, char *schema);

int writeXMLFile(observationBatch * o, char *fn);
int writePSVFile(observationBatch * o, char *fn,
                 _Bool defaultPSV, int pipeAlign);

// readability macros for defaultPSV parameter (PF_ for (P)SV (F)ormat)
#define PF_MIN     0
#define PF_DEFAULT 1

// readability macros for pipeAlign parameter
#define PA_NONE 0
#define PA_DATA 1
#define PA_HDRS 2

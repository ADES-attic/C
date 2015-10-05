//
// ades.h -- ades library API
//
// Definitions here are intended to make up the public API for the ades library.

#include <ds.h>

// readXMLFile -- Read an XML file, parsing it into the observationBatch
// C struct, optionally validating.
//
// Argument fn must be the file name of an ADES XML file, observationBatch
// must be a non-null pointer to an observationBatch pointer.  Argument
// schema must either be the file name of an ADES XSD file or a null pointer.
// If non-null, the function will validate the XML, if null there is no
// validation.
int readXMLFile(char *fn, observationBatch ** o, char *schema);

// readPSVFile -- Read an PSV file, parsing it into the observationBatch
// C struct, optionally validating.
//
// Argument fn must be the file name of an ADES PSV file, observationBatch
// must be a non-null pointer to an observationBatch pointer.  Argument
// schema must either be the file name of an ADES XSD file or a null pointer.
// If non-null, the function will validate the PSV, if null there is no
// validation.
int readPSVFile(char *fn, observationBatch ** o, char *schema);

// writeXMLFile -- Write an ADES XML file from an observationBatch struct.
//
// Argument observationBatch must be a non-null pointer to an observationBatch
// struct with valid data.  Output XML is written to fn.
int writeXMLFile(observationBatch * o, char *fn);

// writeXMLFile -- Write an ADES XML file from an observationBatch struct.
//
// Argument observationBatch must be a non-null pointer to an observationBatch
// struct with valid data.  Output XML is written to fn.
int writePSVFile(observationBatch * o, char *fn,
                 _Bool defaultPSV, int pipeAlign);

// Readability macros for defaultPSV parameter (PF_ for (P)SV (F)ormat)
//
// PF_DEFAULT specifies "Default PSV" as described in the ADES.  Certain fields
// are included even if blank in all records, and data for these fields are
// written in a certain format.
//
// PF_MIN specifies a more minimal format.  Fields with no data are not
// included and data is written without constraints on the format.
#define PF_MIN     0
#define PF_DEFAULT 1

// Readability macros for pipeAlign parameter.
//
// PA_NONE does no column alignment, both column headings and field data
// are written with no additional spacing.
//
// PA_DATA aligns data columns but does not align the column headings above
// the columns.  Column headings are written as with PA_NONE.  Field data
// written with space padding so that pipe characters align vertically.
//
// PA_HDRS aligns both column headings and data.  Both column headings and
// field data are written with space padding so that pip characters align
// vertically.
#define PA_NONE 0
#define PA_DATA 1
#define PA_HDRS 2

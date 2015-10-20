//
// ades.h -- ades library API
//
// Definitions here are intended to make up the public API for the ades library.
//
// Synopsis:
// int readMPC80File(char *fn, observationBatch ** o,
//                   char *schema, char *logsnr);
// int readPSVFile(char *fn, observationBatch ** o, char *schema);
// int readXMLFile(char *fn, observationBatch ** o, char *schema);
// int writeXMLFile(observationBatch * o, char *fn);
// int writePSVFile(observationBatch * o, char *fn,
//                  _Bool defaultPSV, int pipeAlign);
//
// The `observationBatch` type is documented in ds.h.

#include <ds.h>

// readMPC80File -- Read an MPC 80 column observation file, parsing it into
// the observationBatch C struct, optionally validating.
//
// Argument fn must be the file name of an MPC 80 column observation file,
// observationBatch must be a non-null pointer to an observationBatch pointer.
// Argument schema must either be the file name of an ADES XSD file or a null
// pointer.  If non-null, the function will validate the PSV, if null there is
// no validation.  If logsnr is non-NULL, it will be added as a logSNR element
// to each observation.
int readMPC80File(char *fn, observationBatch ** o, char *schema, char *logsnr);

// readPSVFile -- Read an PSV file, parsing it into the observationBatch
// C struct, optionally validating.
//
// Argument fn must be the file name of an ADES PSV file, observationBatch
// must be a non-null pointer to an observationBatch pointer.  Argument
// schema must either be the file name of an ADES XSD file or a null pointer.
// If non-null, the function will validate the PSV, if null there is no
// validation.
int readPSVFile(char *fn, observationBatch ** o, char *schema);

// readXMLFile -- Read an XML file, parsing it into the observationBatch
// C struct, optionally validating.
//
// Argument fn must be the file name of an ADES XML file, observationBatch
// must be a non-null pointer to an observationBatch pointer.  Argument
// schema must either be the file name of an ADES XSD file or a null pointer.
// If non-null, the function will validate the XML, if null there is no
// validation.
int readXMLFile(char *fn, observationBatch ** o, char *schema);

// writePSVFile -- Write an ADES PSV file from an observationBatch struct.
//
// Argument observationBatch must be a non-null pointer to an observationBatch
// struct with valid data.  Output PSV is written to fn.  Readability macros
// for defaultPSV and pipeAlign defined below.
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

// writeXMLFile -- Write an ADES XML file from an observationBatch struct.
//
// Argument observationBatch must be a non-null pointer to an observationBatch
// struct with valid data.  Output XML is written to fn.
int writeXMLFile(observationBatch * o, char *fn);

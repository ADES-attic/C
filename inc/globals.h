//
// globals.h
//
// Definitions intended just for use internal to ades library.

#include <libxml/tree.h>

#include <ds.h>

// common data for reading and parsing files
extern int lineNum;             // for error messages
extern char line[512];          // buffer for holding a single line 
extern char line2[512];         // for copies of line

extern xmlDocPtr doc;
extern xmlNodePtr root_node;

// common error buffer
extern char errLine[512];

// stores msg in errLine, returns -1
int error(char *msg);

// formats msg and arg into errLine, returns -1
int error1(char *msg, char *arg);

// exit function for use from executables.
// not to be used from library functions.
void errExit(int);

int pt(char *fn, xmlDocPtr * pDoc);
int tv(xmlDocPtr doc, char *schema);
void ts(xmlDocPtr doc, observationBatch ** obs);

// fldNames is a list of ADES field names in ADES order.  That is, first
// the order specified in the "default PSV" section, then the order of
// appearance in the ADES.  This list can be indexed by the `fldNum` enum
// defined in ds.h.
extern char *fldNames[];

// fldNum searches fldNames and returns the index of the name matching the
// argument `col`.  It returns -1 if not found.
int fldNum(const char *col);

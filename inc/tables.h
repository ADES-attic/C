//
// tables.h
//
// Currently internal definitions.

// fldNames is a list of ADES field names in ADES order.  That is, first
// the order specified in the "default PSV" section, then the order of
// appearance in the ADES.  This list can be indexed by the `fldNum` enum
// defined in ds.h.
extern char *fldNames[];

// fldNum searches fldNames and returns the index of the name matching the
// argument `col`.  It returns -1 if not found.
int fldNum(const char *col);

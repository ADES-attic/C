#define _GNU_SOURCE
#include <string.h>
#include <unistr.h>

#include <libxml/tree.h>

#include <alerr.h>
#include <ds.h>
#include <tables.h>

// globals
extern char *fldNames[];
extern int nFlds;
typedef void (*obsRecSetter) (obsRec *, char *);
extern obsRecSetter setObsRec[];
extern char *H1Names[];
extern int nH1Names;

FILE *fpsv;
int lineNum;
char line[512];                 // buffer for holding a single line of PSV
char line2[512];                // for copies of line, or for formatting errors

xmlDocPtr doc;
xmlNodePtr root_node;

int errorPSV(char *msg)
{
  snprintf(errLine, sizeof errLine, "PSV line %d: %s", lineNum, msg);
  return -1;
}

int errorPSV1(char *msg, char *arg)
{
  int n = snprintf(errLine, sizeof errLine, "PSV line %d: ", lineNum);
  snprintf(errLine + n, sizeof errLine - n, msg, arg);
  return -1;
}

// TODO figure out the rest of Unicode whitespace. (white space property?
// category Z? related functions in libunistring?)
#define LATIN1_WS "\t\n\v\f\r \x85\xA0"

// trimRight trims white space from the right of a string, by punching a
// null terminator byte.
//
// arg p must be a UTF-8 string.
//
// If successful, trimRight returns 0 as the return value and sets trimmed to
// the number of whitespace bytes trimed.
//
// trimRight scans the entire string; any non UTF-8 is detected and a non
// zero value is returned as an error.
int trimRight(uint8_t * p, size_t * trimmed)
{
  uint8_t *wsStart = NULL;
  size_t all = strlen(p);
  size_t len;
  ucs4_t c;
  while (*p) {
    len = u8_mbtouc(&c, p, all);
    if (!len)
      return errorPSV("invalid UTF-8");
    if (u8_chr(LATIN1_WS, sizeof LATIN1_WS, c)) {
      if (!wsStart)
        wsStart = p;
    } else {
      if (wsStart)
        wsStart = NULL;
    }
    p += len;
    all -= len;
  }
  if (!wsStart)
    *trimmed = 0;
  else {
    *wsStart = 0;
    *trimmed = p - wsStart;
  }
  return 0;
}

// firstNonWS sets pointer n to the first non-whitespace character in p
// and returns 0 as success.
//
// p must be a null terminated UTF-8 string.
//
// If it encounters non-UTF-8 it returns a non-zero value as an error.
int firstNonWS(char *p, char **n)
{
  // note similar code in nonWSEnd
  size_t all = strlen(p);
  size_t len;
  ucs4_t c;
  while (*p) {
    len = u8_mbtouc(&c, p, all);
    if (!len)
      return errorPSV("invalid UTF-8");
    // reversed test is only difference from nonWSEnd
    if (!u8_chr(LATIN1_WS, sizeof LATIN1_WS, c))
      break;
    p += len;
    all -= len;
  }
  *n = p;
  return 0;
}

// nonWSEnd sets pointer n to the first whitespace character in p or the
// the terminating null, and returns 0 as success.
//
// p must be a null terminated UTF-8 string.
//
// If it encounters non-UTF-8 it returns a non-zero value as an error.
int nonWSEnd(char *p, char **n)
{
  // note similar code in firstNonWS
  size_t all = strlen(p);
  size_t len;
  ucs4_t c;
  while (*p) {
    len = u8_mbtouc(&c, p, all);
    if (!len)
      return errorPSV("invalid UTF-8");
    // reversed test is only difference from firstNonWS
    if (u8_chr(LATIN1_WS, sizeof LATIN1_WS, c))
      break;
    p += len;
    all -= len;
  }
  *n = p;
  return 0;
}

// trim trims both leading and trailing whitespace.
//
// It punches a null in the last whitespace character from the end.
// It to also sets pointer n to first non-whitespace character and
// returns 0 as success.
//
// If it encounters non-UTF-8 it returns a non-zero value as an error.
int trim(char *s, char **n)
{
  size_t w;
  int r = trimRight(s, &w);     // w unused
  if (r < 0)
    return r;
  return firstNonWS(s, n);
}

// get non-blank line, validate UTF-8, trim trailing space.
//
// on success global `line` contains a non-empty string and  0 is returned.
//
// at end of file, *line is set to NULL, and 0 is still returned.
//
// A non-zero return value means some other error, such as a line too long.
int getPSVLine()
{
  size_t w;
  char *p;
  do {
    // TODO fgets bad, getline (GNU) better?
    p = fgets(line, sizeof(line), fpsv);
    if (!p) {
      *line = 0;
      break;
    }
    if (strlen(p) == sizeof line - 1)
      return errorPSV("line too long");
    lineNum++;
    // trimRight also validates entire string as UTF-8.
    trimRight(p, &w);           // w unused
  }
  while (!*p);                  // loop on blank lines
  return 0;
}

// On entry `line` must contain the PSV column headers.
//
// The line is parsed, nCols is set to the number of columns found,
// pCols is set to an allocated array of field numbers (OF_ enum values)
// 
//
// return 0 for success, non-zero on error.
int splitColHdrs(int *nCols, int **pCols)
{
  // count pipes, set nCols, allocate array for text of headers
  int nPipes = 0;
  char *p2 = strchr(line, 0);
  for (const char *p = line; p = u8_chr(p, p2 - p, '|'); p++)
    nPipes++;
  *nCols = nPipes + 1;
  char **colHdrs = malloc(*nCols * sizeof(char *));

  // scan again, filling header array
  char *cs = strdup(line);
  colHdrs[0] = cs;
  int i = 1;
  // (cast away const)
  p2 = strchr(cs, 0);
  for (char *p = cs; p = (char *)u8_chr(p, p2 - p, '|');) {
    *p++ = 0;
    colHdrs[i++] = p;
  }

  // allocate colNums, trim headers, catch blank headers,
  // validate against fldNames, fill colNums, set pCols
  int *cols = malloc(*nCols * sizeof(int));
  char *p;
  int j;
  for (int i = 0; i < *nCols; i++) {
    int r = trim(colHdrs[i], &p);
    if (r < 0)
      return r;
    if (!*p)
      return errorPSV("empty column header");
    int j = fldNum(p);
    if (j < 0)
        return errorPSV1("unknown field used as column header: %s", p);
    cols[i] = j;
  }
  *pCols = cols;
  return 0;
}

// On entry `line` must contain the PSV column headers and ol must be a
// valid pointer to an obsList struct.
//
// On success ol is populated and the function returns 0.
int pxObs(obsList * ol)
{
  int nCols;
  int *fldNums;
  int rc = splitColHdrs(&nCols, &fldNums);
  if (rc != 0) {
    return rc;
  }
  if (fldNums[0] != 0)
    return errorPSV("first column must be permID");
  int len = 0;
  int cap = 4;
  obsRec *obs = calloc(cap, sizeof(obsRec));
  int r;
  while (1) {
 nextLine:
    r = getPSVLine();
    if (r != 0)
      return r;
    if (!*line || *line == '#')
      break;
    strcpy(line2, line);        // save a copy

    char *e = strchr(line, 0);
    char *fld = line;
    for (int col = 0;; col++) {
      if (col >= nCols)
        return errorPSV("more fields than column headers");
      char *end = u8_chr(fld, e - fld, '|');
      if (end)
        *end++ = 0;
      r = trim(fld, &fld);
      if (r != 0)
        return r;

      // special handling at first field:
      if (col == 0) {
        int n = fldNum(fld);
        if (n >= F_PERMID) { // if it's a field name,
          if (n != F_PERMID) // it must be permID
            return errorPSV("first column must be permID");
          strcpy(line, line2);  // restore line (we punched holes in it)
          // and reparse as headers
          rc = splitColHdrs(&nCols, &fldNums);
          if (rc != 0) {
            return rc;
          }
          // break column loop, continue with next PSV line
          goto nextLine;
        }
        // normal case: grow list as needed
        if (len == cap) {
          cap *= 2;
          obs = realloc(obs, cap * sizeof(obsRec));
          memset(obs + len, 0, len * sizeof(obsRec));
        }
      }

      if (*fld)
        obs[len][fldNums[col]] = strdup(fld);
      if (!end)
        break;
      fld = end;
    }
    len++;                      // obsRec complete
  }
  // complete obsList
  if (len < cap)
    obs = realloc(obs, len * sizeof(obsRec));
  ol->len = len;
  ol->observations = obs;
  return 0;
}

// parse a single header line, either a # or ! line.
int parseHdr(char **kwd, char **txt)
{
  // find keyword start.  first byte of line is known to be # or !,
  // white space can follow, then a keyword must start.
  char *kwStart;
  int r = firstNonWS(line + 1, &kwStart);
  if (r != 0)
    return r;
  if (!*kwStart)
    return errorPSV("missing header keyword");

  // find keyword end
  char *kwEnd;
  r = nonWSEnd(kwStart, &kwEnd);
  if (r != 0)
    return r;
  if (!*kwEnd)
    *txt = kwEnd;
  else {
    firstNonWS(kwEnd, txt);
    *kwEnd = 0;
  }
  *kwd = kwStart;
  return 0;
}

int setCtxObservation(observationContext * o, char *txt)
{
  if (*txt)
    return errorPSV("text not allowed on observation line");

  ctxObservation *obs = calloc(1, sizeof(ctxObservation));
  o->observation = obs;
  _Bool haveCount = 0;
  _Bool haveType = 0;
  char *kwd;
  while (1) {
    int r = getPSVLine();
    if (r != 0)
      return r;
    if (*line != '!')
      return 0;
    r = parseHdr(&kwd, &txt);
    if (r != 0)
      return r;
    if (!strcmp(kwd, "count")) {
      if (haveCount)
        return errorPSV("multiple count lines");
      obs->count = strdup(txt);
      haveCount = 1;
    } else if (!strcmp(kwd, "type")) {
      if (haveType)
        return errorPSV("multiple type lines");
      obs->type = strdup(txt);
      haveType = 1;
    } else
      return errorPSV("unknown element for observation group");
  }
  return 0;
}

int setCtxObservatory(observationContext * o, char *txt)
{
  if (*txt)
    return errorPSV("text not allowed on observatory line");

  ctxObservatory *obs = calloc(1, sizeof(ctxObservatory));
  o->observatory = obs;
  _Bool haveCode = 0;
  _Bool haveName = 0;
  char *kwd;
  while (1) {
    int r = getPSVLine();
    if (r != 0)
      return r;
    if (*line != '!')
      return 0;
    r = parseHdr(&kwd, &txt);
    if (r != 0)
      return r;
    if (!strcmp(kwd, "mpcCode")) {
      if (haveCode)
        return errorPSV("multiple mpcCode lines");
      obs->mpcCode = strdup(txt);
      haveCode = 1;
    } else if (!strcmp(kwd, "name")) {
      if (haveName)
        return errorPSV("multiple name lines");
      obs->name = strdup(txt);
      haveName = 1;
    } else
      return errorPSV("unknown element for observatory group");
  }
  return 0;
}

int setCtxContact(observationContext * o, char *txt)
{
  if (*txt)
    return errorPSV("text not allowed on contact line");

  ctxContact *con = calloc(1, sizeof(ctxContact));
  o->contact = con;
  _Bool haveName = 0;
  _Bool haveAddress = 0;
  _Bool haveAckMsg = 0;
  _Bool haveAckEmail = 0;
  char *kwd;
  while (1) {
    int r = getPSVLine();
    if (r != 0)
      return r;
    if (*line != '!')
      return 0;
    r = parseHdr(&kwd, &txt);
    if (r != 0)
      return r;
    if (!strcmp(kwd, "name")) {
      if (haveName)
        return errorPSV("multiple name lines");
      con->name = strdup(txt);
      haveName = 1;
    } else if (!strcmp(kwd, "address")) {
      if (haveAddress)
        return errorPSV("multiple address lines");
      con->address = strdup(txt);
      haveAddress = 1;
    } else if (!strcmp(kwd, "ackMessage")) {
      if (haveAckMsg)
        return errorPSV("multiple ackMessage lines");
      con->ackMessage = strdup(txt);
      haveAckMsg = 1;
    } else if (!strcmp(kwd, "ackEmail")) {
      if (haveAckEmail)
        return errorPSV("multiple ackEmail lines");
      con->ackEmail = strdup(txt);
      haveAckEmail = 1;
    } else
      return errorPSV("unknown element for observatory group");
  }
  return 0;
}

int parseNameList(ctxNameList ** pnl, char *txt, char *h1)
{
  if (*txt)
    return errorPSV1("text not allowed on %s line", h1);
  ctxNameList *nl = calloc(1, sizeof(ctxNameList));
  char *kwd;
  while (1) {
    int r = getPSVLine();
    if (r != 0)
      return r;
    if (*line != '!')
      break;
    r = parseHdr(&kwd, &txt);
    if (r != 0)
      return r;
    if (strcmp(kwd, "name")) {
      return errorPSV1("unknown element for %s list", h1);
    }
    int last = nl->len;
    nl->len++;
    nl->names = realloc(nl->names, nl->len * sizeof(dVal));
    nl->names[last] = strdup(txt);
  }
  *pnl = nl;
  return 0;
}

int setCtxObservers(observationContext * o, char *txt)
{
  ctxNameList *nl = NULL;
  int r = parseNameList(&nl, txt, "observers");
  if (r)
    return r;
  o->observers = nl;
  return 0;
}

int setCtxMeasurers(observationContext * o, char *txt)
{
  ctxNameList *nl;
  int r = parseNameList(&nl, txt, "measurers");
  if (r)
    return r;
  o->measurers = nl;
  return 0;
}

int setCtxTelescope(observationContext * o, char *txt)
{
  if (*txt)
    return errorPSV("text not allowed on telescope line");

  ctxTelescope *tel = calloc(1, sizeof(ctxTelescope));
  o->telescope = tel;
  _Bool haveName = 0;
  _Bool haveDes = 0;
  _Bool haveAp = 0;
  _Bool haveDet = 0;
  _Bool haveF = 0;
  _Bool haveFilter = 0;
  _Bool haveArray = 0;
  _Bool havePix = 0;
  char *kwd;
  while (1) {
    int r = getPSVLine();
    if (r != 0)
      return r;
    if (*line != '!')
      return 0;
    r = parseHdr(&kwd, &txt);
    if (r != 0)
      return r;
    if (!strcmp(kwd, "name")) {
      if (haveName)
        return errorPSV("multiple name lines");
      tel->name = strdup(txt);
      haveName = 1;
    } else if (!strcmp(kwd, "design")) {
      if (haveDes)
        return errorPSV("multiple design lines");
      tel->design = strdup(txt);
      haveDes = 1;
    } else if (!strcmp(kwd, "aperture")) {
      if (haveAp)
        return errorPSV("multiple aperture lines");
      tel->aperture = strdup(txt);
      haveAp = 1;
    } else if (!strcmp(kwd, "detector")) {
      if (haveDet)
        return errorPSV("multiple detector lines");
      tel->detector = strdup(txt);
      haveDet = 1;
    } else if (!strcmp(kwd, "fRatio")) {
      if (haveF)
        return errorPSV("multiple fRatio lines");
      tel->fRatio = strdup(txt);
      haveF = 1;
    } else if (!strcmp(kwd, "filter")) {
      if (haveFilter)
        return errorPSV("multiple filter lines");
      tel->filter = strdup(txt);
      haveFilter = 1;
    } else if (!strcmp(kwd, "arraySize")) {
      if (haveArray)
        return errorPSV("multiple arraySize lines");
      tel->arraySize = strdup(txt);
      haveArray = 1;
    } else if (!strcmp(kwd, "pixelScale")) {
      if (havePix)
        return errorPSV("multiple pixelScale lines");
      tel->pixelScale = strdup(txt);
      havePix = 1;
    } else
      return errorPSV("unknown element for observatory group");
  }
  return 0;
}

int setCtxSoftware(observationContext * o, char *txt)
{
  if (*txt)
    return errorPSV("text not allowed on software line");

  ctxSoftware *sw = calloc(1, sizeof(ctxSoftware));
  o->software = sw;
  _Bool haveAst = 0;
  _Bool haveFit = 0;
  _Bool havePhot = 0;
  _Bool haveDet = 0;
  char *kwd;
  while (1) {
    int r = getPSVLine();
    if (r != 0)
      return r;
    if (*line != '!')
      return 0;
    r = parseHdr(&kwd, &txt);
    if (r != 0)
      return r;
    if (!strcmp(kwd, "astrometry")) {
      if (haveAst)
        return errorPSV("multiple astrometry lines");
      sw->astrometry = strdup(txt);
      haveAst = 1;
    } else if (!strcmp(kwd, "fitOrder")) {
      if (haveFit)
        return errorPSV("multiple fitOrder lines");
      sw->fitOrder = strdup(txt);
      haveFit = 1;
    } else if (!strcmp(kwd, "photometry")) {
      if (havePhot)
        return errorPSV("multiple photometry lines");
      sw->photometry = strdup(txt);
      havePhot = 1;
    } else if (!strcmp(kwd, "objectDetection")) {
      if (haveDet)
        return errorPSV("multiple objectDetection lines");
      sw->objectDetection = strdup(txt);
      haveDet = 1;
    } else
      return errorPSV("unknown element for software group");
  }
  return 0;
}

int setCtxComment(observationContext * o, char *txt)
{
  if (*txt)
    o->comment = strdup(txt);
  return getPSVLine();
}

int setCtxCoinvestigators(observationContext * o, char *txt)
{
  ctxNameList *nl;
  int r = parseNameList(&nl, txt, "coinvestigators");
  if (r != 0)
    return r;
  o->coinvestigators = nl;
  return 0;
}

int setCtxCollaborators(observationContext * o, char *txt)
{
  ctxNameList *nl;
  int r = parseNameList(&nl, txt, "collaborators");
  if (r != 0)
    return r;
  o->collaborators = nl;
  return 0;
}

int setCtxFundingSource(observationContext * o, char *txt)
{
  if (*txt)
    o->fundingSource = strdup(txt);
  return getPSVLine();
}

int setCtxOrbProd(observationContext * o, char *txt)
{
  if (*txt)
    o->orbProd = strdup(txt);
  return getPSVLine();
}

int setCtxPhotProd(observationContext * o, char *txt)
{
  if (*txt)
    o->photProd = strdup(txt);
  return getPSVLine();
}

typedef int (*ctxSetter) (observationContext *, char *);
ctxSetter setCtx[] = {
  setCtxObservation,
  setCtxObservatory,
  setCtxContact,
  setCtxObservers,
  setCtxMeasurers,
  setCtxTelescope,
  setCtxSoftware,
  setCtxComment,
  setCtxCoinvestigators,
  setCtxCollaborators,
  setCtxFundingSource,
  setCtxOrbProd,
  setCtxPhotProd,
};

// pxHeader
//
// on entry `line` contains a top level (#) header line.
// parse the observation header into the tree, skip blank lines,
// on non-header, non-blank line, call pxObs.
int pxHeader(observationContext * ctx)
{
  char *kwd;
  char *txt;
  while (1) {
    int r = parseHdr(&kwd, &txt);
    if (r != 0)
      return r;
    int kx;
    for (kx = 0; strcmp(kwd, H1Names[kx]);) {
      if (++kx == nH1Names)
        return errorPSV("unknown header keyword");
    }
    r = setCtx[kx] (ctx, txt);
    if (r != 0)
      return r;
    if (*line != '#')
      break;
  }
  return 0;
}

// return 0 for no error, non-zero for error
int psNoTree(char *fn, observationBatch ** o)
{
  fpsv = fopen(fn, "r");
  if (!fpsv)
    return error1("can't open %s", fn);
  int r = getPSVLine();
  if (r != 0)
    return r;
  if (!*line)
    return error1("file %s empty", fn);

  observationBatch *b = calloc(1, sizeof(observationBatch));
  *o = b;
  int len = 0;
  int cap = 1;
  observationSegment *seg = malloc(cap * sizeof(observationSegment));
  while (1) {
    if (len == cap) {
      cap *= 2;
      seg = realloc(seg, cap * sizeof(observationSegment));
    }
    observationSegment *s = seg + len;
    if (*line == '#') {
      s->obs = 0;
      s->ctx = calloc(1, sizeof(observationContext));
      r = pxHeader(s->ctx);
    } else {
      s->ctx = 0;
      s->obs = calloc(1, sizeof(obsList));
      r = pxObs(s->obs);
    }
    if (r != 0)
      return r;
    ++len;                      // segment complete
    if (!*line)
      break;
  }
  if (len < cap)
    seg = realloc(seg, len * sizeof(observationSegment));
  b->len = len;
  b->segments = seg;
  return 0;
}

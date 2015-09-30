#define _GNU_SOURCE
#include <string.h>

#include <libxml/tree.h>

#include <alerr.h>
#include <ds.h>
#include <tables.h>

// globals
FILE *fpsv;
int lineNum;
char line[512];                 // buffer for holding a single line of PSV
char line2[512];                // for copies of line
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
// category Z? related functions is libxml2? in libunistring?)
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
int trimRight(char *p, size_t * trimmed)
{
  char *wsStart = NULL;
  int all = strlen(p);
  int c, len;
  while (*p) {
    len = all;
    c = xmlGetUTF8Char(p, &len);
    if (!len)
      return errorPSV("invalid UTF-8");
    if (xmlStrchr(LATIN1_WS, c)) {
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
  int all = strlen(p);
  int c, len;
  while (*p) {
    len = all;
    c = xmlGetUTF8Char(p, &len);
    if (!len)
      return errorPSV("invalid UTF-8");
    // reversed test is only difference from nonWSEnd
    if (!xmlStrchr(LATIN1_WS, c))
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
  int all = strlen(p);
  int c, len;
  while (*p) {
    len = all;
    c = xmlGetUTF8Char(p, &len);
    if (!len)
      return errorPSV("invalid UTF-8");
    // reversed test is only difference from firstNonWS
    if (xmlStrchr(LATIN1_WS, c))
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
    // trimRight also validates UTF-8.
    trimRight(p, &w);           // w unused
  }
  while (!*p);                  // loop on blank lines
  return 0;
}

// On entry `line` must contain the PSV column headers
int splitColHdrs(int *nCols, int **pCols)
{
  // count pipes first and allocate array for headers
  int nPipes = 0;
  for (const char *p = line; p = xmlStrchr(p, '|'); p++)
    nPipes++;
  *nCols = nPipes + 1;
  char **colHdrs = malloc(*nCols * sizeof(char *));

  // scan again, filling header array
  char *cs = strdup(line);
  colHdrs[0] = cs;
  int i = 1;
  // (cast away const)
  for (char *p = cs; p = (char *)xmlStrchr(p, '|');) {
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
    if (r)
      return r;
    if (!*p)
      return errorPSV("empty column header");
    for (j = 0; strcmp(p, fldNames[j]);)
      if (++j == nFlds) {
        snprintf(line2, sizeof line2,
                 "unknown field used as column header: %s", p);
        return errorPSV(line2);
      }
    cols[i] = j;
  }
  *pCols = cols;
  return 0;
}

// On entry `line` must contain the PSV column headers
int pxObs()
{
  int nCols;
  int *fldNums;
  int r = splitColHdrs(&nCols, &fldNums);
  if (r)
    return r;
  if (fldNums[0])
    return errorPSV("first column must be permID");

  xmlNodePtr obsList = xmlNewChild(root_node, NULL, "observations", NULL);
  obsList->line = lineNum;
  while (1) {
 nextLine:
    r = getPSVLine();
    if (r)
      return r;
    if (!*line || *line == '#')
      break;
    strcpy(line2, line);        // save a copy

    char *fld = line;
    xmlNodePtr obs;
    for (int col = 0;; col++) {
      if (col >= nCols)
        return errorPSV("more fields than column headers");
      char *end = (char *)xmlStrchr(fld, '|');
      if (end)
        *end++ = 0;
      r = trim(fld, &fld);
      if (r)
        return r;

      // special handling at first field:
      if (col == 0) {
        if (isFldName(fld)) {
          if (strcmp(fld, "permID"))
            return errorPSV("first column must be permID");
          strcpy(line, line2);  // restore line (we punched holes in it)
          // reparse it as headers
          r = splitColHdrs(&nCols, &fldNums);
          if (r)
            return r;
          // break column loop, continue with next PSV line
          goto nextLine;
        }
        // normal case: start a new observation.
        // assume optical until mode Radar is found
        obs = xmlNewChild(obsList, NULL, BAD_CAST "optical", NULL);
        obs->line = lineNum;
      }

      int fldNum = fldNums[col];
      if (fldNum == F_MODE && !strcmp(fld, "Radar"))
        xmlNodeSetName(obs, "radar");
      if (*fld)
        xmlNewChild(obs, NULL, fldNames[fldNum], fld)->line = lineNum;
      if (!end) {
        if (col + 1 < nCols)
          return errorPSV("fewer data fields than column headers");
        break;                  // normal end of record
      }
      fld = end;
    }
  }
}

// handle a single header line, either a # or ! line.
int addHdr(xmlNodePtr parent, xmlNodePtr * hdr)
{
  // find keyword start.  first byte of line is known to be # or !,
  // white space can follow, then a keyword must start.
  char *kwStart;
  int r = firstNonWS(line + 1, &kwStart);
  if (r)
    return r;
  if (!*kwStart)
    return errorPSV("missing header keyword");

  // find keyword end, copy to buffer
  char *kwEnd;
  char *txt;
  r = nonWSEnd(kwStart, &kwEnd);
  if (r)
    return r;
  if (!*kwEnd)
    txt = kwEnd;                // set txt to empty string
  else {
    firstNonWS(kwEnd, &txt);
    *kwEnd = 0;
  }
  xmlNodePtr h = xmlNewChild(parent, NULL, kwStart, *txt
                             ? xmlEncodeEntitiesReentrant(doc, txt)
                             : NULL);
  h->line = lineNum;
  *hdr = h;
  return 0;
}

// pxHeader
//
// on entry `line` contains a top level (#) header line.
// parse the observation header into the tree, skip blank lines,
// on non-header, non-blank line, call pxObs.
int pxHeader()
{
  xmlNodePtr ctx = xmlNewChild(root_node, NULL,
                               "observationContext", NULL);
  ctx->line = lineNum;
  xmlNodePtr h1;
  int r = addHdr(ctx, &h1);     // top level header
  if (r)
    return r;
  h1->line = lineNum;
  xmlNodePtr h2;
  while (1) {
    r = getPSVLine();
    if (r)
      return r;
    if (!*line)
      break;
    if (*line == '!') {
      r = addHdr(h1, &h2);
      if (r)
        return r;
      h2->line = lineNum;
    } else if (line[0] == '#') {
      r = addHdr(ctx, &h1);
      if (r)
        return r;
      h1->line = lineNum;
    } else
      return pxObs();
  }
}

// pt
//
int pt(char *fn, xmlDocPtr * pDoc)
{
  fpsv = fopen(fn, "r");
  if (!fpsv) {
    return error1("can't open PSV file %s", fn);
  }
  int r = getPSVLine();
  if (r)
    return r;
  if (!*line)
    return error1("PSV file %s empty or unreadable", fn);

  doc = xmlNewDoc("1.0");
  root_node = xmlNewNode(NULL, "observationBatch");
  root_node->line = 1;
  xmlDocSetRootElement(doc, root_node);

  do {
    if (*line == '#')
      pxHeader();
    else
      pxObs();
    r = getPSVLine();
    if (r)
      return r;
  } while (*line);

  *pDoc = doc;
  return 0;
}

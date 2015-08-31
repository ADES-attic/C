#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>

#include <libxml/tree.h>

// globals
FILE *fpsv;
int lineNum;
char line[512];
char kw[31];
xmlDocPtr doc;
xmlNodePtr root_node;

// fatal - print message to stdout and terminate program.
//
// fatal always adds a newline.
// typically msg should not have a trailing newine.
void fatal(char *msg)
{
  fputs(msg, stderr);
  fputc('\n', stderr);
  exit(-1);
}

void fatal1(char *msg, char *arg)
{
  char *m;
  asprintf(&m, msg, arg);
  fatal(m);
}

void fatalPSV(char *msg)
{
  char *m;
  asprintf(&m, "PSV line %d: %s", lineNum, msg);
  fatal(m);
}

// TODO figure out the rest of Unicode whitespace. (white space property?
// category Z? related functions is libxml2? in libunistring?)
#define LATIN1_WS "\t\n\v\f\r \x85\xA0"

// trimRight trims white space from the right of a string, by punching a
// null terminator byte.
//
// arg is an xmlChar*.  trimRight scans the entire string; any non UTF-8 is
// detected and is a fatal error.
//
// return value is the number of whitespace bytes trimed.
size_t trimRight(xmlChar * p)
{
  xmlChar *wsStart = NULL;
  int all = strlen(p);
  int c, len;
  while (*p) {
    len = all;
    c = xmlGetUTF8Char(p, &len);
    if (!len)
      fatalPSV("invalid UTF-8");
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
    return 0;
  *wsStart = 0;
  return p - wsStart;
}

// firstNonWS returns a pointer to the first non-whitespace character in p.
//
// p must be a null terminated string.
char *firstNonWS(char *p)
{
  int all = strlen(p);
  int c, len;
  while (*p) {
    len = all;
    c = xmlGetUTF8Char(p, &len);
    if (!xmlStrchr(LATIN1_WS, c))
      break;
    p += len;
    all -= len;
  }
  return p;
}

// trim trims both leading and trailing whitespace.
//
// a pointer to first non-whitespace character is returned.
// a null is punched to the last whitespace character from the end.
char *trim(char *s)
{
  trimRight(s);
  return firstNonWS(s);
}

// get non-blank line, validate UTF-8, trim trailing space.
char *getPSVLine()
{
  char *p;
  do {
    // TODO fgets bad, getline (GNU) better?
    p = fgets(line, sizeof(line), fpsv);
    if (!p)
      break;
    lineNum++;
    trimRight(p);               // trimRight also validates UTF-8.
  }
  while (!*p);
  return p;
}

// On entry `line` must contain the PSV column headers
char **splitColHdrs(int *nCols)
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
  // cast away const
  for (char *p = cs; p = (char *)xmlStrchr(p, '|');) {
    *p++ = 0;
    colHdrs[i++] = p;
  }

  // trim headers, catch blank headers
  for (int i = 0; i < *nCols; i++) {
    char *p = trim(colHdrs[i]);
    if (!*p)
      fatalPSV("empty column header");
    colHdrs[i] = p;
  }

  printf("%d headers:\n", *nCols);
  for (int i = 0; i < *nCols; i++)
    printf("  <%s>\n", colHdrs[i]);
  return colHdrs;
}

// On entry `line` must contain the PSV column headers
void pxObs()
{
  int nCols;
  char **colHdrs = splitColHdrs(&nCols);
}

// handle a single header line, either a # or ! line.
xmlNodePtr addHdr(xmlNodePtr parent)
{
  // find keyword start.  first byte of line is known to be # or !,
  // white space can follow, then a keyword must start.
  char *kwStart = firstNonWS(line + 1);
  if (!*kwStart)
    fatalPSV("missing header keyword");

  // find keyword end, copy to buffer
  char *kwEnd = kwStart;
  size_t all = strlen(kwEnd);
  int len;
  while (*kwEnd) {
    len = all;
    int c = xmlGetUTF8Char(kwEnd, &len);
    if (xmlStrchr(LATIN1_WS, c))
      break;
    kwEnd += len;
    all -= len;
  }
  size_t kwLen = kwEnd - kwStart;
  if (kwLen > sizeof(kw) - 1)
    fatalPSV("long keyword");
  strncpy(kw, kwStart, kwLen);
  kw[kwLen] = 0;
  char *txt = firstNonWS(kwEnd);
  return xmlNewChild(parent, NULL, BAD_CAST kw, *txt
                     ? xmlEncodeEntitiesReentrant(doc, txt)
                     : NULL);
}

// pxHeader
//
// on entry `line` contains a top level (#) header line.
// parse the observation header into the tree, skip blank lines,
// on non-header, non-blank line, call pxObs.
void pxHeader()
{
  xmlNodePtr hdr = xmlNewChild(root_node, NULL,
                               BAD_CAST "observationContext", NULL);
  xmlNodePtr h1 = addHdr(hdr);  // top level header
  while (getPSVLine()) {
    if (line[0] == '!')
      addHdr(h1);
    else if (line[0] == '#')
      h1 = addHdr(hdr);
    else {
      pxObs();
      break;
    }
  }
}

// px
//
// top level loop, called after .psv is open and root_node is created.
//
// loop:
//   skip blank lines
//   if '#', start batch, parse header
//   else start batch, parse obs
void px()
{
  if (!getPSVLine())
    fatal("can't read psv");
  do {
    if (*line == '#')
      pxHeader();
    else
      pxObs();
  }
  while (getPSVLine());
}

// main
//
// usage: px <input .psv file> <output .xml file>
int main(int argc, char **argv)
{
  LIBXML_TEST_VERSION;
  if (argc != 3) {
    fatal("usage: px <input .psv file> <output .xml file>");
  }
  fpsv = fopen(argv[1], "r");
  if (!fpsv) {
    fatal1("can't open %s", argv[1]);
  }

  doc = xmlNewDoc(BAD_CAST "1.0");
  root_node = xmlNewNode(NULL, BAD_CAST "observationBatch");
  xmlDocSetRootElement(doc, root_node);

  px();

  xmlSaveFormatFileEnc(argv[2], doc, "UTF-8", 1);
}

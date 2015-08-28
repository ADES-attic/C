#include <libxml/tree.h>
#include <string.h>

// globals
FILE *fpsv;
int lineNum;
xmlDocPtr doc;
xmlNodePtr root_node;
char line[512];
char kw[31];

// fatal - print message to stdout and terminate program.
//
// fatal always adds a newline.  typically msg should not have a trailing
// newine.
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

#define LATIN1_WS "\t\n\v\f\r \x85\xA0"
// TODO figure out the rest of Unicode whitespace. (white space property?
// category Z? related functions is libxml2? in libunistring?)

// trimRight trims white space from the right of a string, by punching a
// null terminator byte.
//
// arg is an xmlChar*.  trimRight scans the entire string; any non UTF-8 is
// detected and is a fatal error.
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

void pxObs()
{
}

// handle a single header line, either a # or ! line.
xmlNodePtr addHdr(xmlNodePtr parent)
{
  // find keyword start.  first byte of line is known to be # or !,
  // white space can follow, then a keyword must start.
  char *kwStart = line + 1;
  int all = strlen(kwStart);
  int c, len;
  while (1) {
    if (!*kwStart)
      fatalPSV("missing header keyword");
    len = all;
    c = xmlGetUTF8Char(kwStart, &len);
    if (!xmlStrchr(LATIN1_WS, c))
      break;
    kwStart += len;
    all -= len;
  }

  // find keyword end, copy to buffer
  char *kwEnd = kwStart + len;
  all -= len;
  while (*kwEnd) {
    c = xmlGetUTF8Char(kwEnd, &len);
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

  // advance past whitespace to text following keyword
  while (*kwEnd) {
    c = xmlGetUTF8Char(kwEnd, &len);
    if (!xmlStrchr(LATIN1_WS, c))
      break;
    kwEnd += len;
    all -= len;
  }

  return xmlNewChild(parent, NULL, BAD_CAST kw, *kwEnd
                     ? xmlEncodeEntitiesReentrant(doc, kwEnd)
                     : NULL);
}

// pxHeader
//
// on entry `line` contains a header line.
// parse the observation header into the tree, skip blank lines,
// on non-header, non-blank line, call pxObs.
void pxHeader()
{
  xmlNodePtr hdr = xmlNewChild(root_node, NULL,
                               BAD_CAST "observationContext", NULL);
  xmlNodePtr h1 = addHdr(hdr);
  while (getPSVLine()) {
    if (line[0] == '!')
      addHdr(h1);
    else if (line[0] == '#')
      h1 = addHdr(hdr);
    else
      break;
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
    else if (*line != 0)
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

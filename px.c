#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <libxml/tree.h>
#include <libxml/xmlschemas.h>

// globals
FILE *fpsv;
int lineNum;
char line[512];                 // buffer for holding a single line of PSV
char line2[512];                // for copies of line, or for formatting fatal mesages
char kw[31];
xmlDocPtr doc;
xmlNodePtr root_node;
xmlSchemaValidCtxtPtr schemaCtx = NULL;

char *fldNames[] = {
  "permID",
  "provID",
  "trkSub",
  "obsID",
  "trkID",
  "mode",
  "stn",
  "prg",
  "obsTime",
  "ra",
  "dec",
  "deltaRA",
  "deltaDec",
  "raStar",
  "decStar",
  "frame",
  "astCat",
  "rmsRA",
  "rmsDec",
  "rmsCorr",
  "mag",
  "band",
  "photCat",
  "rmsMag",
  "photAp",
  "nucMag",
  "logSNR",
  "seeing",
  "exp",
  "rmsFit",
  "nStars",
  "ref",
  "disc",
  "subFmt",
  "precTime",
  "precRA",
  "precDec",
  "uncTime",
  "notes",
  "remarks",
  "sys",
  "ctr",
  "pos1",
  "pos2",
  "pos3",
  "posCov11",
  "posCov12",
  "posCov13",
  "posCov22",
  "posCov23",
  "posCov33",
  "valRad",
  "rmsRad",
  "com",
  "frq",
  "trx",
  "rcv",
  "orbProd",
  "photProd",
  "resRA",
  "resDec",
  "orbID",
  "selAst",
  "sigRA",
  "sigDec",
  "sigCorr",
  "sigTime",
  "biasRA",
  "biasDec",
  "biasTime",
  "resMag",
  "selPhot",
  "sigMag",
  "biasMag",
  "photMod",
  "resRad",
  "selRad",
  "sigRad"
};

int nFlds = sizeof fldNames / sizeof *fldNames;

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
  // note similar code in nonWSEnd
  int all = strlen(p);
  int c, len;
  while (*p) {
    len = all;
    c = xmlGetUTF8Char(p, &len);
    // reversed test is only difference from nonWSEnd
    if (!xmlStrchr(LATIN1_WS, c))
      break;
    p += len;
    all -= len;
  }
  return p;
}

// nonWSEnd returns a pointer to the first whitespace character or the
// the terminating null.
char *nonWSEnd(char *p)
{
  // note similar code in firstNonWS
  int all = strlen(p);
  int c, len;
  while (*p) {
    len = all;
    c = xmlGetUTF8Char(p, &len);
    // reversed test is only difference from firstNonWS
    if (xmlStrchr(LATIN1_WS, c))
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
  // (cast away const)
  for (char *p = cs; p = (char *)xmlStrchr(p, '|');) {
    *p++ = 0;
    colHdrs[i++] = p;
  }

  // trim headers, catch blank headers, validate against fldNames
  for (int i = 0; i < *nCols; i++) {
    char *p = trim(colHdrs[i]);
    if (!*p)
      fatalPSV("empty column header");
    for (int j = 0; strcmp(p, fldNames[j]);)
      if (++j == nFlds) {
        snprintf(line, sizeof line,
                 "unknown field used as column header: %s", p);
        fatalPSV(line);
      }
    colHdrs[i] = p;
  }
  return colHdrs;
}

// On entry `line` must contain the PSV column headers
void pxObs()
{
  int nCols;
  char **colHdrs = splitColHdrs(&nCols);
  xmlNodePtr obsList = xmlNewChild(root_node, NULL,
                                   BAD_CAST "observations", NULL);
  obsList->line = lineNum;
  while (getPSVLine() && *line != '#') {
    strcpy(line2, line);        // save a copy
    char *fld = line;
    xmlNodePtr obs;
    for (int col = 0;; col++) {
      if (col >= nCols)
        fatalPSV("more fields than column headers");
      char *end = (char *)xmlStrchr(fld, '|');
      if (end)
        *end++ = 0;
      fld = trim(fld);

      // special handling at first field:
      if (col == 0) {
        // if first field is some sort of ID,
        if (!strcmp(fld, fldNames[0]) || // permID
            !strcmp(fld, fldNames[1]) || // provID
            !strcmp(fld, fldNames[2]) || // trkSub
            !strcmp(fld, fldNames[3]) || // obsID
            !strcmp(fld, fldNames[4])) { // trkID
          strcpy(line, line2);  // restore line (we punched holes in it)
          colHdrs = splitColHdrs(&nCols); // parse it as headers
          // break column loop, continue with next PSV line
          break;
        }
        // normal case: start a new observation.
        // assume optical until mode Radar is found
        obs = xmlNewChild(obsList, NULL, BAD_CAST "optical", NULL);
        obs->line = lineNum;
      }

      if (*fld)
        xmlNewChild(obs, NULL, colHdrs[col], fld)->line = lineNum;
      if (!strcmp(colHdrs[col], "mode") && !strcmp(fld, "Radar"))
        xmlNodeSetName(obs, "radar");
      if (!end)
        break;
      fld = end;
    }
  }
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
  char *kwEnd = nonWSEnd(kwStart);
  size_t kwLen = kwEnd - kwStart;
  if (kwLen > sizeof(kw) - 1)
    fatalPSV("long keyword");
  strncpy(kw, kwStart, kwLen);
  kw[kwLen] = 0;
  char *txt = firstNonWS(kwEnd);
  xmlNodePtr h = xmlNewChild(parent, NULL, BAD_CAST kw, *txt
                             ? xmlEncodeEntitiesReentrant(doc, txt)
                             : NULL);
  h->line = lineNum;
  return h;
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
  hdr->line = lineNum;
  xmlNodePtr h1 = addHdr(hdr);  // top level header
  while (getPSVLine()) {
    if (line[0] == '!') {
      xmlNodePtr h2 = addHdr(h1);
      h2->line = lineNum;
    } else if (line[0] == '#')
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
  char *schema = NULL;
  int oc = getopt(argc, argv, "s:");
  if (oc == '?')
    exit(-1);                   // getopt already emitted err msg
  if (oc > 0)
    schema = optarg;
  if (argc - optind != 2) {
    fatal("usage: px [-s schema] <input .psv file> <output .xml file>");
  }
  fpsv = fopen(argv[optind], "r");
  if (!fpsv) {
    fatal1("can't open %s", argv[optind]);
  }

  if (schema) {
    xmlSchemaParserCtxtPtr pCtx = xmlSchemaNewParserCtxt(schema);
    if (!pCtx)
      exit(-1);                 // xml functions emit err msgs
    xmlSchemaPtr sPtr = xmlSchemaParse(pCtx);
    if (!sPtr)
      exit(-1);
    schemaCtx = xmlSchemaNewValidCtxt(sPtr);
    if (!schemaCtx)
      exit(-1);
  }

  doc = xmlNewDoc(BAD_CAST "1.0");
  root_node = xmlNewNode(NULL, BAD_CAST "observationBatch");
  root_node->line = 1;
  xmlDocSetRootElement(doc, root_node);
  px();

  if (schema && xmlSchemaValidateDoc(schemaCtx, doc))
    exit(-1);

  xmlSaveFormatFileEnc(argv[optind + 1], doc, "UTF-8", 1);
}

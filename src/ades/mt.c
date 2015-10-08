//
// mt.c -- (M)PC 80 column to libxml2 (T)ree conversion.
//
// Implements function mt().
//
// This function intended for use primarily within the ades library and not
// as an API function.  The result is a libxml2 xmlDocPtr and the idea is that
// API users should not be required to interpret libxml2 data types.

#define _GNU_SOURCE
#include <regex.h>
#include <string.h>
#include <sys/stat.h>

#include <globals.h>

FILE *fobs;
regex_t rxHdr;
_Bool rxHdrCompiled;
regex_t rxTel;
_Bool rxTelCompiled;
regex_t rxPermMP;
regex_t rxPermComet;
regex_t rxPermNatSat;
regex_t rxProvMP;
regex_t rxProvCometNatSat;
regex_t rxCometLetter;
_Bool rxDesigCompiled;

xmlNodePtr contactNode;
xmlNodePtr observersNode;
xmlNodePtr measurersNode;
_Bool contactNameSet;
char *contactAddress;
char *ackEmail;
char *net;
char *bnd;
char *com;

// note mtFileError functions also use globals from globals.c
int mtFileError(char *msg)
{
  snprintf(errLine, sizeof errLine, "Obs file line %d: %s", lineNum, msg);
  return -1;
}

int mtFileError1(char *msg, char *arg)
{
  int n = snprintf(errLine, sizeof errLine, "Obs file line %d: ", lineNum);
  snprintf(errLine + n, sizeof errLine - n, msg, arg);
  return -1;
}

#define ASCII_WS " \n\r\t\f\v"

// trim trailing space
void mtTrimRight(char *p)
{
  char *q = strchr(p, 0);
  while (q != p && strchr(ASCII_WS, *--q))
    *q = 0;
}

// get non-blank line, trim trailing space.
//
// on success global `line` contains a non-empty string and  0 is returned.
//
// at end of file, *line is set to NULL, and 0 is still returned.
//
// A non-zero return value means some other error, such as a line too long.
int mtGetLine()
{
  char *p;
  do {
    if (!(p = fgets(line, sizeof(line), fobs))) {
      *line = 0;
      break;
    }
    if (strlen(p) == sizeof line - 1)
      return mtFileError("line too long");
    lineNum++;
    mtTrimRight(line);          // trim trailing space
  }
  while (!*p);                  // loop on blank lines
  return 0;
}

_Bool mtMatchHdr()
{
  return !regexec(&rxHdr, line, 0, NULL, 0);
}

xmlNodePtr newChild(xmlNodePtr parent, char *name, char *val)
{
  xmlNodePtr n = xmlNewChild(parent, NULL, name, val);
  n->line = lineNum;
  return n;
}

int mtCOD(char *val, xmlNodePtr ctx)
{
  xmlNodePtr obs = newChild(ctx, "observatory", NULL);
  newChild(obs, "mpcCode", val);
  return 0;
}

int mtCON(char *val, xmlNodePtr ctx)
{
  if (!contactNode)
    contactNode = newChild(ctx, "contact", NULL);

  char *lb, *rb;
  if ((lb = strchr(val, '[')) && (rb = strchr(lb, ']'))) {
    *rb = 0;
    if (ackEmail)
      asprintf(&ackEmail, "%s,%s", lb + 1, ackEmail);
    else
      ackEmail = strdup(lb + 1);
    memmove(lb, rb + 1, strlen(rb + 1) + 1);
    mtTrimRight(val);           // trim again after excising email
  }
  if (!*val)
    return 0;

  if (!contactNameSet) {
    char *c = strchr(val, ',');
    if (c)
      *c = 0;
    newChild(contactNode, "name", val);
    contactNameSet = 1;
    if (!c)
      return 0;
    val = c + 1;
    if (*val == ' ')
      val++;
  }
  if (contactAddress)
    asprintf(&contactAddress, "%s, %s", contactAddress, val);
  else
    contactAddress = strdup(val);
  return 0;
}

int mtNameList(char *val, xmlNodePtr parent)
{
  char *c;
  while (*val) {
    while (strchr(ASCII_WS, *val))
      val++;
    if (!*val)
      break;
    if (c = strchr(val, ',')) {
      *c = 0;
      mtTrimRight(val);
      if (!*val) {
        val = c + 1;
        continue;
      }
    }
    newChild(parent, "name", val);
    if (!c)
      break;
    val = c + 1;
  }
  return 0;
}

int mtOBS(char *val, xmlNodePtr ctx)
{
  if (!observersNode)
    observersNode = newChild(ctx, "observers", NULL);
  return mtNameList(val, observersNode);
}

int mtMEA(char *val, xmlNodePtr ctx)
{
  if (!measurersNode)
    measurersNode = newChild(ctx, "measurers", NULL);
  return mtNameList(val, measurersNode);
}

void mtDesign(char *design, xmlNodePtr tel)
{
  FILE *f = fopen("telescopes.csv", "r");
  if (!f)
    goto des;
  struct stat st;
  if (fstat(fileno(f), &st))
    goto des;
  char *buf = malloc(st.st_size + 1);
  fread(buf, st.st_size, 1, f);
  buf[st.st_size] = 0;
  fclose(f);
  char *p;
  int dl = strlen(design);
  for (p = buf; (p = strstr(p, design)); p += dl) {
    if (p != buf && p[-1] != '\n') // must start at beginning of line
      continue;
    if (p[dl] != ',')           // must end at comma
      continue;
    // design argument matches telescope name, actual design follows comma.
    p += dl + 1;
    char *q = strchr(p, '\n');
    if (q)
      *q = 0;
    newChild(tel, "name", design);
    newChild(tel, "design", p);
    return;
  }
 des:
  newChild(tel, "design", design);
}

int mtTEL(char *val, xmlNodePtr ctx)
{
  xmlNodePtr tel = newChild(ctx, "telescope", NULL);
  if (!rxTelCompiled) {
    int r = regcomp(&rxTel, "^\
([.0-9]+)-m \
(f/([.0-9]+) )?\
([0-9A-Za-z][0-9A-Za-z -]*[0-9A-Za-z])\
( [+]( ([0-9]+K?)(x([0-9]+K?))?)? CCD)?\
$", REG_EXTENDED);
    if (r) {
      regerror(r, &rxHdr, errLine, sizeof errLine);
      return -1;
    }
    rxTelCompiled = 1;
  }
  regmatch_t sub[10];
  if (regexec(&rxTel, val, 10, sub, 0)) {
    // no match, just add everything as design.
    newChild(tel, "design", val);
    return 0;
  }
  // aperture must match
  val[sub[1].rm_eo] = 0;
  newChild(tel, "aperture", val + sub[1].rm_so);

  // focal ratio optional
  int eo = sub[3].rm_eo;
  if (eo > 0) {
    val[eo] = 0;
    newChild(tel, "fRatio", val + sub[3].rm_so);
  }
  // design/name must match
  val[sub[4].rm_eo] = 0;
  mtDesign(val + sub[4].rm_so, tel);

  // size optional
  if ((eo = sub[7].rm_eo) > 0) {
    val[eo] = 0;
    int e2 = sub[9].rm_eo;
    if (e2 > 0) {
      val[e2] = 0;
    }
    snprintf(line2, sizeof line2, "%s \u00d7 %s",
             val + sub[7].rm_so, val + sub[e2 > 0 ? 9 : 7].rm_so);
    newChild(tel, "arraySize", line2);
  }
  return 0;
}

int mtNET(char *val)
{
  if (net)
    return mtFileError("multiple NET lines");
  net = strdup(val);
  return 0;
}

int mtBND(char *val)
{
  if (bnd)
    return mtFileError("multiple BND lines");
  bnd = strdup(val);
  return 0;
}

int mtCOM(char *val)
{
  if (com)
    asprintf(&com, "%s, %s", com, val);
  else
    com = strdup(val);
  return 0;
}

int mtNUM(char *val, xmlNodePtr ctx)
{
  xmlNodePtr n = newChild(ctx, "observation", NULL);
  newChild(n, "count", val);
  return 0;
}

int mtACK(char *val, xmlNodePtr ctx)
{
  if (!contactNode)
    contactNode = newChild(ctx, "contact", NULL);
  newChild(contactNode, "ackMessage", val);
  return 0;
}

int mtAC2(char *val)
{
  if (ackEmail)
    asprintf(&ackEmail, "%s,%s", ackEmail, val);
  else
    ackEmail = strdup(val);
  return 0;
}

int mtHdrLine(xmlNodePtr ctx)
{
  if (!strncmp(line, "COD", 3))
    return mtCOD(line + 4, ctx);
  if (!strncmp(line, "CON", 3))
    return mtCON(line + 4, ctx);
  if (!strncmp(line, "OBS", 3))
    return mtOBS(line + 4, ctx);
  if (!strncmp(line, "MEA", 3))
    return mtMEA(line + 4, ctx);
  if (!strncmp(line, "TEL", 3))
    return mtTEL(line + 4, ctx);
  if (!strncmp(line, "NET", 3))
    return mtNET(line + 4);
  if (!strncmp(line, "BND", 3))
    return mtBND(line + 4);
  if (!strncmp(line, "COM", 3))
    return mtCOM(line + 4);
  if (!strncmp(line, "NUM", 3))
    return mtNUM(line + 4, ctx);
  if (!strncmp(line, "ACK", 3))
    return mtACK(line + 4, ctx);
  if (!strncmp(line, "AC2", 3))
    return mtAC2(line + 4);
  return 0;
}

int mtHdrBlock()
{
  xmlNodePtr ctx = newChild(root_node, "observationContext", NULL);
  do {
    int r = mtHdrLine(ctx);
    if (r)
      return r;
    if (r = mtGetLine())
      return r;
  }
  while (mtMatchHdr());

  if (contactAddress) {
    newChild(contactNode, "address", contactAddress);
    contactAddress = NULL;
  }
  if (ackEmail) {
    if (!contactNode)
      contactNode = newChild(ctx, "contact", NULL);
    newChild(contactNode, "ackEmail", ackEmail);
    ackEmail = NULL;
  }
  if (com) {
    newChild(ctx, "comment", com);
    com = NULL;
  }
  if (contactNode)
    contactNode = NULL;
  if (observersNode)
    observersNode = NULL;
  if (measurersNode)
    measurersNode = NULL;
  return 0;
}

// copy a field of size `len` from `line`, starting at `start`,
// trimming leading and trailing space.
// the result in buf will be null terminated.
void copyTrim(int start, int len, char buf[len + 1])
{
  char *fld = line + start;
  char *p = fld;
  while (*p == ' ')
    if (++p == fld + len) {
      *buf = 0;
      return;
    }
  int c = fld + len - p;
  memcpy(buf, p, c);
  do
    buf[c] = 0;
  while (c > 0 && buf[--c] == ' ');
}

char *b62 = "0123456789\
ABCDEFGHIJKLMNOPQRSTUVWXYZ\
abcdefghijklmnopqrstuvwxyz";

int mtUnpackYear(char *p)
{
  return (*p - 'K' + 20) * 100 + (p[1] - '0') * 10 + p[2] - '0';
}

// p must be 5 character null terminated string.  formats into line2 and
// returns pointer to line2.
char *mtFormatUnpackPermMP(char *p)
{
  sprintf(line2, "%d", (strchr(b62, *p) - b62) * 10000 + atoi(p + 1));
  return line2;
}

int mtDesigMP(xmlNodePtr obs)
{
  int e = 0;
  char buf[6];
  memcpy(buf, line, 5);
  buf[5] = 0;
  if (!strcmp(buf, "     ")) ;  // common case, just prov des, no perm des
  else if (!regexec(&rxPermMP, buf, 0, NULL, 0)) {
    // it's a valid perm.  unpack, reformat, add.
    newChild(obs, "permID", mtFormatUnpackPermMP(buf));
  } else if (!regexec(&rxCometLetter, buf, 0, NULL, 0)) {
    // comet indicator on mp designation, just add prefix
    e = sprintf(line2, "%c/", buf[4]);
  } else if (!regexec(&rxPermComet, buf, 0, NULL, 0)) {
    // valid comet perm
    buf[4] = 0;
    sprintf(line2, "%dP", atoi(buf));
    newChild(obs, "permID", line2);
    strcpy(line2, "P/");
    e = 2;
  } else {                      // something crazy in perm field, trim and add it as trkSub
    copyTrim(0, 5, line2);
    newChild(obs, "trkSub", line2);
  }

  // in any case, there's still a prov des, perhaps with a prefix already
  // in line2.
  char *prov = line + 5;
  if (prov[2] == 'S') {
    // it's a historical survey designation
    memcpy(line2 + e, prov + 3, 4);
    sprintf(line2 + e + 4, " %c-%c", prov[0], prov[1]);
  } else {
    e += sprintf(line2 + e, "%d %c%c", mtUnpackYear(prov), prov[3], prov[6]);
    int cycle = (strchr(b62, prov[4]) - b62) * 10 + prov[5] - '0';
    if (cycle)
      sprintf(line2 + e, "%d", cycle);
  }
  newChild(obs, "provID", line2);
  return 0;
}

int mtDesigPermMP(xmlNodePtr obs)
{
  char buf[6];
  memcpy(buf, line, 5);
  buf[5] = 0;
  newChild(obs, "permID", mtFormatUnpackPermMP(buf));
  return 0;
}

// columns 0-11, desig = permID, provID, trkSub.
int mtDesig(xmlNodePtr obs)
{
  char buf[13];

  // consider prov des field first
  strncpy(buf, line + 5, 7);
  buf[7] = 0;

  if (!regexec(&rxProvMP, buf, 0, NULL, 0))
    return mtDesigMP(obs);
/*
  if (!regexec(&rxProvCometNatSat, buf, 0, NULL, 0))
    return mtDesigCometNatSat(obs);
*/
  // no match yet, consider perm des field
  memcpy(buf, line, 5);
  buf[5] = 0;
  if (!regexec(&rxPermMP, buf, 0, NULL, 0))
    return mtDesigPermMP(obs);
/*
  if (!regexec(&rxPermComet, buf, 0, NULL, 0))
    return mtDesigPermComet(obs);

  if (!regexec(&rxPermNatSat, buf, 0, NULL, 0))
    return mtDesigPermNatSat(obs);
*/
  // no perm match either, take any non-blank from the 12 columns as trkSub
  copyTrim(0, 12, buf);
  if (*buf)
    newChild(obs, "trkSub", buf);
  return 0;
}

int mtObsCCD(xmlNodePtr obs)
{
  printf("mtObsCCD\n");
  obs = newChild(obs, "optical", NULL);
  int r;
  if (r = mtDesig(obs))
    return r;
}

// compile rx for designation formats
//
// packed formats of:
//  perm comet
//  prov mp
//  prov comet
//  prov nat sat
// per http://www.minorplanetcenter.net/iau/info/PackedDes.html
//
// no known documentation on perm mp, perm nat sat
int mtCompileDesig()
{
  int r;

  if (r = regcomp(&rxPermMP, "^[0-9A-Za-z][0-9]{4}$", REG_EXTENDED)) {
    regerror(r, &rxPermComet, errLine, sizeof errLine);
    return -1;
  }

  if (r = regcomp(&rxPermComet, "^[0-9]{4}P$", REG_EXTENDED)) {
    regerror(r, &rxPermComet, errLine, sizeof errLine);
    return -1;
  }

  if (r = regcomp(&rxCometLetter, "^ {4}[CPDXA]$", REG_EXTENDED)) {
    regerror(r, &rxCometLetter, errLine, sizeof errLine);
    return -1;
  }

  if (r = regcomp(&rxPermNatSat, "^[JSUN][0-9]{3}S$", REG_EXTENDED)) {
    regerror(r, &rxPermComet, errLine, sizeof errLine);
    return -1;
  }

  if (r = regcomp(&rxProvMP, "^\
([IJK][0-9]{2}[A-HJ-Y][0-9A-Za-z][0-9][A-HJ-Z])|\
((PL|T1|T2|T3)S[0-9]{4})\
$", REG_EXTENDED)) {
    regerror(r, &rxProvMP, errLine, sizeof errLine);
    return -1;
  }

  if (r = regcomp(&rxProvCometNatSat, "^\
[IJK][0-9]{2}[A-HJ-Y][0-9A-Za-z][0-9][0a-z]\
$", REG_EXTENDED)) {
    regerror(r, &rxProvCometNatSat, errLine, sizeof errLine);
    return -1;
  }

  rxDesigCompiled = 1;
  return 0;
}

int mtObsBlock()
{
  if (!rxDesigCompiled)
    mtCompileDesig();

  xmlNodePtr obs = newChild(root_node, "observations", NULL);
  int r;
  do {
    if (strlen(line) != 80)
      return mtFileError("line not 80 columns");
    switch (line[14]) {
    case 'C':
      r = mtObsCCD(obs);
      break;
    default:
      r = mtFileError("column 15 unimplemented or invalid observation mode");
    }
    if (r)
      return r;
    if (r = mtGetLine())
      return r;
  }
  while (*line);
  net = NULL;
  bnd = NULL;
  return 0;
}

// mt
//
int mt(char *fn, xmlDocPtr * pDoc)
{
  if (!(fobs = fopen(fn, "r")))
    return error1("can't open obs file %s", fn);
  int r = mtGetLine();
  if (r)
    return r;
  if (!*line)
    return error1("Obs file %s empty or unreadable", fn);

  doc = xmlNewDoc("1.0");
  root_node = xmlNewNode(NULL, "observationBatch");
  root_node->line = 1;
  xmlDocSetRootElement(doc, root_node);

  if (!rxHdrCompiled) {
    int r = regcomp(&rxHdr, "^[A-Z]{2}[A-Z2] ", REG_EXTENDED | REG_NOSUB);
    if (r) {
      regerror(r, &rxHdr, errLine, sizeof errLine);
      return -1;
    }
    rxHdrCompiled = 1;
  }

  if (!mtMatchHdr())
    goto obsBlock;
  do {
    if (r = mtHdrBlock())
      return r;
    if (!*line)
      break;
 obsBlock:
    if (r = mtObsBlock())
      return r;
  } while (*line);

  *pDoc = doc;
  return 0;
}

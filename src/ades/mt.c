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
#include <time.h>

#include <globals.h>

FILE *fobs;
regex_t rxHdr;
_Bool rxHdrCompiled;
regex_t rxTel;
_Bool rxTelCompiled;
regex_t rxPermMP;
regex_t rxPermComet;
regex_t rxPermNatSat;
regex_t rxProvAny;
regex_t rxProvMP;
regex_t rxProvComet;
regex_t rxProvNatSat;
regex_t rxCometLetter;
regex_t rxCometFragment;
regex_t rxDate;
regex_t rxRA;
regex_t rxDec;
_Bool rxObsCompiled;

char *catalogData;
char *progCodeData;
_Bool dataFilesLoaded;
char *telescopeData;
_Bool telDataLoaded;

xmlNodePtr ctxNode;
xmlNodePtr contactNode;
xmlNodePtr observersNode;
xmlNodePtr measurersNode;
_Bool observationCountSet;
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
  xmlNodePtr n =
    xmlNewChild(parent, NULL, name, xmlEncodeEntitiesReentrant(doc, val));
  n->line = lineNum;
  return n;
}

int mtCOD(char *val)
{
  xmlNodePtr obs = newChild(ctxNode, "observatory", NULL);
  newChild(obs, "mpcCode", val);
  return 0;
}

int mtCON(char *val)
{
  if (!contactNode)
    contactNode = newChild(ctxNode, "contact", NULL);

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

int mtOBS(char *val)
{
  if (!observersNode)
    observersNode = newChild(ctxNode, "observers", NULL);
  return mtNameList(val, observersNode);
}

int mtMEA(char *val)
{
  if (!measurersNode)
    measurersNode = newChild(ctxNode, "measurers", NULL);
  return mtNameList(val, measurersNode);
}

char *loadFile(char *fn)
{
  FILE *f = fopen(fn, "r");
  if (!f)
    return NULL;
  struct stat st;
  if (fstat(fileno(f), &st))
    return NULL;
  char *buf = malloc(st.st_size + 1);
  fread(buf, st.st_size, 1, f);
  buf[st.st_size] = 0;
  fclose(f);
  return buf;
}

void mtDesign(char *design, xmlNodePtr tel)
{
  if (!telDataLoaded) {
    telescopeData = loadFile("telescopes.csv");
    telDataLoaded = 1;
  }
  if (telescopeData) {
    char *p;
    int dl = strlen(design);
    for (p = telescopeData; (p = strstr(p, design)); p += dl) {
      // must start at beginning of line
      if (!(p == telescopeData || p[-1] == '\n'))
        continue;
      if (p[dl] != ',')         // must end at comma
        continue;
      // design argument matches telescope name, actual design follows comma.
      p += dl + 1;
      dl = strcspn(p, "\n\r");
      memcpy(line2, p, dl);
      line2[dl] = 0;
      newChild(tel, "name", design);
      newChild(tel, "design", line2);
      return;
    }
  }
  newChild(tel, "design", design);
}

int mtTEL(char *val)
{
  xmlNodePtr tel = newChild(ctxNode, "telescope", NULL);
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

int mtNUM(char *val)
{
  xmlNodePtr n = newChild(ctxNode, "observation", NULL);
  newChild(n, "count", val);
  observationCountSet = 1;
  return 0;
}

int mtACK(char *val)
{
  if (!contactNode)
    contactNode = newChild(ctxNode, "contact", NULL);
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

int mtHdrLine()
{
  if (!strncmp(line, "COD", 3))
    return mtCOD(line + 4);
  if (!strncmp(line, "CON", 3))
    return mtCON(line + 4);
  if (!strncmp(line, "OBS", 3))
    return mtOBS(line + 4);
  if (!strncmp(line, "MEA", 3))
    return mtMEA(line + 4);
  if (!strncmp(line, "TEL", 3))
    return mtTEL(line + 4);
  if (!strncmp(line, "NET", 3))
    return mtNET(line + 4);
  if (!strncmp(line, "BND", 3))
    return mtBND(line + 4);
  if (!strncmp(line, "COM", 3))
    return mtCOM(line + 4);
  if (!strncmp(line, "NUM", 3))
    return mtNUM(line + 4);
  if (!strncmp(line, "ACK", 3))
    return mtACK(line + 4);
  if (!strncmp(line, "AC2", 3))
    return mtAC2(line + 4);
  return 0;
}

int mtHdrBlock()
{
  contactNode = NULL;
  observersNode = NULL;
  measurersNode = NULL;

  ctxNode = newChild(root_node, "observationContext", NULL);
  do {
    int r = mtHdrLine(ctxNode);
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
      contactNode = newChild(ctxNode, "contact", NULL);
    newChild(contactNode, "ackEmail", ackEmail);
    ackEmail = NULL;
  }
  if (com) {
    newChild(ctxNode, "comment", com);
    com = NULL;
  }
  return 0;
}

enum {
  DT_MP,
  DT_Comet,
  DT_NatSat,
};

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

// formats prov field of line to offset e of line2,
// returns pointer to line2.
char *mtFormatUnpackProvMP(int e)
{
  char *p = line + 5;
  e += sprintf(line2 + e, "%d %c%c", mtUnpackYear(p), p[3], p[6]);
  int cycle = (strchr(b62, p[4]) - b62) * 10 + p[5] - '0';
  if (cycle)
    sprintf(line2 + e, "%d", cycle);
  return line2;
}

int mtDesigPermMP(xmlNodePtr obs)
{
  char buf[8];                  // enough for prov,
  memcpy(buf, line, 5);         // but just copy perm at first
  buf[5] = 0;
  newChild(obs, "permID", mtFormatUnpackPermMP(buf));

  // also handle anything in prov field
  if (!memcmp(line + 5, "       ", 7))
    return 0;

  // see if prov field has valid mp des
  memcpy(buf, line + 5, 7);
  buf[7] = 0;
  if (!regexec(&rxProvMP, buf, 0, NULL, 0)) {
    newChild(obs, "provID", mtFormatUnpackProvMP(0));
    return 0;
  }
  // add anything else as trkSub
  copyTrim(5, 7, buf);
  newChild(obs, "trkSub", buf);
  return 0;
}

int mtDesigProvMP(xmlNodePtr obs, int *desType)
{
  int e = 0;
  char buf[6];
  memcpy(buf, line, 5);
  buf[5] = 0;
  if (!strcmp(buf, "     ")) ;  // common case, just prov des, perm field blank
  else if (!regexec(&rxCometLetter, buf, 0, NULL, 0)) {
    // comet indicator on mp designation, add prefix
    e = sprintf(line2, "%c/", buf[4]);
    *desType = DT_Comet;
  } else {
    // something crazy in perm field, trim and add it as trkSub
    copyTrim(0, 5, line2);
    newChild(obs, "trkSub", line2);
  }

  // in any case, now there's a prov des, perhaps with a prefix already
  // in line2.
  char *prov = line + 5;
  if (prov[2] == 'S') {
    // it's a historical survey designation
    memcpy(line2 + e, prov + 3, 4);
    sprintf(line2 + e + 4, " %c-%c", prov[0], prov[1]);
  } else
    mtFormatUnpackProvMP(e);
  newChild(obs, "provID", line2);
  return 0;
}

// formats prov field of line to offset e of line2,
// returns pointer to line2.
char *mtFormatUnpackProvComet(int e)
{
  char frag = 0;
  char *p = line + 5;
  e += sprintf(line2 + e, "%d %c", mtUnpackYear(p), p[3]);
  if (p[6] == '0') ;
  // A-Y are the 25 valid mp "order within half month" letters
  else if (strchr("ABCDEFGHIJKLMNOPQRSTUVWXY", p[6]))
    line2[e++] = p[6];
  else                          // rx leaves only lower case letter
    frag = p[6];
  e += sprintf(line2 + e, "%d", (strchr(b62, p[4]) - b62) * 10 + p[5] - '0');
  if (frag)
    sprintf(line2 + e, "-%c", frag - 32);
  return line2;
}

int mtDesigPermComet(xmlNodePtr obs)
{
  char buf[8];                  // enough for prov,
  memcpy(buf, line, 5);         // but just copy perm at first
  buf[5] = 0;
  int e = sprintf(line2, "%dP", atoi(buf + 1));

  // perm formatted into line2 now, not added yet

  // simplest case, prov field blank
  if (!memcmp(line + 5, "       ", 7)) {
    newChild(obs, "permID", line2);
    return 0;
  }
  // check for fragment, no prov
  memcpy(buf, line + 5, 7);
  buf[7] = 0;
  regmatch_t sub[2];
  if (!regexec(&rxCometFragment, buf, 2, sub, 0)) {
    for (int i = sub[1].rm_so; i < sub[1].rm_eo; i++) {
      buf[i] -= 32;
      sprintf(line2 + e, "-%s", buf + sub[1].rm_so);
    }
    newChild(obs, "permID", line2);
    return 0;
  }
  // see if prov field has a full valid prov des
  if (!regexec(&rxProvAny, buf, 0, NULL, 0)) {
    // it does, but perm still needs to be added before using line2.
    // tack on any single letter fragment, then add.
    _Bool mp = 0;
    if (buf[6] == '0') ;        // fast path
    else if (strchr("abcdefghijklmnopqrstuvwxyz", buf[6]))
      sprintf(line2 + e, "-%c", buf[6] - 32);
    else {
      // it must be an upper case letter of the mp format
      mp = 1;
    }
    newChild(obs, "permID", line2);

    // all clear, now do prov des
    strcpy(line2, "P/");
    newChild(obs, "provID",
             mp ? mtFormatUnpackProvMP(2) : mtFormatUnpackProvComet(2));
    return 0;
  }
  // otherwise add anything else as trkSub
  copyTrim(5, 7, buf);
  newChild(obs, "trkSub", buf);
  return 0;
}

int mtDesigProvComet(xmlNodePtr obs)
{
  int e = 0;
  int scrap = 5;
  if (strchr("CPXDA", line[4])) {
    e = sprintf(line2, "%c/", line[4]);
    scrap--;
  }
  newChild(obs, "provID", mtFormatUnpackProvComet(e));
  if (memcmp(line, "     ", scrap)) {
    copyTrim(0, scrap, line2);
    newChild(obs, "trkSub", line2);
  }
  return 0;
}

// formats prov field of line to offset e of line2,
// returns pointer to line2.
char *mtFormatUnpackProvNatSat(int e)
{
  char frag = 0;
  char *p = line + 5;
  sprintf(line2, "S/%d %c %d",
          mtUnpackYear(p), p[3], (strchr(b62, p[4]) - b62) * 10 + p[5] - '0');
  return line2;
}

int mtDesigPermNatSat(xmlNodePtr obs)
{
  char buf[8];
  memcpy(buf, line, 4);         // fifth char is S, don't copy it.
  buf[4] = 0;
  int e = sprintf(line2, "%c%d", *buf, atoi(buf + 1));
  newChild(obs, "permID", line2);

  // fast path, prov field blank
  if (!memcmp(line + 5, "       ", 7)) {
    return 0;
  }
  // see if prov field has a full valid nat sat prov des
  memcpy(buf, line + 5, 7);
  buf[7] = 0;
  if (!regexec(&rxProvNatSat, buf, 0, NULL, 0)) {
    newChild(obs, "provID", mtFormatUnpackProvNatSat(0));
    return 0;
  }
  // otherwise add anything else as trkSub
  copyTrim(5, 7, buf);
  newChild(obs, "trkSub", buf);
  return 0;
}

int mtDesigProvNatSat(xmlNodePtr obs)
{
  newChild(obs, "provID", mtFormatUnpackProvNatSat(0));

  if (memcmp(line, "    ", 4)) {
    copyTrim(0, 4, line2);
    newChild(obs, "trkSub", line2);
  }
  return 0;
}

// columns 0-11, desig = permID, provID, trkSub.
int mtDesig(xmlNodePtr obs, int *desType)
{
  *desType = DT_MP;
  char buf[13];
  // consider perm des field first.
  memcpy(buf, line, 5);
  buf[5] = 0;
  if (!regexec(&rxPermMP, buf, 0, NULL, 0))
    return mtDesigPermMP(obs);
  if (!regexec(&rxPermComet, buf, 0, NULL, 0)) {
    *desType = DT_Comet;
    return mtDesigPermComet(obs);
  }
  if (!regexec(&rxPermNatSat, buf, 0, NULL, 0)) {
    *desType = DT_NatSat;
    return mtDesigPermNatSat(obs);
  }
  // no valid perm, consider prov field
  memcpy(buf, line + 5, 7);
  buf[7] = 0;
  if (!regexec(&rxProvMP, buf, 0, NULL, 0))
    return mtDesigProvMP(obs, desType);
  if (line[4] == 'S' && !regexec(&rxProvNatSat, buf, 0, NULL, 0)) {
    *desType = DT_NatSat;
    return mtDesigProvNatSat(obs);
  }
  if (!regexec(&rxProvComet, buf, 0, NULL, 0)) {
    *desType = DT_Comet;
    return mtDesigProvComet(obs);
  }
  // no perm or prov match, take any non-blank from the 12 columns as trkSub
  copyTrim(0, 12, buf);
  if (*buf)
    newChild(obs, "trkSub", buf);
  desType = DT_MP;
  return 0;
}

void addPCNote(xmlNodePtr obs)
{
  char cc[5];
  memcpy(cc, line + 77, 3);
  cc[3] = line[13];
  cc[4] = 0;
  // invalid in obscodes, and code here must skip comment and blank lines
  // in the .txt file
  if (cc[0] == ' ' || cc[0] == '#')
    goto noPC;

  if (!progCodeData)
    goto noPC;
  char *p = strstr(progCodeData, cc);
  if (!p)
    goto noPC;
  if (p != progCodeData && p[-1] != '\n') // must start at beginning of line
    goto noPC;
  if (p[4] != ' ')              // must end at space
    goto noPC;
  // matched obs code and prog code, prog num follows
  p += 5;
  int cl = strcspn(p, "\n\r");
  memcpy(line2, p, cl);
  line2[cl] = 0;
  newChild(obs, "prg", line2);
  return;

 noPC:
  newChild(obs, "notes", cc + 3);
}

int addDate(xmlNodePtr obs, char *d, char *logsnr)
{
  // fill in "broken down time" struct
  struct tm br;
  memset(&br, 0, sizeof br);

  // parse year, month, and integer day directly
  d[4] = 0;
  br.tm_year = atoi(d) - 1900;
  d[7] = 0;
  br.tm_mon = atoi(d + 5) - 1;  // not ordinal
  d[10] = 0;
  br.tm_mday = atoi(d + 8);     // ordinal

  // fraction of day can have six digits or not.
  // output, after conversion to hours minutes and seconds, will have
  // integer seconds if there are less than six digits. it will have
  // tenths of seconds if that sixth digit is present.
  d[10] = '.';
  double dFrac = strtod(d + 10, NULL) * 24 * 60 * 60; // day frac as seconds
  int tenths;
  if (d[16] == ' ')
    br.tm_sec = dFrac + .5;     // round to whole seconds
  else {
    br.tm_sec = dFrac * 10 + .5; // dFrac in tenths of seconds, rounded.
    tenths = br.tm_sec % 10;
    br.tm_sec /= 10;            // iFrac now day frac in whole seconds
  }

  // format and add
  time_t t = mktime(&br);
  if (t == -1)
    return mtFileError("invalid date");
  int e = strftime(line2, sizeof line2, "%FT%H:%M:%S", &br);
  if (d[16] != ' ')
    e += sprintf(line2 + e, ".%c", '0' + tenths);
  strcpy(line2 + e, "Z");
  newChild(obs, "obsTime", line2);

  if (!logsnr) {                // precTime
    int pt = 1;
    for (int i = 16; d[i] == ' ' && i > 10; i--)
      pt *= 10;
    sprintf(line2, "%d", pt);
    newChild(obs, "precTime", line2);
  }
}

void addRA(xmlNodePtr obs, char *r, char *logsnr)
{
  r[2] = 0;
  int hr = atoi(r);
  r[5] = 0;
  int min = atoi(r + 3);
  double sec = strtod(r + 6, NULL);
  double deg = 15 * (hr + (min + sec / 60) / 60);

  int d = 6;                    // digits for sprintf
  double pr = .001;             // precRA
  for (int i = 11; r[i] == ' ' && i > 8; i--) {
    d--;
    pr *= 10;
  }

  sprintf(line2, "%.*f", d, deg);
  newChild(obs, "ra", line2);

  if (!logsnr) {
    sprintf(line2, "%.3g", pr);
    newChild(obs, "precRA", line2);
  }
}

void addDec(xmlNodePtr obs, char *d, char *logsnr)
{
  d[3] = 0;
  int deg = atoi(d + 1);
  d[6] = 0;
  int min = atoi(d + 4);
  double sec = strtod(d + 7, NULL);
  double dd = deg + (min + sec / 60) / 60;
  if (*d == '-')
    dd = -dd;

  int p = 6;                    // precision for sprintf
  double pr = .01;              // precDec
  for (int i = 11; d[i] == ' ' && i > 9; i--) {
    p--;
    pr *= 10;
  }

  sprintf(line2, "%.*f", p, dd);
  newChild(obs, "dec", line2);

  if (!logsnr) {
    sprintf(line2, "%.2g", pr);
    newChild(obs, "precDec", line2);
  }
}

void addCat(xmlNodePtr obs, _Bool mag)
{
  char *n = "UNK";
  if (net)
    n = net;
  // space means no cat, # invalid.  (code here must skip comment lines
  // and blank lines in file.)
  if (line[71] == ' ' || line[71] == '#')
    goto add;
  if (!catalogData)
    goto add;
  char *c = catalogData;
  if (c[0] != line[71] || c[1] != ',') {
    sprintf(line2, "\n%c,", line[71]);
    c = strstr(catalogData, line2);
    if (!c)
      goto add;
    c++;
  }

  int cl = strcspn(c + 2, "\n\r");
  memcpy(line2, c + 2, cl);
  line2[cl] = 0;
  n = line2;
 add:
  newChild(obs, "astCat", n);
  if (mag)
    newChild(obs, "photCat", n);
}

// NULL logsnr means generate exchange format; output all elements.
// non-NULL logsnr means generate submit format; add logSNR but no subFmt,
// timePrec, raPrec, or decPrec.  Presence of data for ref or disc are errors.
//
int mtObsCCD(xmlNodePtr obs, char *logsnr)
{
  obs = newChild(obs, "optical", NULL);
  if (logsnr)
    newChild(obs, "logSNR", logsnr);
  else
    newChild(obs, "subFmt", "M92");

  // cols 0-11, parse designation, also returning designation type
  int r, desType;
  if (r = mtDesig(obs, &desType))
    return r;

  // col 12, discovery
  if (line[12] != ' ' && desType == DT_MP) {
    if (logsnr)
      return error("discovery indicator not allowed in submission format");
    char d[2];
    d[0] = line[12];
    d[1] = 0;
    newChild(obs, "disc", d);
  }
  // col 13, program code or note
  if (line[13] != ' ')
    addPCNote(obs);

  // col 14, mode already known to contain 'C'
  newChild(obs, "mode", "CCD");

  {                             // cols 15-31, time
    char d[18];
    memcpy(d, line + 15, 17);
    d[17] = 0;
    if (regexec(&rxDate, d, 0, NULL, 0))
      return mtFileError("invalid date");
    addDate(obs, d, logsnr);
  }
  {                             // cols 32-43, RA
    char r[13];
    memcpy(r, line + 32, 12);
    r[12] = 0;
    if (regexec(&rxRA, r, 0, NULL, 0))
      return mtFileError("invalid RA");
    addRA(obs, r, logsnr);
  }
  {                             // cols 44-55, Dec
    char d[13];
    memcpy(d, line + 44, 12);
    d[12] = 0;
    if (regexec(&rxDec, d, 0, NULL, 0))
      return mtFileError("invalid Dec");
    addDec(obs, d, logsnr);
  }

  if (memcmp(line + 56, "         ", 9))
    return mtFileError("columns between Dec and Mag must be blank");

  _Bool mag = 0;                // cols 65-69, Mag
  copyTrim(65, 5, line2);
  if (*line2) {
    newChild(obs, "mag", line2);
    mag = 1;
  }

  if (line[70] != ' ') {        // col 70, Band
    line2[0] = line[70];
    line2[1] = 0;
    newChild(obs, "band", line2);
  }

  addCat(obs, mag);             // col 71, always add astCat

  // TEST just output 5 character ref for now.  maybe expand later.
  copyTrim(72, 5, line2);       // cols 72-76, reference
  if (*line2) {
    if (logsnr)
      return error("reference not allowed in submission format");
    newChild(obs, "ref", line2);
  }

  copyTrim(77, 3, line2);       // cols 77-79, Obscode
  if (*line2)
    newChild(obs, "stn", line2);

  return 0;
}

// compile rx for observation fields
int mtCompileObsRx()
{
  int r;

  if (r = regcomp(&rxPermMP, "^[0-9A-Za-z][0-9]{4}$", REG_EXTENDED)) {
    regerror(r, &rxPermMP, errLine, sizeof errLine);
    return -1;
  }
  if (r = regcomp(&rxPermComet, "^[0-9]{4}P$", REG_EXTENDED)) {
    regerror(r, &rxPermComet, errLine, sizeof errLine);
    return -1;
  }
  // rxCometLetter matches the case where the perm field is blank except
  // for a comet indicating letter.
  if (r = regcomp(&rxCometLetter, "^ {4}[CPDXA]$", REG_EXTENDED)) {
    regerror(r, &rxCometLetter, errLine, sizeof errLine);
    return -1;
  }
  // rxCometFragment matches the case where the prov field is blank except
  // for a fragment.  match against the prov field after the perm
  // field is found to contain a permanent comet designation.
  if (r = regcomp(&rxCometFragment, "^ +([a-z]{1,2})$", REG_EXTENDED)) {
    regerror(r, &rxCometFragment, errLine, sizeof errLine);
    return -1;
  }
  if (r = regcomp(&rxPermNatSat, "^[JSUN][0-9]{3}S$", REG_EXTENDED)) {
    regerror(r, &rxPermNatSat, errLine, sizeof errLine);
    return -1;
  }
  if (r = regcomp(&rxProvMP, "^\
([IJK][0-9]{2}[A-HJ-Y][0-9A-Za-z][0-9][A-HJ-Z])|\
((PL|T1|T2|T3)S[0-9]{4})\
$", REG_EXTENDED)) {
    regerror(r, &rxProvMP, errLine, sizeof errLine);
    return -1;
  }
  if (r = regcomp(&rxProvComet, "^\
[IJK][0-9]{2}[A-HJ-Y][0-9A-Za-z][0-9][0a-z]\
$", REG_EXTENDED)) {
    regerror(r, &rxProvComet, errLine, sizeof errLine);
    return -1;
  }
  if (r = regcomp(&rxProvNatSat, "^\
[IJK][0-9]{2}[JSUN][0-9A-Za-z][0-9]0\
$", REG_EXTENDED)) {
    regerror(r, &rxProvNatSat, errLine, sizeof errLine);
    return -1;
  }
  if (r = regcomp(&rxProvAny, "^\
[IJK][0-9]{2}[A-HJ-Y][0-9A-Za-z][0-9][0a-zA-HJ-Z]\
$", REG_EXTENDED)) {
    regerror(r, &rxProvAny, errLine, sizeof errLine);
    return -1;
  }
  if (r = regcomp(&rxDate, "^\
[12][0-9]{3} [ 01][0-9] [ 0-3][0-9](.[0-9]*)? *\
$", REG_EXTENDED)) {
    regerror(r, &rxDate, errLine, sizeof errLine);
    return -1;
  }
  if (r = regcomp(&rxRA, "^\
[ 012][0-9] [ 0-5][0-9] [ 0-5][0-9](.[0-9]*)? *\
$", REG_EXTENDED)) {
    regerror(r, &rxRA, errLine, sizeof errLine);
    return -1;
  }
  if (r = regcomp(&rxDec, "^\
[ +-][ 0-9][0-9] [ 0-5][0-9] [ 0-5][0-9](.[0-9]*)? *\
$", REG_EXTENDED)) {
    regerror(r, &rxDec, errLine, sizeof errLine);
    return -1;
  }

  rxObsCompiled = 1;
  return 0;
}

int mtObsBlock(char *logsnr)
{
  if (!rxObsCompiled) {
    int r = mtCompileObsRx();
    if (r)
      return r;
  }
  if (!dataFilesLoaded) {
    catalogData = loadFile("catalogs.csv");
    progCodeData = loadFile("program_codes.txt");
    dataFilesLoaded = 1;
  }

  xmlNodePtr obs = newChild(root_node, "observations", NULL);
  int r;
  int c = 0;
  do {
    if (strlen(line) != 80)
      return mtFileError("line not 80 columns");
    switch (line[14]) {
    case 'C':
      r = mtObsCCD(obs, logsnr);
      break;
    default:
      r = mtFileError("column 15 unimplemented or invalid observation mode");
    }
    if (r)
      return r;
    c++;                        // count obs
    if (r = mtGetLine())
      return r;
  }
  while (*line);

  if (ctxNode && !observationCountSet) {
    sprintf(line2, "%d", c);
    mtNUM(line2);
  }

  net = NULL;
  bnd = NULL;
  ctxNode = NULL;
  observationCountSet = 0;
  return 0;
}

// mt
//
int mt(char *fn, xmlDocPtr * pDoc, char *logsnr)
{
  if (!(fobs = fopen(fn, "r")))
    return error1("can't open obs file %s", fn);
  int r = mtGetLine();
  if (r)
    return r;
  if (!*line)
    return error1("Obs file %s empty or unreadable", fn);

  // tolerate bom
  if (!memcmp(line, "\xef\xbb\xbf", 3))
    strcpy(line, line + 3);

  doc = xmlNewDoc("1.0");
  root_node = xmlNewNode(NULL, "observationBatch");
  root_node->line = 1;
  xmlDocSetRootElement(doc, root_node);

  if (!rxHdrCompiled) {
    // spec says space is required, but mtGetLine trims trailing space.
    // $ allows <hdr><sp>\n after it has been trimmed to just <hdr>
    // by mtGetLine.
    int r = regcomp(&rxHdr, "^[A-Z]{2}[A-Z2]( |$)", REG_EXTENDED | REG_NOSUB);
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
    if (r = mtObsBlock(logsnr))
      return r;
  } while (*line);

  *pDoc = doc;
  return 0;
}

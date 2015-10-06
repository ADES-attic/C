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

#include <globals.h>

FILE *fobs;
regex_t rxHdr;
_Bool rxCompiled;

xmlNodePtr contactNode;
xmlNodePtr observersNode;
xmlNodePtr measurersNode;
_Bool contactNameSet;
char *contactAddress;
char *ackEmail;

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
  if (!rxCompiled) {
    int r = regcomp(&rxHdr, "^[A-Z]{2}[A-Z2] ", REG_EXTENDED | REG_NOSUB);
    if (r) {
      regerror(r, &rxHdr, errLine, sizeof errLine);
      exit(-1);
    }
    rxCompiled = 1;
  }
  return !regexec(&rxHdr, line, 0, NULL, 0);
}

int mtCOD(char *val, xmlNodePtr ctx)
{
  xmlNodePtr obs = xmlNewChild(ctx, NULL, "observatory", NULL);
  xmlNewChild(obs, NULL, "mpcCode", val);
  return 0;
}

int mtCON(char *val, xmlNodePtr ctx)
{
  if (!contactNode) {
    contactNode = xmlNewChild(ctx, NULL, "contact", NULL);
    contactNode->line = lineNum;
  }

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
    xmlNodePtr n = xmlNewChild(contactNode, NULL, "name", val);
    n->line = lineNum;
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
    xmlNewChild(parent, NULL, "name", val);
    if (!c)
      break;
    val = c + 1;
  }
  return 0;
}

int mtOBS(char *val, xmlNodePtr ctx)
{
  if (!observersNode)
    observersNode = xmlNewChild(ctx, NULL, "observers", NULL);
  return mtNameList(val, observersNode);
}

int mtMEA(char *val, xmlNodePtr ctx)
{
  if (!measurersNode)
    measurersNode = xmlNewChild(ctx, NULL, "measurers", NULL);
  return mtNameList(val, measurersNode);
}

int mtTEL(char *val, xmlNodePtr ctx)
{
  xmlNodePtr tel = xmlNewChild(ctx, NULL, "telescope", val);
  return 0;
}

int mtHdrLine(xmlNodePtr ctx)
{
  printf("mtHdrLine: %s\n", line);
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
  return 0;
}

int mtHdrBlock()
{
  printf("mtHdrBlock\n");
  xmlNodePtr ctx = xmlNewChild(root_node, NULL,
                               "observationContext", NULL);
  ctx->line = lineNum;
  do {
    int r = mtHdrLine(ctx);
    if (r)
      return r;
    if (r = mtGetLine())
      return r;
  }
  while (mtMatchHdr());

  if (contactAddress) {
    xmlNewChild(contactNode, NULL, "address", contactAddress);
    contactAddress = NULL;
  }
  if (ackEmail) {
    xmlNewChild(contactNode, NULL, "ackEmail", ackEmail);
    ackEmail = NULL;
  }
  if (contactNode)
    contactNode = NULL;
  if (observersNode)
    observersNode = NULL;
  if (measurersNode)
    measurersNode = NULL;
  return 0;
}

int mtObsBlock()
{
  printf("mtObsBlock\n");
  do {
    int r = mtGetLine();
    if (r)
      return r;
  }
  while (*line);
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

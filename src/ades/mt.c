//
// mt.c -- (M)PC 80 column to libxml2 (T)ree conversion.
//
// Implements function mt().
//
// This function intended for use primarily within the ades library and not
// as an API function.  The result is a libxml2 xmlDocPtr and the idea is that
// API users should not be required to interpret libxml2 data types.

#include <regex.h>
#include <string.h>

#include <globals.h>

FILE *fobs;
regex_t rxHdr;
_Bool rxCompiled = 0;

// errorObsFile functions also use globals from globals.c
int errorObsFile(char *msg)
{
  snprintf(errLine, sizeof errLine, "Obs file line %d: %s", lineNum, msg);
  return -1;
}

int errorObsFile1(char *msg, char *arg)
{
  int n = snprintf(errLine, sizeof errLine, "Obs file line %d: ", lineNum);
  snprintf(errLine + n, sizeof errLine - n, msg, arg);
  return -1;
}

// get non-blank line, trim trailing space.
//
// on success global `line` contains a non-empty string and  0 is returned.
//
// at end of file, *line is set to NULL, and 0 is still returned.
//
// A non-zero return value means some other error, such as a line too long.
int getMPCLine()
{
  char *p;
  do {
    p = fgets(line, sizeof(line), fobs);
    if (!p) {
      *line = 0;
      break;
    }
    if (strlen(p) == sizeof line - 1)
      return errorObsFile("line too long");
    lineNum++;
    char *q = strchr(p, 0);     // trim trailing space
    while (q != p && *--q == ' ')
      *q = 0;
  }
  while (!*p);                  // loop on blank lines
  return 0;
}

_Bool mtMatchHdr()
{
  if (!rxCompiled) {
    int r = regcomp(&rxHdr, "^[A-Z]{2}[A-Z2] ", REG_NOSUB);
    if (r) {
      regerror(r, &rxHdr, errLine, sizeof errLine);
      exit(-1);
    }
    rxCompiled = 1;
  }
  return !regexec(&rxHdr, line, 0, NULL, 0);
}

// mt
//
int mt(char *fn, xmlDocPtr * pDoc)
{
  fobs = fopen(fn, "r");
  if (!fobs) {
    return error1("can't open obs file %s", fn);
  }
  int r = getMPCLine();
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
    r = mtHdrBlock();
    if (r)
      return r;
    if (!*line)
      break;
 obsBlock:
    r = mtObsBlock();
    if (r)
      return r;
  } while (*line);

  *pDoc = doc;
  return 0;
}

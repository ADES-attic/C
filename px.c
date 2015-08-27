#include <libxml/tree.h>
#include <string.h>

// globals
FILE *fpsv;
int lineNum;
xmlDocPtr doc;
xmlNodePtr root_node;
char line[512];
char kw[31];

// fatal
//
// print message to stdout and terminate program.
void fatal(char *msg)
{
  fputs(msg, stderr);
  fputc('\n', stderr);
  exit(-1);
}

void fatal1(char *msg, char *arg)
{
  sprintf(line, msg, arg);
  fatal(line);
}

void fatalPSV(char *msg)
{
  sprintf(line, "PSV line %d: %s", lineNum, msg);
  fatal(line);
}

char *getPSVLine()
{
  // TODO fgets bad, getline better?
  char *p = fgets(line, sizeof(line), fpsv);
  if (p)
    lineNum++;
  if (line[strlen(line) - 1] == '\n')
    line[strlen(line) - 1] = 0;
  return p;
}

int trimSpace(char *s, char **start)
{
  // TODO handle general whitespce
  char *s2 = s + strspn(s, " ");
  *start = s2;
  size_t len = strlen(s2);
  while (len > 0 && s2[len - 1] == ' ')
    len--;
  return len;
}

void pxObs()
{
}

// handle a single header line
xmlNodePtr addHdr(xmlNodePtr parent)
{
  // parse keyword
  char *kwStart = line + 1 + strspn(line + 1, " ");
  if (!*kwStart)
    fatalPSV("missing header keyword");
  char *kwEnd = strchr(kwStart, ' ');
  size_t kwLen = kwEnd ? kwEnd - kwStart : strlen(kwStart);
  if (kwLen > sizeof(kw) - 1)
    fatalPSV("long keyword");
  strncpy(kw, kwStart, kwLen);
  kw[kwLen] = 0;
  if (!xmlCheckUTF8(kw))
    fatalPSV("invalid characters present");
  // parse text following keyword
  char *txt = NULL;
  if (kwEnd) {
    kwLen = trimSpace(kwEnd, &txt);
    if (!kwLen)
      txt = NULL;
    else {
      txt[kwLen] = 0;
      if (!xmlCheckUTF8(txt))
        fatalPSV("invalid characters present");
    }
  }
  return xmlNewChild(parent, NULL, BAD_CAST kw, BAD_CAST txt);
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
  root_node = xmlNewNode(NULL, BAD_CAST "ObservationBatch");
  xmlDocSetRootElement(doc, root_node);

  px();

  xmlSaveFormatFileEnc(argv[2], doc, "UTF-8", 1);
}

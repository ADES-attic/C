#include <libxml/tree.h>

#include <ades.h>
#include <alerr.h>

int ps(char *fn, observationBatch ** o);

void errExit(int r)
{
  fputs(errLine, stderr);
  fputc('\n', stderr);
  exit(r);
}

int main(int argc, char **argv)
{
  LIBXML_TEST_VERSION;
  char *schema = NULL;
  if (argc != 3) {
    errExit(error("usage: px2 <input .psv file> <output .xml file>"));
  }

  observationBatch *o;
  int r = ps(argv[1], &o);
  if (r)
    errExit(r);
  r = writeXMLFile(o, argv[2]);
  if (r)
    errExit(r);
  return 0;
}

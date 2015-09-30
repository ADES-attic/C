#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <libxml/tree.h>

#include <ades.h>
#include <alerr.h>

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
  int oc = getopt(argc, argv, "s:");
  if (oc == '?')
    exit(-1);                   // getopt already emitted err msg
  if (oc > 0)
    schema = optarg;
  if (argc - optind != 2) {
    errExit(error
            ("usage: xp [-s schema] <input .xml file> <output .psv file>"));
  }

  observationBatch *o;
  int r = readXMLFile(argv[optind], &o, schema);
  if (r)
    errExit(r);

  // TEST hard code options
  r = writePSVFile(o, argv[optind + 1], PF_MIN, PA_NONE);
  if (r)
    errExit(r);
  return 0;
}

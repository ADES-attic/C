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
            ("usage: px [-s schema] <input .psv file> <output .xml file>"));
  }

  observationBatch *o;
  int r = readPSVFile(argv[optind], &o, schema);
  if (r)
    errExit(r);
  r = writeXMLFile(o, argv[optind + 1]);
  if (r)
    errExit(r);
  return 0;
}

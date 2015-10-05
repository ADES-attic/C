#define _GNU_SOURCE
#include <unistd.h>

#include <ades.h>
#include <globals.h>

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

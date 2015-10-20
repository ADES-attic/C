//
// px -- (P)SV to (X)ML converter
//
// Executable program

#define _GNU_SOURCE
#include <getopt.h>

#include <config.h>
#include <ades.h>
#include <globals.h>

char *msgVersion = "\
px -- PSV to XML converter\n\
Version %s, compiled %s.  Public domain.\n\
";

char *msgUsage = "\
Usage: px {option} <input .psv file> <output .xml file>     convert\n\
       px -v or --version                                   display version\n\
\n\
Option:\n\
  -s or --schema <xsd schema>       validate against xsd schema\n\
";

int main(int argc, char **argv)
{
  LIBXML_TEST_VERSION;

  char *schema = NULL;

  char *sOpt = "vs:";
  struct option lOpt[] = {
    {"version", no_argument, 0, 'v'},
    {"schema", required_argument, 0, 's'},
    {0, 0, 0, 0}
  };

  while (1) {
    int oc = getopt_long(argc, argv, sOpt, lOpt, NULL);
    switch (oc) {
    case '?':
      exit(-1);                 // getopt already emitted err msg
    case 'v':
      printf(msgVersion, VERSION, __DATE__);
      exit(0);
    case 's':
      schema = optarg;
      break;
    case -1:
      if (argc - optind == 2)
        goto c;
      // else fall through
    default:
      fputs(msgUsage, stderr);
      exit(-1);
    }
  }

 c:;
  observationBatch *o;
  int r = readPSVFile(argv[optind], &o, schema);
  if (r)
    errExit(r);
  if (r = writeXMLFile(o, argv[optind + 1]))
    errExit(r);
  return 0;
}

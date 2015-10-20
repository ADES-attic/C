//
// xp -- (X)ML to (P)SV converter
//
// Executable program

#define _GNU_SOURCE
#include <string.h>
#include <getopt.h>

#include <config.h>
#include <ades.h>
#include <globals.h>

char *msgVersion = "\
xp -- XML to PSV converter\n\
Version %s, compiled %s.  Public domain.\n\
";

char *msgUsage = "\
Usage: xp {options} <input .xml file> <output .psv file>     convert\n\
       xp -v or --version                                    display version\n\
\n\
Options:\n\
  -s or --schema <xsd schema>       validate against xsd schema\n\
  -m or --min                       minimal format -- not \"default PSV\"\n\
  -a or --align [none|data|hdrs]    column alignment:\n\
                 none:  no alignment\n\
                 data:  align field data\n\
                 hdrs:  align column headers with field data\n\
";

int main(int argc, char **argv)
{
  LIBXML_TEST_VERSION;
  char *schema = NULL;
  char *as = NULL;
  int pf = PF_DEFAULT;
  int pa = PA_DATA;

  char *sOpt = "vs:ma:";
  struct option lOpt[] = {
    {"version", no_argument, 0, 'v'},
    {"schema", required_argument, 0, 's'},
    {"min", no_argument, 0, 'm'},
    {"align", required_argument, 0, 'a'},
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
    case 'm':
      pf = PF_MIN;
      break;
    case 'a':
      if (!strcmp(optarg, "none"))
        pa = PA_NONE;
      else if (!strcmp(optarg, "data"))
        pa = PA_DATA;
      else if (!strcmp(optarg, "hdrs"))
        pa = PA_HDRS;
      else {
        fputs("invalid alignment option\n", stderr);
        goto d;
      }
      break;
    case -1:
      if (argc - optind == 2)
        goto c;
      // else fall through
    default:
 d:
      fputs(msgUsage, stderr);
      exit(-1);
    }
  }

 c:;
  observationBatch *o;
  int r = readXMLFile(argv[optind], &o, schema);
  if (r)
    errExit(r);
  if (r = writePSVFile(o, argv[optind + 1], pf, pa))
    errExit(r);
  return 0;
}

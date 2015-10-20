//
// mx -- (M)PC 80 column to (X)ML
//
// Executable program

#define _GNU_SOURCE
#include <math.h>
#include <getopt.h>

#include <ades.h>
#include <globals.h>

char *msgVersion = "\
mx -- MPC 80 column to XML converter\n\
Compiled %s, no version number yet.  Public domain.\n\
";
char *msgUsage = "\
Usage: mx {options} <input MPC 80 col file> <output .xml file>\n\
       mx -h or --help            display help\n\
       mx -v or --version         display program version\n\
\n\
Options:\n\
       --logsnr <logSNR to apply to all observations>\n\
       --schema or -s <xsd schema file to validate against>\n\
";
char *msgHelp = "\
Input can be either MPC 80 column submission format with submission headers\n\
or general MPC 80 column data with or without headers.\n\
\n\
The ADES logSNR field is required for XML submissions but is not present in\n\
the 80 column format.  To allow xm to format XML in submission format\n\
you must use the --logsnr option and specify a value for this field.\n\
The --logsnr switch thus serves to indicate whether xm will format data in\n\
the submission format or not.\n\
\n\
With --logsnr:\n\
* The required logSNR element is generated for each observation.\n\
* The required observation count element is generated, either from the NUM\n\
  header keyword if present or by counting observations if not present.\n\
* Discovery indicators and references are disallowed.\n\
\n\
Witout --logsnr:\n\
* No logSNR elements are generated.\n\
* Observation count elements are generated only if the NUM header keyword\n\
  is present.\n\
* Elements precTime, precRA, and precDec are generated.\n\
\n\
Example schema are at https://github.com/IAU-ADES/xsd.  There are\n\
currently two, `exchange.xml` and `submit.xml`.  The exchange schema\n\
is general purpose and suitable for representing archival observations.\n\
The submit schema encodes requirements for data submitted to the MPC\n\
as new observations.\n\
";

int main(int argc, char **argv)
{
  LIBXML_TEST_VERSION;
  char *schema = NULL;
  char *logsnr = NULL;
  char *sOpt = "hvs:";          // no short for logsnr
  struct option lOpt[] = {
    {"help", no_argument, 0, 'h'},
    {"version", no_argument, 0, 'v'},
    {"schema", required_argument, 0, 's'},
    {"logsnr", required_argument, 0, 'g'},
    {0, 0, 0, 0}
  };

  int ox;                       // opt index
  int r;
  observationBatch *o;
  while (1) {
    int oc = getopt_long(argc, argv, sOpt, lOpt, &ox);
    switch (oc) {
    case '?':
      exit(-1);                 // getopt already emitted err msg
    case 'h':
      puts(msgUsage);
      puts(msgHelp);
      exit(0);
    case 'v':
      printf(msgVersion, __DATE__);
      exit(0);
    case 's':
      schema = optarg;
      break;
    case 'g':
      logsnr = optarg;
      break;
    case -1:
      if (argc - optind == 2)
        goto w;
      // else fall through
    default:
      fputs(msgUsage, stderr);
      exit(-1);
    }
  }
 w:
  if (r = readMPC80File(argv[optind], &o, schema, logsnr))
    errExit(r);
  if (r = writeXMLFile(o, argv[optind + 1]))
    errExit(r);
  return 0;
}

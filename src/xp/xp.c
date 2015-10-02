#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
  char *as = NULL;
  int pf = PF_DEFAULT;
  int pa = PA_DATA;
  int oc;
  while ((oc = getopt(argc, argv, "a:ms:")) != -1)
    switch (oc) {
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
        fputs("invalid option for -a\n", stderr);
        exit(-1);
      }
      break;
    default:
      exit(-1);                 // getopt already emitted err msg
    }
  if (argc - optind != 2)
    errExit(error("\
usage: xp {options} <input .xml file> <output .psv file>\n\
options:\n\
  -s <xsd schema>       validate against xsd schema\n\
  -m                    minimal format -- not \"default PSV\"\n\
  -a [none|data|hdrs]   column alignment:\n\
      none:  no alignment\n\
      data:  align field data\n\
      hdrs:  align column headers with field data\n\
"));

  observationBatch *o;
  int r = readXMLFile(argv[optind], &o, schema);
  if (r)
    errExit(r);

  // TEST hard code options
  r = writePSVFile(o, argv[optind + 1], pf, pa);
  if (r)
    errExit(r);
  return 0;
}

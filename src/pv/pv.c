//
// pv -- (P)SV (V)alidate
//
// Executable program

#include <getopt.h>

#include <config.h>
#include <globals.h>

char *msgVersion = "\
pv -- PSV Validator\n\
Version %s, compiled %s.  Public domain.\n\
";

char *msgUsage = "\
Usage: pv <psv file> <XML schema file>     validate\n\
       pv -v or --version                  display version\n\
";

int main(int argc, char **argv)
{
  LIBXML_TEST_VERSION;

  char *sOpt = "v";
  struct option lOpt[] = {
    {"version", no_argument, 0, 'v'},
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
    case -1:
      if (argc - optind == 2)
        goto v;
      // else fall through
    default:
      fputs(msgUsage, stderr);
      exit(-1);
    }
  }

 v:;
  xmlDocPtr doc;
  int r = pt(argv[1], &doc);
  if (r)
    errExit(r);
  if (r = tv(doc, argv[2]))
    exit(r);                    // xml functions emit error messages
  printf("validate okay\n");
}

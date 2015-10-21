//
// xv -- (X)ML (V)alidate
//
// Executable program

#include <getopt.h>

#include <config.h>
#include <commit.h>
#include <globals.h>

char *msgVersion = "\
xv -- XML Validator -- Public domain.\n\
Version %s %s\n\
Compiled: %s.\n\
";

char *msgUsage = "\
Usage: xv <xml file> <schema file>     validate\n\
       xv -v or --version              display version\n\
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
      printf(msgVersion, VERSION, COMMIT, __DATE__);
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
  xmlDocPtr doc = xmlParseFile(argv[1]);
  if (!doc)
    exit(-1);                   // xml functions emit err msgs
  int r = tv(doc, argv[2]);
  if (r)
    exit(r);
  printf("validate okay\n");
}

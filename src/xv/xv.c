
#include <libxml/tree.h>

extern int tv(xmlDocPtr doc, char *schema);

int main(int argc, char **argv)
{
  if (argc != 3) {
    printf("usage: xv <xml file> <schema file>\n");
    exit(-1);
  }

  xmlDocPtr doc = xmlParseFile(argv[1]);
  if (!doc) {
    exit(-1);                   // xml functions emit err msgs
  }

  int r = tv(doc, argv[2]);
  if (r)
    exit(r);

  printf("validate okay\n");
}

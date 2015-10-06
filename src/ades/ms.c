//
// ms.c -- (M)PC 80 to C (S)truct conversion.
//
// Implements library function readMPC80File().

#include <ades.h>
#include <globals.h>

int readMPC80File(char *fn, observationBatch ** obs, char *schema)
{
  xmlDocPtr doc;
  int r = mt(fn, &doc);
  if (r)
    return r;
  // printf("read okay\n");

  if (schema) {
    r = tv(doc, schema);
    if (r) {
      // xmlSaveFormatFileEnc("debug.xml", doc, "UTF-8", 1);
      exit(r);                  // xml functions emit error messages
    }
    // printf("validate okay\n");
  }

  ts(doc, obs);
}

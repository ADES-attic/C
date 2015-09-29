
#include <libxml/tree.h>

#include <ds.h>

int pt(char *fn, xmlDocPtr * pDoc);
int tv(xmlDocPtr doc, char *schema);
void ts(xmlDocPtr doc, observationBatch ** obs);

int readPSVFile(char *fn, observationBatch ** obs, char *schema)
{
  xmlDocPtr doc;
  int r = pt(fn, &doc);
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

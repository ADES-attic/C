#include <libxml/tree.h>
#include <libxml/xmlschemas.h>

// tv (tree validate) validates an in-memory doc tree against a schema.
//
// Error messages from xml functions go to stderr.
//
// tv returns 0 on successful validate, non-zero on any failure.

int tv(xmlDocPtr doc, char *schema)
{
  xmlSchemaParserCtxtPtr pCtx = xmlSchemaNewParserCtxt(schema);
  if (!pCtx)
    return -1;

  xmlSchemaPtr sPtr = xmlSchemaParse(pCtx);
  if (!sPtr)
    return -1;

  xmlSchemaValidCtxtPtr schemaCtx = xmlSchemaNewValidCtxt(sPtr);
  if (!schemaCtx)
    return -1;

  if (xmlSchemaValidateDoc(schemaCtx, doc))
    return -1;

  return 0;
}

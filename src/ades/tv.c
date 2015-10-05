//
// tv.c -- libxml2 (T)ree (V)alidate
//
// Implements function tv().
//
// This function intended for use primarily within the ades library and not
// as an API function.  The argument is a libxml2 xmlDocPtr and the idea is
// that API users should not be required to handle libxml2 data types.

#include <libxml/xmlschemas.h>

#include <globals.h>

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

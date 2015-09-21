#include <alerr.h>
#include <ds.h>

#include <libxml/tree.h>

// globals
xmlDocPtr doc;
xmlNodePtr root_node;

int writeXMLFile(observationBatch * o, char *fn)
{
  if (!o)
    return error("no data");

  doc = xmlNewDoc(BAD_CAST "1.0");
  root_node = xmlNewNode(NULL, BAD_CAST "observationBatch");
  root_node->line = 1;
  xmlDocSetRootElement(doc, root_node);

  for (int i = 0; i < o->len; i++)
    addSeg(o->segments + i);

  xmlSaveFormatFileEnc(fn, doc, "UTF-8", 1);
  return 0;
}

int addSeg(observationSegment * os)
{
}

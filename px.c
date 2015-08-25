#include <libxml/tree.h>

int main()
{
  LIBXML_TEST_VERSION;

  xmlDocPtr doc = xmlNewDoc(BAD_CAST "1.0");
  xmlNodePtr root_node = xmlNewNode(NULL, BAD_CAST "ObservationBatch");
  xmlDocSetRootElement(doc, root_node);

  xmlNodePtr obs = xmlNewChild(root_node, NULL, BAD_CAST "observations", NULL);
  xmlNodePtr opt = xmlNewChild(obs, NULL, BAD_CAST "optical", NULL);
  xmlNewChild(opt, NULL, BAD_CAST "trkSub", BAD_CAST "P01abcd");

  xmlSaveFormatFile("-", doc, 1);
}

//
// ts.c -- libxml2 (T)ree to C (S)truct conversion.
//
// Implements function ts().
//
// This function intended for use primarily within the ades library and not
// as an API function.  The argument is a libxml2 xmlDocPtr and the idea is
// that API users should not be required to handle libxml2 data types.

#define _GNU_SOURCE
#include <string.h>

#include <globals.h>

void addH1Observation(xmlNodePtr xH1, observationContext * ctx)
{
  ctxObservation *obs = calloc(1, sizeof(ctxObservation));
  ctx->observation = obs;
  xmlNodePtr xH2 = xmlFirstElementChild(xH1);
  while (xH2) {
    const char *kwd = xH2->name;
    char *txt = strdup(xmlNodeGetContent(xH2));
    if (!strcmp(kwd, "count"))
      obs->count = txt;
    else if (!strcmp(kwd, "type"))
      obs->type = txt;
    xH2 = xmlNextElementSibling(xH2);
  }
}

void addH1Observatory(xmlNodePtr xH1, observationContext * ctx)
{
  ctxObservatory *obs = calloc(1, sizeof(ctxObservatory));
  ctx->observatory = obs;
  xmlNodePtr xH2 = xmlFirstElementChild(xH1);
  while (xH2) {
    const char *kwd = xH2->name;
    char *txt = strdup(xmlNodeGetContent(xH2));
    if (!strcmp(kwd, "mpcCode"))
      obs->mpcCode = txt;
    else if (!strcmp(kwd, "name"))
      obs->name = txt;
    xH2 = xmlNextElementSibling(xH2);
  }
}

void addH1Contact(xmlNodePtr xH1, observationContext * ctx)
{
  ctxContact *con = calloc(1, sizeof(ctxContact));
  ctx->contact = con;
  xmlNodePtr xH2 = xmlFirstElementChild(xH1);
  while (xH2) {
    const char *kwd = xH2->name;
    char *txt = strdup(xmlNodeGetContent(xH2));
    if (!strcmp(kwd, "name"))
      con->name = txt;
    else if (!strcmp(kwd, "address"))
      con->address = txt;
    else if (!strcmp(kwd, "ackMessage"))
      con->ackMessage = txt;
    else if (!strcmp(kwd, "ackEmail"))
      con->ackEmail = txt;
    xH2 = xmlNextElementSibling(xH2);
  }
}

ctxNameList *nameList(xmlNodePtr xH1)
{
  ctxNameList *nl = calloc(1, sizeof(ctxNameList));
  xmlNodePtr xH2 = xmlFirstElementChild(xH1);
  while (xH2) {
    char *txt = strdup(xmlNodeGetContent(xH2));
    if (!strcmp(xH2->name, "name")) {
      int last = nl->len;
      nl->len++;
      nl->names = realloc(nl->names, nl->len * sizeof(dVal));
      nl->names[last] = txt;
    }
    xH2 = xmlNextElementSibling(xH2);
  }
  return nl;
}

void addH1Observers(xmlNodePtr xH1, observationContext * ctx)
{
  ctx->observers = nameList(xH1);
}

void addH1Measurers(xmlNodePtr xH1, observationContext * ctx)
{
  ctx->measurers = nameList(xH1);
}

void addH1Telescope(xmlNodePtr xH1, observationContext * ctx)
{
  ctxTelescope *tel = calloc(1, sizeof(ctxTelescope));
  ctx->telescope = tel;
  xmlNodePtr xH2 = xmlFirstElementChild(xH1);
  while (xH2) {
    const char *kwd = xH2->name;
    char *txt = strdup(xmlNodeGetContent(xH2));
    if (!strcmp(kwd, "name"))
      tel->name = txt;
    else if (!strcmp(kwd, "design"))
      tel->design = txt;
    else if (!strcmp(kwd, "aperture"))
      tel->aperture = txt;
    else if (!strcmp(kwd, "detector"))
      tel->detector = txt;
    else if (!strcmp(kwd, "fRatio"))
      tel->fRatio = txt;
    else if (!strcmp(kwd, "filter"))
      tel->filter = txt;
    else if (!strcmp(kwd, "arraySize"))
      tel->arraySize = txt;
    else if (!strcmp(kwd, "pixelScale"))
      tel->pixelScale = txt;
    xH2 = xmlNextElementSibling(xH2);
  }
}

void addH1Software(xmlNodePtr xH1, observationContext * ctx)
{
  ctxSoftware *sw = calloc(1, sizeof(ctxSoftware));
  ctx->software = sw;
  xmlNodePtr xH2 = xmlFirstElementChild(xH1);
  while (xH2) {
    const char *kwd = xH2->name;
    char *txt = strdup(xmlNodeGetContent(xH2));
    if (!strcmp(kwd, "astrometry"))
      sw->astrometry = txt;
    else if (!strcmp(kwd, "fitOrder"))
      sw->fitOrder = txt;
    else if (!strcmp(kwd, "photometry"))
      sw->photometry = txt;
    else if (!strcmp(kwd, "objectDetection"))
      sw->objectDetection = txt;
    xH2 = xmlNextElementSibling(xH2);
  }
}

void addH1Comment(xmlNodePtr xH1, observationContext * ctx)
{
  ctx->comment = strdup(xmlNodeGetContent(xH1));
}

void addH1Coinvestigators(xmlNodePtr xH1, observationContext * ctx)
{
  ctx->coinvestigators = nameList(xH1);
}

void addH1Collaborators(xmlNodePtr xH1, observationContext * ctx)
{
  ctx->collaborators = nameList(xH1);
}

void addH1FundingSource(xmlNodePtr xH1, observationContext * ctx)
{
  ctx->fundingSource = strdup(xmlNodeGetContent(xH1));
}

void addH1OrbProd(xmlNodePtr xH1, observationContext * ctx)
{
  ctx->orbProd = strdup(xmlNodeGetContent(xH1));
}

void addH1PhotProd(xmlNodePtr xH1, observationContext * ctx)
{
  ctx->photProd = strdup(xmlNodeGetContent(xH1));
}

void addSegCtx(xmlNodePtr xSeg, observationSegment * seg)
{
  observationContext *ctx = calloc(1, sizeof(observationContext));
  seg->ctx = ctx;
  seg->obs = NULL;
  xmlNodePtr xH1 = xmlFirstElementChild(xSeg);
  while (xH1) {
    if (!strcmp(xH1->name, "observation"))
      addH1Observation(xH1, ctx);

    else if (!strcmp(xH1->name, "observatory"))
      addH1Observatory(xH1, ctx);

    else if (!strcmp(xH1->name, "contact"))
      addH1Contact(xH1, ctx);

    else if (!strcmp(xH1->name, "observers"))
      addH1Observers(xH1, ctx);

    else if (!strcmp(xH1->name, "measurers"))
      addH1Measurers(xH1, ctx);

    else if (!strcmp(xH1->name, "telescope"))
      addH1Telescope(xH1, ctx);

    else if (!strcmp(xH1->name, "software"))
      addH1Software(xH1, ctx);

    else if (!strcmp(xH1->name, "comment"))
      addH1Comment(xH1, ctx);

    else if (!strcmp(xH1->name, "coinvestigators"))
      addH1Coinvestigators(xH1, ctx);

    else if (!strcmp(xH1->name, "collaborators"))
      addH1Collaborators(xH1, ctx);

    else if (!strcmp(xH1->name, "fundingSource"))
      addH1FundingSource(xH1, ctx);

    else if (!strcmp(xH1->name, "orbProd"))
      addH1OrbProd(xH1, ctx);

    else if (!strcmp(xH1->name, "photProd"))
      addH1PhotProd(xH1, ctx);

    xH1 = xmlNextElementSibling(xH1);
  }
}

void addObs(xmlNodePtr xObs, obsRec obs)
{
  xmlNodePtr xFld = xmlFirstElementChild(xObs);
  while (xFld) {
    const char *fld = xFld->name;
    // usually we just drop memory but there are potential lots
    // of these things so we'll clean up as as we go along.
    xmlChar *x = xmlNodeGetContent(xFld);
    char *txt = strdup(x);
    xmlFree(x);

    int n = fldNum(fld);
    if (n >= F_PERMID)
      obs[n] = txt;

    xFld = xmlNextElementSibling(xFld);
  }
}

void addSegObsList(xmlNodePtr xSeg, observationSegment * seg)
{
  obsList *ol = malloc(sizeof(obsList));
  seg->ctx = NULL;
  seg->obs = ol;
  int len = 0;
  int cap = 4;
  obsRec *obs = calloc(cap, sizeof(obsRec));
  xmlNodePtr xObs = xmlFirstElementChild(xSeg);
  while (xObs) {
    if (!strcmp(xObs->name, "optical") || !strcmp(xObs->name, "radar")) {
      // grow list as needed
      if (len == cap) {
        cap *= 2;
        obs = realloc(obs, cap * sizeof(obsRec));
        memset(obs + len, 0, len * sizeof(obsRec));
      }
      addObs(xObs, obs[len]);
      len++;
    }
    xObs = xmlNextElementSibling(xObs);
  }
  // complete obsList
  if (len < cap)
    obs = realloc(obs, len * sizeof(obsRec));
  ol->len = len;
  ol->observations = obs;
}

void ts(xmlDocPtr doc, observationBatch ** obs)
{
  observationBatch *o = calloc(1, sizeof(observationBatch));
  *obs = o;
  xmlNodePtr root_node = xmlDocGetRootElement(doc);
  xmlNodePtr x1 = xmlFirstElementChild(root_node);
  xmlNodePtr xSeg = x1;
  printf("ts: x1->name: %s\n", x1->name);
  if (!strcmp(x1->name, "adesVersion")) {
    o->adesVersion = strdup(xmlNodeGetContent(x1));
    xSeg = xmlNextElementSibling(x1);
  }
  while (xSeg) {
    int last = o->len;
    o->len++;
    o->segments = realloc(o->segments, o->len * sizeof(observationSegment));
    observationSegment *seg = o->segments + last;
    if (!strcmp(xSeg->name, "observationContext"))
      addSegCtx(xSeg, seg);
    else if (!strcmp(xSeg->name, "observations"))
      addSegObsList(xSeg, seg);
    xSeg = xmlNextElementSibling(xSeg);
  }
}

//
// sx.c -- C (S)truct to (X)ML conversion.
//
// Implements API function writeXMLFile().

#include <string.h>

#include <globals.h>

int addCtxObservation(ctxObservation * obs, xmlNodePtr xCtx)
{
  xmlNodePtr xObs = xmlNewChild(xCtx, NULL, "observation", NULL);
  if (obs->count)
    xmlNewChild(xObs, NULL, "count", obs->count);
  if (obs->type)
    xmlNewChild(xObs, NULL, "type", obs->type);
  return 0;
}

int addCtxObservatory(ctxObservatory * obs, xmlNodePtr xCtx)
{
  xmlNodePtr xObs = xmlNewChild(xCtx, NULL, "observatory", NULL);
  if (obs->mpcCode)
    xmlNewChild(xObs, NULL, "mpcCode", obs->mpcCode);
  if (obs->name)
    xmlNewChild(xObs, NULL, "name", obs->name);
  return 0;
}

int addCtxContact(ctxContact * con, xmlNodePtr xCtx)
{
  xmlNodePtr xCon = xmlNewChild(xCtx, NULL, "contact", NULL);
  if (con->name)
    xmlNewChild(xCon, NULL, "name", con->name);
  if (con->address)
    xmlNewChild(xCon, NULL, "address", con->address);
  if (con->ackMessage)
    xmlNewChild(xCon, NULL, "ackMessage", con->ackMessage);
  if (con->ackEmail)
    xmlNewChild(xCon, NULL, "ackEmail", con->ackEmail);
  return 0;
}

int addCtxObservers(ctxNameList * nl, xmlNodePtr xCtx)
{
  xmlNodePtr xObs = xmlNewChild(xCtx, NULL, "observers", NULL);
  for (int i = 0; i < nl->len; i++)
    xmlNewChild(xObs, NULL, "name", nl->names[i]);
  return 0;
}

int addCtxMeasurers(ctxNameList * nl, xmlNodePtr xCtx)
{
  xmlNodePtr xMea = xmlNewChild(xCtx, NULL, "measurers", NULL);
  for (int i = 0; i < nl->len; i++)
    xmlNewChild(xMea, NULL, "name", nl->names[i]);
  return 0;
}

int addCtxTelescope(ctxTelescope * tel, xmlNodePtr xCtx)
{
  xmlNodePtr xTel = xmlNewChild(xCtx, NULL, "telescope", NULL);
  if (tel->name)
    xmlNewChild(xTel, NULL, "name", tel->name);
  if (tel->design)
    xmlNewChild(xTel, NULL, "design", tel->design);
  if (tel->aperture)
    xmlNewChild(xTel, NULL, "aperture", tel->aperture);
  if (tel->detector)
    xmlNewChild(xTel, NULL, "detector", tel->detector);
  if (tel->fRatio)
    xmlNewChild(xTel, NULL, "fRatio", tel->fRatio);
  if (tel->filter)
    xmlNewChild(xTel, NULL, "filter", tel->filter);
  if (tel->arraySize)
    xmlNewChild(xTel, NULL, "arraySize", tel->arraySize);
  if (tel->pixelScale)
    xmlNewChild(xTel, NULL, "pixelScale", tel->pixelScale);
  return 0;
}

int addCtxSoftware(ctxSoftware * sw, xmlNodePtr xCtx)
{
  xmlNodePtr xSW = xmlNewChild(xCtx, NULL, "software", NULL);
  if (sw->astrometry)
    xmlNewChild(xSW, NULL, "astrometry", sw->astrometry);
  if (sw->fitOrder)
    xmlNewChild(xSW, NULL, "fitOrder", sw->fitOrder);
  if (sw->photometry)
    xmlNewChild(xSW, NULL, "photometry", sw->photometry);
  if (sw->objectDetection)
    xmlNewChild(xSW, NULL, "objectDetection", sw->objectDetection);
  return 0;
}

int addCtxComment(dVal com, xmlNodePtr xCtx)
{
  xmlNewChild(xCtx, NULL, "comment", com);
  return 0;
}

int addCtxCoinv(ctxNameList * nl, xmlNodePtr xCtx)
{
  xmlNodePtr xCoinv = xmlNewChild(xCtx, NULL, "coinvestigators", NULL);
  for (int i = 0; i < nl->len; i++)
    xmlNewChild(xCoinv, NULL, "name", nl->names[i]);
  return 0;
}

int addCtxCollab(ctxNameList * nl, xmlNodePtr xCtx)
{
  xmlNodePtr xCollab = xmlNewChild(xCtx, NULL, "collaborators", NULL);
  for (int i = 0; i < nl->len; i++)
    xmlNewChild(xCollab, NULL, "name", nl->names[i]);
  return 0;
}

int addCtxFunding(dVal fun, xmlNodePtr xCtx)
{
  xmlNewChild(xCtx, NULL, "fundingSource", fun);
  return 0;
}

int addCtxOrbProd(dVal orb, xmlNodePtr xCtx)
{
  xmlNewChild(xCtx, NULL, "orbProd", orb);
  return 0;
}

int addCtxPhotProd(dVal phot, xmlNodePtr xCtx)
{
  xmlNewChild(xCtx, NULL, "photProd", phot);
  return 0;
}

int addCtx(observationContext * ctx)
{
  xmlNodePtr xCtx = xmlNewChild(root_node, NULL, "observationContext", NULL);
  int r;
  if (ctx->observation) {
    r = addCtxObservation(ctx->observation, xCtx);
    if (r)
      return r;
  }
  if (ctx->observatory) {
    r = addCtxObservatory(ctx->observatory, xCtx);
    if (r)
      return r;
  }
  if (ctx->contact) {
    r = addCtxContact(ctx->contact, xCtx);
    if (r)
      return r;
  }
  if (ctx->observers) {
    r = addCtxObservers(ctx->observers, xCtx);
    if (r)
      return r;
  }
  if (ctx->measurers) {
    r = addCtxMeasurers(ctx->measurers, xCtx);
    if (r)
      return r;
  }
  if (ctx->telescope) {
    r = addCtxTelescope(ctx->telescope, xCtx);
    if (r)
      return r;
  }
  if (ctx->software) {
    r = addCtxSoftware(ctx->software, xCtx);
    if (r)
      return r;
  }
  if (ctx->comment) {
    r = addCtxComment(ctx->comment, xCtx);
    if (r)
      return r;
  }
  if (ctx->coinvestigators) {
    r = addCtxCoinv(ctx->coinvestigators, xCtx);
    if (r)
      return r;
  }
  if (ctx->collaborators) {
    r = addCtxCollab(ctx->collaborators, xCtx);
    if (r)
      return r;
  }
  if (ctx->fundingSource) {
    r = addCtxFunding(ctx->fundingSource, xCtx);
    if (r)
      return r;
  }
  if (ctx->orbProd) {
    r = addCtxOrbProd(ctx->orbProd, xCtx);
    if (r)
      return r;
  }
  if (ctx->photProd) {
    r = addCtxPhotProd(ctx->photProd, xCtx);
    if (r)
      return r;
  }
}

int addObsList(obsList * ol)
{
  xmlNodePtr xObs = xmlNewChild(root_node, NULL, "observations", NULL);
  int r;
  xmlNodePtr xRec;
  obsRec *rec = ol->observations;
  for (int i = 0; i < ol->len; i++, rec++) {
    if (rec[0][F_MODE] && !strcmp(rec[0][F_MODE], "Radar"))
      xRec = xmlNewChild(xObs, NULL, "radar", NULL);
    else
      xRec = xmlNewChild(xObs, NULL, "optical", NULL);

    for (int fld = F_PERMID; fld < F_NUM; fld++)
      if (rec[0][fld])
        xmlNewChild(xRec, NULL, fldNames[fld],
                    xmlEncodeEntitiesReentrant(doc, rec[0][fld]));
  }
  return 0;
}

int addSeg(observationSegment * os)
{
  if (os->ctx)
    return addCtx(os->ctx);

  return addObsList(os->obs);
}

int writeXMLFile(observationBatch * o, char *fn)
{
  if (!o)
    return error("no data");

  doc = xmlNewDoc("1.0");
  root_node = xmlNewNode(NULL, "observationBatch");
  xmlDocSetRootElement(doc, root_node);

  for (int i = 0; i < o->len; i++)
    addSeg(o->segments + i);

  xmlSaveFormatFileEnc(fn, doc, "UTF-8", 1);
  return 0;
}

#include <string.h>

#include <ades.h>
#include <alerr.h>
#include <ds.h>

#include <libxml/tree.h>

// globals
xmlDocPtr doc;
xmlNodePtr root_node;

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
  for (int i = 0; i < ol->len; i++) {
    obsRec *rec = ol->observations + i;

    if (rec->mode && !strcmp(rec->mode, "Radar"))
      xRec = xmlNewChild(xObs, NULL, "radar", NULL);
    else
      xRec = xmlNewChild(xObs, NULL, "optical", NULL);

    // group of names in order specified in "Default PSV"
    if (rec->permID)
      xmlNewChild(xRec, NULL, "permID", rec->permID);
    if (rec->provID)
      xmlNewChild(xRec, NULL, "provID", rec->provID);
    if (rec->trkSub)
      xmlNewChild(xRec, NULL, "trkSub", rec->trkSub);
    if (rec->mode)
      xmlNewChild(xRec, NULL, "mode", rec->mode);
    if (rec->stn)
      xmlNewChild(xRec, NULL, "stn", rec->stn);
    if (rec->prg)
      xmlNewChild(xRec, NULL, "prg", rec->prg);
    if (rec->obsTime)
      xmlNewChild(xRec, NULL, "obsTime", rec->obsTime);
    if (rec->ra)
      xmlNewChild(xRec, NULL, "ra", rec->ra);
    if (rec->dec)
      xmlNewChild(xRec, NULL, "dec", rec->dec);
    if (rec->astCat)
      xmlNewChild(xRec, NULL, "astCat", rec->astCat);
    if (rec->rmsRA)
      xmlNewChild(xRec, NULL, "rmsRA", rec->rmsRA);
    if (rec->rmsDec)
      xmlNewChild(xRec, NULL, "rmsDec", rec->rmsDec);
    if (rec->rmsCorr)
      xmlNewChild(xRec, NULL, "rmsCorr", rec->rmsCorr);
    if (rec->mag)
      xmlNewChild(xRec, NULL, "mag", rec->mag);
    if (rec->band)
      xmlNewChild(xRec, NULL, "band", rec->band);
    if (rec->photCat)
      xmlNewChild(xRec, NULL, "photCat", rec->photCat);
    if (rec->rmsMag)
      xmlNewChild(xRec, NULL, "rmsMag", rec->rmsMag);
    if (rec->photAp)
      xmlNewChild(xRec, NULL, "photAp", rec->photAp);
    if (rec->logSNR)
      xmlNewChild(xRec, NULL, "logSNR", rec->logSNR);
    if (rec->seeing)
      xmlNewChild(xRec, NULL, "seeing", rec->seeing);
    if (rec->exp)
      xmlNewChild(xRec, NULL, "exp", rec->exp);
    if (rec->notes)
      xmlNewChild(xRec, NULL, "notes", rec->notes);

    // remaining names in order of definition in ADES
    if (rec->obsID)
      xmlNewChild(xRec, NULL, "obsID", rec->obsID);
    if (rec->trkID)
      xmlNewChild(xRec, NULL, "trkID", rec->trkID);
    if (rec->deltaRA)
      xmlNewChild(xRec, NULL, "deltaRA", rec->deltaRA);
    if (rec->deltaDec)
      xmlNewChild(xRec, NULL, "deltaDec", rec->deltaDec);
    if (rec->raStar)
      xmlNewChild(xRec, NULL, "raStar", rec->raStar);
    if (rec->frame)
      xmlNewChild(xRec, NULL, "frame", rec->frame);
    if (rec->nucMag)
      xmlNewChild(xRec, NULL, "nucMag", rec->nucMag);
    if (rec->rmsFit)
      xmlNewChild(xRec, NULL, "rmsFit", rec->rmsFit);
    if (rec->nStars)
      xmlNewChild(xRec, NULL, "nStars", rec->nStars);
    if (rec->ref)
      xmlNewChild(xRec, NULL, "ref", rec->ref);
    if (rec->disc)
      xmlNewChild(xRec, NULL, "disc", rec->disc);
    if (rec->subFmt)
      xmlNewChild(xRec, NULL, "subFmt", rec->subFmt);
    if (rec->precTime)
      xmlNewChild(xRec, NULL, "precTime", rec->precTime);
    if (rec->precRA)
      xmlNewChild(xRec, NULL, "precRA", rec->precRA);
    if (rec->precDec)
      xmlNewChild(xRec, NULL, "precDec", rec->precDec);
    if (rec->uncTime)
      xmlNewChild(xRec, NULL, "uncTime", rec->uncTime);
    if (rec->sys)
      xmlNewChild(xRec, NULL, "sys", rec->sys);
    if (rec->ctr)
      xmlNewChild(xRec, NULL, "ctr", rec->ctr);
    if (rec->pos1)
      xmlNewChild(xRec, NULL, "pos1", rec->pos1);
    if (rec->pos2)
      xmlNewChild(xRec, NULL, "pos2", rec->pos2);
    if (rec->pos3)
      xmlNewChild(xRec, NULL, "pos3", rec->pos3);
    if (rec->posCov11)
      xmlNewChild(xRec, NULL, "posCov11", rec->posCov11);
    if (rec->posCov12)
      xmlNewChild(xRec, NULL, "posCov12", rec->posCov12);
    if (rec->posCov13)
      xmlNewChild(xRec, NULL, "posCov13", rec->posCov13);
    if (rec->posCov22)
      xmlNewChild(xRec, NULL, "posCov22", rec->posCov22);
    if (rec->posCov23)
      xmlNewChild(xRec, NULL, "posCov23", rec->posCov23);
    if (rec->posCov33)
      xmlNewChild(xRec, NULL, "posCov33", rec->posCov33);

    // radar specific
    if (rec->valRad)
      xmlNewChild(xRec, NULL, "valRad", rec->valRad);
    if (rec->rmsRad)
      xmlNewChild(xRec, NULL, "rmsRad", rec->rmsRad);
    if (rec->com)
      xmlNewChild(xRec, NULL, "com", rec->com);
    if (rec->frq)
      xmlNewChild(xRec, NULL, "frq", rec->frq);
    if (rec->trx)
      xmlNewChild(xRec, NULL, "trx", rec->trx);
    if (rec->rcv)
      xmlNewChild(xRec, NULL, "rcv", rec->rcv);

    // residual specific
    if (rec->resRA)
      xmlNewChild(xRec, NULL, "resRA", rec->resRA);
    if (rec->resDec)
      xmlNewChild(xRec, NULL, "resDec", rec->resDec);
    if (rec->orbID)
      xmlNewChild(xRec, NULL, "orbID", rec->orbID);
    if (rec->selAst)
      xmlNewChild(xRec, NULL, "selAst", rec->selAst);
    if (rec->sigRA)
      xmlNewChild(xRec, NULL, "sigRA", rec->sigRA);
    if (rec->sigDec)
      xmlNewChild(xRec, NULL, "sigDec", rec->sigDec);
    if (rec->sigCorr)
      xmlNewChild(xRec, NULL, "sigCorr", rec->sigCorr);
    if (rec->sigTime)
      xmlNewChild(xRec, NULL, "sigTime", rec->sigTime);
    if (rec->biasRA)
      xmlNewChild(xRec, NULL, "biasRA", rec->biasRA);
    if (rec->biasDec)
      xmlNewChild(xRec, NULL, "biasDec", rec->biasDec);
    if (rec->biasTime)
      xmlNewChild(xRec, NULL, "biasTime", rec->biasTime);
    if (rec->resMag)
      xmlNewChild(xRec, NULL, "resMag", rec->resMag);
    if (rec->selPhot)
      xmlNewChild(xRec, NULL, "selPhot", rec->selPhot);
    if (rec->sigMag)
      xmlNewChild(xRec, NULL, "sigMag", rec->sigMag);
    if (rec->biasMag)
      xmlNewChild(xRec, NULL, "biasMag", rec->biasMag);
    if (rec->photMod)
      xmlNewChild(xRec, NULL, "photMod", rec->photMod);

    // radar residual specific
    if (rec->resRad)
      xmlNewChild(xRec, NULL, "resRad", rec->resRad);
    if (rec->selRad)
      xmlNewChild(xRec, NULL, "selRad", rec->selRad);
    if (rec->sigRad)
      xmlNewChild(xRec, NULL, "sigRad", rec->sigRad);

    // final field of "Default PSV"
    if (rec->remarks)
      xmlNewChild(xRec, NULL, "remarks", rec->remarks);
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

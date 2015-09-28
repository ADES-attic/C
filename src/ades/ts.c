#define _GNU_SOURCE
#include <string.h>

#include <libxml/tree.h>

#include <ds.h>

// globals
xmlDocPtr doc;
xmlNodePtr root_node;

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

void addCtx(xmlNodePtr xSeg, observationSegment * seg)
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

void addObs(xmlNodePtr xObs, obsRec * obs)
{
  xmlNodePtr xFld = xmlFirstElementChild(xObs);
  while (xFld) {
    const char *fld = xFld->name;
    // usually we just drop memory but there are potential lots
    // of these things so we'll clean up as as we go along.
    xmlChar *x = xmlNodeGetContent(xFld);
    char *txt = strdup(x);
    xmlFree(x);

    // group of names in order specified in "Default PSV"
    if (!strcmp(fld, "permID"))
      obs->permID = txt;

    else if (!strcmp(fld, "provID"))
      obs->provID = txt;

    else if (!strcmp(fld, "trkSub"))
      obs->trkSub = txt;

    else if (!strcmp(fld, "mode"))
      obs->mode = txt;

    else if (!strcmp(fld, "stn"))
      obs->stn = txt;

    else if (!strcmp(fld, "prg"))
      obs->prg = txt;

    else if (!strcmp(fld, "obsTime"))
      obs->obsTime = txt;

    else if (!strcmp(fld, "ra"))
      obs->ra = txt;

    else if (!strcmp(fld, "dec"))
      obs->dec = txt;

    else if (!strcmp(fld, "astCat"))
      obs->astCat = txt;

    else if (!strcmp(fld, "rmsRA"))
      obs->rmsRA = txt;

    else if (!strcmp(fld, "rmsDec"))
      obs->rmsDec = txt;

    else if (!strcmp(fld, "rmsCorr"))
      obs->rmsCorr = txt;

    else if (!strcmp(fld, "mag"))
      obs->mag = txt;

    else if (!strcmp(fld, "band"))
      obs->band = txt;

    else if (!strcmp(fld, "photCat"))
      obs->photCat = txt;

    else if (!strcmp(fld, "rmsMag"))
      obs->rmsMag = txt;

    else if (!strcmp(fld, "photAp"))
      obs->photAp = txt;

    else if (!strcmp(fld, "logSNR"))
      obs->logSNR = txt;

    else if (!strcmp(fld, "seeing"))
      obs->seeing = txt;

    else if (!strcmp(fld, "exp"))
      obs->exp = txt;

    else if (!strcmp(fld, "notes"))
      obs->notes = txt;

    // remaining names in order of definition in ADES
    else if (!strcmp(fld, "obsID"))
      obs->obsID = txt;

    else if (!strcmp(fld, "trkID"))
      obs->trkID = txt;

    else if (!strcmp(fld, "deltaRA"))
      obs->deltaRA = txt;

    else if (!strcmp(fld, "deltaDec"))
      obs->deltaDec = txt;

    else if (!strcmp(fld, "raStar"))
      obs->raStar = txt;

    else if (!strcmp(fld, "decStar"))
      obs->decStar = txt;

    else if (!strcmp(fld, "frame"))
      obs->frame = txt;

    else if (!strcmp(fld, "nucMag"))
      obs->nucMag = txt;

    else if (!strcmp(fld, "rmsFit"))
      obs->rmsFit = txt;

    else if (!strcmp(fld, "nStars"))
      obs->nStars = txt;

    else if (!strcmp(fld, "ref"))
      obs->ref = txt;

    else if (!strcmp(fld, "disc"))
      obs->disc = txt;

    else if (!strcmp(fld, "subFmt"))
      obs->subFmt = txt;

    else if (!strcmp(fld, "precTime"))
      obs->precTime = txt;

    else if (!strcmp(fld, "precRA"))
      obs->precRA = txt;

    else if (!strcmp(fld, "precDec"))
      obs->precDec = txt;

    else if (!strcmp(fld, "uncTime"))
      obs->uncTime = txt;

    else if (!strcmp(fld, "sys"))
      obs->sys = txt;

    else if (!strcmp(fld, "ctr"))
      obs->ctr = txt;

    else if (!strcmp(fld, "pos1"))
      obs->pos1 = txt;

    else if (!strcmp(fld, "pos2"))
      obs->pos2 = txt;

    else if (!strcmp(fld, "pos3"))
      obs->pos3 = txt;

    else if (!strcmp(fld, "posCov11"))
      obs->posCov11 = txt;

    else if (!strcmp(fld, "posCov12"))
      obs->posCov12 = txt;

    else if (!strcmp(fld, "posCov13"))
      obs->posCov13 = txt;

    else if (!strcmp(fld, "posCov22"))
      obs->posCov22 = txt;

    else if (!strcmp(fld, "posCov23"))
      obs->posCov23 = txt;

    else if (!strcmp(fld, "posCov33"))
      obs->posCov33 = txt;

    // radar specific
    else if (!strcmp(fld, "valRad"))
      obs->valRad = txt;

    else if (!strcmp(fld, "rmsRad"))
      obs->rmsRad = txt;

    else if (!strcmp(fld, "com"))
      obs->com = txt;

    else if (!strcmp(fld, "frq"))
      obs->frq = txt;

    else if (!strcmp(fld, "trx"))
      obs->trx = txt;

    else if (!strcmp(fld, "rcv"))
      obs->rcv = txt;

    // residual specific
    else if (!strcmp(fld, "resRA"))
      obs->resRA = txt;

    else if (!strcmp(fld, "resDec"))
      obs->resDec = txt;

    else if (!strcmp(fld, "orbID"))
      obs->orbID = txt;

    else if (!strcmp(fld, "selASt"))
      obs->selAst = txt;

    else if (!strcmp(fld, "sigRA"))
      obs->sigRA = txt;

    else if (!strcmp(fld, "sigDec"))
      obs->sigDec = txt;

    else if (!strcmp(fld, "sigCorr"))
      obs->sigCorr = txt;

    else if (!strcmp(fld, "sigTime"))
      obs->sigTime = txt;

    else if (!strcmp(fld, "biasRA"))
      obs->biasRA = txt;

    else if (!strcmp(fld, "biasDec"))
      obs->biasDec = txt;

    else if (!strcmp(fld, "biasTime"))
      obs->biasTime = txt;

    else if (!strcmp(fld, "resMag"))
      obs->resMag = txt;

    else if (!strcmp(fld, "selPhot"))
      obs->selPhot = txt;

    else if (!strcmp(fld, "sigMag"))
      obs->sigMag = txt;

    else if (!strcmp(fld, "biasMag"))
      obs->biasMag = txt;

    else if (!strcmp(fld, "photMod"))
      obs->photMod = txt;

    // radar residual specific
    else if (!strcmp(fld, "resRad"))
      obs->resRad = txt;

    else if (!strcmp(fld, "selRad"))
      obs->selRad = txt;

    else if (!strcmp(fld, "sigRad"))
      obs->sigRad = txt;

    // final field of "Default PSV"
    else if (!strcmp(fld, "remarks"))
      obs->remarks = txt;

    xFld = xmlNextElementSibling(xFld);
  }
}

void addObsList(xmlNodePtr xSeg, observationSegment * seg)
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
      addObs(xObs, obs + len);
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
  observationBatch *o = malloc(sizeof(observationBatch));
  *obs = o;
  xmlNodePtr root_node = xmlDocGetRootElement(doc);
  xmlNodePtr xSeg = xmlFirstElementChild(root_node);
  while (xSeg) {
    int last = o->len;
    o->len++;
    o->segments = realloc(o->segments, o->len * sizeof(observationSegment));
    observationSegment *seg = o->segments + last;
    if (!strcmp(xSeg->name, "observationContext"))
      addCtx(xSeg, seg);
    else if (!strcmp(xSeg->name, "observations"))
      addObsList(xSeg, seg);
    xSeg = xmlNextElementSibling(xSeg);
  }
}

#include <stdio.h>

#include <ades.h>
#include <alerr.h>
#include <tables.h>

FILE *fpsv;
_Bool defp;
int algn;

void writeH2(char *kwd, char *val)
{
  fprintf(fpsv, "! %s %s\n", kwd, val);
}

void writeCtxObservation(ctxObservation * obs)
{
  fputs("# observation\n", fpsv);

  if (obs->count)
    writeH2("observation", obs->count);

  if (obs->type)
    writeH2("type", obs->type);
}

void writeCtxObservatory(ctxObservatory * obs)
{
  fputs("# observatory\n", fpsv);

  if (obs->mpcCode)
    writeH2("mpcCode", obs->mpcCode);

  if (obs->name)
    writeH2("name", obs->name);
}

void writeCtxContact(ctxContact * con)
{
  fputs("# contact\n", fpsv);

  if (con->name)
    writeH2("name", con->name);

  if (con->address)
    writeH2("address", con->address);

  if (con->ackMessage)
    writeH2("ackMessage", con->ackMessage);

  if (con->ackEmail)
    writeH2("ackEmail", con->ackEmail);
}

void writeNameList(ctxNameList * nl)
{
  for (int i = 0; i < nl->len; i++)
    writeH2("name", nl->names[i]);
}

void writeCtxObservers(ctxNameList * nl)
{
  fputs("# observers\n", fpsv);
  writeNameList(nl);
}

void writeCtxMeasurers(ctxNameList * nl)
{
  fputs("# measurers\n", fpsv);
  writeNameList(nl);
}

void writeCtxTelescope(ctxTelescope * tel)
{
  if (tel->name)
    writeH2("name", tel->name);
  if (tel->design)
    writeH2("design", tel->design);
  if (tel->aperture)
    writeH2("aperture", tel->aperture);
  if (tel->detector)
    writeH2("detector", tel->detector);
  if (tel->fRatio)
    writeH2("fRatio", tel->fRatio);
  if (tel->filter)
    writeH2("filter", tel->filter);
  if (tel->arraySize)
    writeH2("arraySize", tel->arraySize);
  if (tel->pixelScale)
    writeH2("pixelScale", tel->pixelScale);
}

void writeCtxSoftware(ctxSoftware * sw)
{
  if (sw->astrometry)
    writeH2("astrometry", sw->astrometry);
  if (sw->fitOrder)
    writeH2("fitOrder", sw->fitOrder);
  if (sw->photometry)
    writeH2("photometry", sw->photometry);
  if (sw->objectDetection)
    writeH2("objectDetection", sw->objectDetection);
}

void writeCtxComment(dVal com)
{
  fprintf(fpsv, "# comment %s\n", com);
}

void writeCtxCoinv(ctxNameList * nl)
{
  fputs("# coinvestigators\n", fpsv);
  writeNameList(nl);
}

void writeCtxCollab(ctxNameList * nl)
{
  fputs("# collaborators\n", fpsv);
  writeNameList(nl);
}

void writeCtxFunding(dVal fun)
{
  fprintf(fpsv, "# fundingSource %s\n", fun);
}

void writeCtxOrbProd(dVal orb)
{
  fprintf(fpsv, "# orbProd %s\n", orb);
}

void writeCtxPhotProd(dVal phot)
{
  fprintf(fpsv, "# photProd %s\n", phot);
}

void writeCtx(observationContext * ctx)
{
  if (ctx->observation)
    writeCtxObservation(ctx->observation);

  if (ctx->observatory)
    writeCtxObservatory(ctx->observatory);

  if (ctx->contact)
    writeCtxContact(ctx->contact);

  if (ctx->observers)
    writeCtxObservers(ctx->observers);

  if (ctx->measurers)
    writeCtxMeasurers(ctx->measurers);

  if (ctx->telescope)
    writeCtxTelescope(ctx->telescope);

  if (ctx->software)
    writeCtxSoftware(ctx->software);

  if (ctx->comment)
    writeCtxComment(ctx->comment);

  if (ctx->coinvestigators)
    writeCtxCoinv(ctx->coinvestigators);

  if (ctx->collaborators)
    writeCtxCollab(ctx->collaborators);

  if (ctx->fundingSource)
    writeCtxFunding(ctx->fundingSource);

  if (ctx->orbProd)
    writeCtxOrbProd(ctx->orbProd);

  if (ctx->photProd)
    writeCtxPhotProd(ctx->photProd);
}

void setColList(_Bool colList[], obsList * ol)
{
  colList[F_PERMID] = 1; // always required

  if (defp) {
    for (int c = F_PROVID; c <= F_NOTES; c++)
      colList[c] = 1;
    colList[F_REMARKS] = 1;
  }

  obsRec *r = ol->observations;
  for (int i = 0; i < ol->len; i++, r++) {
    if (!defp) {
    // group of names in order specified in "Default PSV"
    if (r->provID)
      colList[F_PROVID] = 1;
    if (r->trkSub)
      colList[F_TRKSUB] = 1;
    if (r->mode)
      colList[F_MODE] = 1;
    if (r->stn)
      colList[F_STN] = 1;
    if (r->prg)
      colList[F_PRG] = 1;
    if (r->obsTime)
      colList[F_OBSTIME] = 1;
    if (r->ra)
      colList[F_RA] = 1;
    if (r->dec)
      colList[F_DEC] = 1;
    if (r->astCat)
      colList[F_ASTCAT] = 1;
    if (r->rmsRA)
      colList[F_RA] = 1;
    if (r->rmsDec)
      colList[F_DEC] = 1;
    if (r->rmsCorr)
      colList[F_RMSCORR] = 1;
    if (r->mag)
      colList[F_MAG] = 1;
    if (r->band)
      colList[F_BAND] = 1;
    if (r->photCat)
      colList[F_PHOTCAT] = 1;
    if (r->rmsMag)
      colList[F_RMSMAG] = 1;
    if (r->photAp)
      colList[F_PHOTAP] = 1;
    if (r->logSNR)
      colList[F_LOGSNR] = 1;
    if (r->seeing)
      colList[F_SEEING] = 1;
    if (r->exp)
      colList[F_EXP] = 1;
    if (r->notes)
      colList[F_NOTES] = 1;
    }

    // remaining names in order of definition in ADES
    if (r->obsID)
      colList[F_OBSID] = 1;
    if (r->trkID)
      colList[F_TRKID] = 1;
    if (r->deltaRA)
      colList[F_DELTARA] = 1;
    if (r->deltaDec)
      colList[F_DELTADEC] = 1;
    if (r->raStar)
      colList[F_RASTAR] = 1;
    if (r->decStar)
      colList[F_DECSTAR] = 1;
    if (r->frame)
      colList[F_FRAME] = 1;
    if (r->nucMag)
      colList[F_NUCMAG] = 1;
    if (r->rmsFit)
      colList[F_RMSFIT] = 1;
    if (r->nStars)
      colList[F_NSTARS] = 1;
    if (r->ref)
      colList[F_REF] = 1;
    if (r->disc)
      colList[F_DISC] = 1;
    if (r->subFmt)
      colList[F_SUBFMT] = 1;
    if (r->precTime)
      colList[F_PRECTIME] = 1;
    if (r->precRA)
      colList[F_PRECRA] = 1;
    if (r->precDec)
      colList[F_PRECDEC] = 1;
    if (r->uncTime)
      colList[F_UNCTIME] = 1;
    if (r->sys)
      colList[F_SYS] = 1;
    if (r->ctr)
      colList[F_CTR] = 1;
    if (r->pos1)
      colList[F_POS1] = 1;
    if (r->pos2)
      colList[F_POS2] = 1;
    if (r->pos3)
      colList[F_POS3] = 1;
    if (r->posCov11)
      colList[F_POSCOV11] = 1;
    if (r->posCov12)
      colList[F_POSCOV12] = 1;
    if (r->posCov13)
      colList[F_POSCOV13] = 1;
    if (r->posCov22)
      colList[F_POSCOV22] = 1;
    if (r->posCov23)
      colList[F_POSCOV23] = 1;
    if (r->posCov33)
      colList[F_POSCOV33] = 1;

    // radar specific
    if (r->valRad)
      colList[F_VALRAD] = 1;
    if (r->rmsRad)
      colList[F_RMSRAD] = 1;
    if (r->com)
      colList[F_COM] = 1;
    if (r->frq)
      colList[F_FRQ] = 1;
    if (r->trx)
      colList[F_TRX] = 1;
    if (r->rcv)
      colList[F_RCV] = 1;

    // residual specific
    if (r->resRA)
      colList[F_RESRA] = 1;
    if (r->resDec)
      colList[F_RESDEC] = 1;
    if (r->orbID)
      colList[F_ORBID] = 1;
    if (r->selAst)
      colList[F_SELAST] = 1;
    if (r->sigRA)
      colList[F_SIGRA] = 1;
    if (r->sigDec)
      colList[F_SIGDEC] = 1;
    if (r->sigCorr)
      colList[F_SIGCORR] = 1;
    if (r->sigTime)
      colList[F_SIGTIME] = 1;
    if (r->biasRA)
      colList[F_BIASRA] = 1;
    if (r->biasDec)
      colList[F_BIASDEC] = 1;
    if (r->biasTime)
      colList[F_BIASTIME] = 1;
    if (r->resMag)
      colList[F_RESMAG] = 1;
    if (r->selPhot)
      colList[F_SELPHOT] = 1;
    if (r->sigMag)
      colList[F_SIGMAG] = 1;
    if (r->biasMag)
      colList[F_BIASMAG] = 1;
    if (r->photMod)
      colList[F_PHOTMOD] = 1;

    // radar residual specific
    if (r->resRad)
      colList[F_RESRAD] = 1;
    if (r->selRad)
      colList[F_SELRAD] = 1;
    if (r->sigRad)
      colList[F_SIGRAD] = 1;

    // final field of "Default PSV"
    if (r->remarks)
      colList[F_REMARKS] = 1;
  }
}

void writeObsNoAlign(_Bool colList[], obsList * ol) {
  // write column headers, smashed
  fputs("permID", fpsv);
  for (int c = F_PROVID; c < F_NUM; c++)
    if (colList[c]) {
      fputc('|', fpsv);
      fputs(fldNames[c], fpsv);
    }
  fputc('\n', fpsv);

  // write records, smashed
  obsRec *r = ol->observations;
  for (int i = 0; i < ol->len; i++, r++) {
    if (r->permID)
      if (defp)
        printf("%7s", r->permID);
      else
        puts(r->permID);
  }
}

void writeObsAligned(_Bool colList[], obsList * ol) {}

void writeObsList(obsList * ol)
{
  _Bool colList[F_NUM];
  setColList(colList, ol);
  if (algn == PA_NONE)
    writeObsNoAlign(colList, ol);
  else
    writeObsAligned(colList, ol);
}

void writeSeg(observationSegment * os)
{
  if (os->ctx)
    return writeCtx(os->ctx);

  return writeObsList(os->obs);
}

int writePSVFile(observationBatch * o, char *fn,
                 _Bool defaultPSV, int pipeAlign)
{
  if (!o)
    return error("no data");

  fpsv = fopen(fn, "r");
  if (!fpsv)
    return error1("can't open %s", fn);

  defp = defaultPSV;
  algn = pipeAlign;

  if (o->len)
    writeSeg(o->segments);
  for (int i = 1; i < o->len; i++) {
    fputc('\n', fpsv);
    writeSeg(o->segments + i);
  }
}

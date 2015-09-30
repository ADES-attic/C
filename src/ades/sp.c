#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ades.h>
#include <alerr.h>
#include <tables.h>

FILE *fpsv;
_Bool defp;
int algn;
char line[512];

void writeH2(char *kwd, char *val)
{
  fprintf(fpsv, "! %s %s\n", kwd, val);
}

void writeCtxObservation(ctxObservation * obs)
{
  fputs("# observation\n", fpsv);

  if (obs->count)
    writeH2("count", obs->count);

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
  fputs("# telescope\n", fpsv);
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
  fputs("# software\n", fpsv);
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
  colList[F_PERMID] = 1;        // always required

  if (defp) {
    for (int c = F_PROVID; c <= F_NOTES; c++)
      colList[c] = 1;
    colList[F_REMARKS] = 1;
  }

  obsRec *r = ol->observations;
  for (int i = 0; i < ol->len; i++, r++)
    for (int n = F_PERMID; n < F_NUM; n++)
      if (r[0][n])
        colList[n] = 1;
}

void writeS(dVal v, int width)
{
  fprintf(fpsv, "|%*s", width, v ? v : "");
}

void writeD(dVal v, int width, int dpos)
{
  if (!v) {
    fprintf(fpsv, "|%*s", width, ""); // blank
    return;
  }
  char *tp;
  double d = strtod(v, &tp);
  if (tp == v) {                // can't parse number
    fprintf(fpsv, "|%-*s", width, v); // left justify unparsable text
    return;
  }
  // try reformatting
  snprintf(line, sizeof line, "|%*.*f", width, width - dpos, d);
  char *p = strchr(line, '.');  // did we get a decimal point?
  if (!p) {
    fprintf(fpsv, "|%-*s", width, v); // left justify unformattable text
    return;
  }
  // blank right zeros
  for (char *z = strchr(p, 0) - 1; *z == '0'; z--)
    *z = ' ';
  fputs(line, fpsv);
}

void defpNoAlign(_Bool colList[], obsRec r)
{
  dVal v = r[F_PERMID];
  fprintf(fpsv, "%7s", v ? v : "");

  writeS(r[F_PROVID], -11);
  writeS(r[F_TRKSUB], 7);
  writeS(r[F_MODE], 3);
  writeS(r[F_STN], -4);
  writeS(r[F_PRG], 2);
  writeS(r[F_OBSTIME], -23);
  writeD(r[F_RA], 11, 4);
  writeD(r[F_DEC], 11, 4);
  writeS(r[F_ASTCAT], 3);
  writeD(r[F_RMSRA], 6, 3);
  writeD(r[F_RMSDEC], 6, 3);
  writeD(r[F_RMSCORR], 5, 2);
  writeD(r[F_MAG], 5, 3);
  writeS(r[F_BAND], 3);
  writeS(r[F_PHOTCAT], 3);
  writeD(r[F_RMSMAG], 4, 2);
  writeD(r[F_PHOTAP], 4, 3);
  writeD(r[F_LOGSNR], 4, 2);
  writeD(r[F_SEEING], 3, 2);
  writeS(r[F_EXP], 4);
  writeS(r[F_NOTES], -5);

  for (int c = F_OBSID; c < F_REMARKS; c++)
    if (colList[c]) {
      fputc('|', fpsv);
      if (r[c])
        fputs(r[c], fpsv);
    }

  v = r[F_REMARKS];
  fprintf(fpsv, "|%s\n", v ? v : "");
}

void minNoAlign(_Bool colList[], obsRec r)
{
  dVal v = r[F_PERMID];
  if (v)
    fputs(v, fpsv);
  for (int c = F_PROVID; c < F_NUM; c++)
    if (colList[c]) {
      fputc('|', fpsv);
      if (r[c])
        fputs(r[c], fpsv);
    }
  fputc('\n', fpsv);
}

void writeObsNoAlign(_Bool colList[], obsList * ol)
{
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
    if (defp)
      defpNoAlign(colList, r[0]);
    else
      minNoAlign(colList, r[0]);
  }
}

void writeObsAligned(_Bool colList[], obsList * ol)
{
}

void writeObsList(obsList * ol)
{
  _Bool colList[F_NUM];
  memset(colList, 0, sizeof colList);

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

  fpsv = fopen(fn, "w");
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

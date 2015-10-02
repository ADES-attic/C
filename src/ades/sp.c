#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libxml/tree.h>

#include <ades.h>
#include <alerr.h>
#include <tables.h>

FILE *fpsv;
_Bool defp;
int algn;
char line[512];
char line2[512];

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

void fmtS(dVal v, int width)
{
  snprintf(line, sizeof line, "|%*s", width, v ? v : "");
}

void writeS(dVal v, int width)
{
  fmtS(v, width);
  fputs(line, fpsv);
}

int fmtLenS(dVal v, int width)
{
  fmtS(v, width);
  return strlen(line) - 1;      // -1 to not count pipe character
}

void fmtD(dVal v, int width, int dpos)
{
  if (!v) {
    snprintf(line, sizeof line, "|%*s", width, ""); // blank
    return;
  }
  char *dp = strchr(v, '.');
  if (!dp) {
    // no decimal point: right align to dpos, then left align in field
    snprintf(line2, sizeof line2, "%*s", dpos - 1, v);
    snprintf(line, sizeof line, "|%-*s", width, line2);
    return;
  }
  // decimal point present.
  // count characters before decimal point
  int before = 0;
  int all = strlen(v);
  char *p = v;
  while (*p != '.') {
    int len = all;
    xmlGetUTF8Char(p, &len);
    before++;
    all -= len;
    p += len;
  }
  // pad left to align decimal point
  int pad = dpos - before - 1;
  if (pad < 0)
    pad = 0;
  snprintf(line2, sizeof line2, "%*s%s", pad, "", v);
  snprintf(line, sizeof line, "|%-*s", width, line2);
}

void writeD(dVal v, int width, int dpos)
{
  fmtD(v, width, dpos);
  fputs(line, fpsv);
}

int fmtLenD(dVal v, int width, int dpos)
{
  fmtD(v, width, dpos);
  return strlen(line) - 1;      // -1 to not count pipe character
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

void expandCol(int *n, int width)
{
  if (width > *n)
    *n = width;
}

void defpExpand(int colWidth[], _Bool colList[], obsRec r)
{
  expandCol(colWidth + F_PERMID, fmtLenS(r[F_PERMID], 7));
  expandCol(colWidth + F_PROVID, fmtLenS(r[F_PROVID], -11));
  expandCol(colWidth + F_TRKSUB, fmtLenS(r[F_TRKSUB], 7));
  expandCol(colWidth + F_MODE, fmtLenS(r[F_MODE], 3));
  expandCol(colWidth + F_STN, fmtLenS(r[F_STN], -4));
  expandCol(colWidth + F_PRG, fmtLenS(r[F_PRG], 2));
  expandCol(colWidth + F_OBSTIME, fmtLenS(r[F_OBSTIME], -23));
  expandCol(colWidth + F_RA, fmtLenD(r[F_RA], 11, 4));
  expandCol(colWidth + F_DEC, fmtLenD(r[F_DEC], 11, 4));
  expandCol(colWidth + F_ASTCAT, fmtLenS(r[F_ASTCAT], 3));
  expandCol(colWidth + F_RMSRA, fmtLenD(r[F_RMSRA], 6, 3));
  expandCol(colWidth + F_RMSDEC, fmtLenD(r[F_RMSDEC], 6, 3));
  expandCol(colWidth + F_RMSCORR, fmtLenD(r[F_RMSCORR], 5, 2));
  expandCol(colWidth + F_MAG, fmtLenD(r[F_MAG], 5, 3));
  expandCol(colWidth + F_BAND, fmtLenS(r[F_BAND], 3));
  expandCol(colWidth + F_PHOTCAT, fmtLenS(r[F_PHOTCAT], 3));
  expandCol(colWidth + F_RMSMAG, fmtLenD(r[F_RMSMAG], 4, 2));
  expandCol(colWidth + F_PHOTAP, fmtLenD(r[F_PHOTAP], 4, 3));
  expandCol(colWidth + F_LOGSNR, fmtLenD(r[F_LOGSNR], 4, 2));
  expandCol(colWidth + F_SEEING, fmtLenD(r[F_SEEING], 3, 2));
  expandCol(colWidth + F_EXP, fmtLenS(r[F_EXP], 4));
  expandCol(colWidth + F_NOTES, fmtLenS(r[F_NOTES], -5));

  for (int c = F_OBSID; c < F_REMARKS; c++)
    if (colList[c] && r[c])
      expandCol(colWidth + c, strlen(r[c]));

  if (r[F_REMARKS])
    expandCol(colWidth + F_REMARKS, strlen(r[F_REMARKS]));
}

void minExpand(int colWidth[], _Bool colList[], obsRec r)
{
  for (int c = 0; c < F_NUM; c++)
    if (colList[c] && r[c])
      expandCol(colWidth + c, strlen(r[c]));
}

void setColWidth(int colWidth[], _Bool colList[], obsList * ol)
{
  // if col hdrs are included in the alignment, start with the col hdr widths
  if (algn == PA_HDRS)
    for (int c = 0; c < F_NUM; c++)
      if (colList[c])
        colWidth[c] = strlen(fldNames[c]);

  // then in all cases, iterate over obsRecs and expand to fit data
  obsRec *r = ol->observations;
  for (int i = 0; i < ol->len; i++, r++)
    if (defp)
      defpExpand(colWidth, colList, *r);
    else
      minExpand(colWidth, colList, *r);
}

void writeDefpAlignedRecs(int colWidth[], obsList * ol)
{
  obsRec *r = ol->observations;
  for (int i = 0; i < ol->len; i++, r++) {
    dVal v = r[0][F_PERMID];
    fprintf(fpsv, "%*s", colWidth[F_PERMID], v ? v : "");

    writeS(r[0][F_PROVID], -colWidth[F_PROVID]);
    writeS(r[0][F_TRKSUB], colWidth[F_TRKSUB]);
    writeS(r[0][F_MODE], colWidth[F_MODE]);
    writeS(r[0][F_STN], -colWidth[F_STN]);
    writeS(r[0][F_PRG], colWidth[F_PRG]);
    writeS(r[0][F_OBSTIME], -colWidth[F_OBSTIME]);

    writeD(r[0][F_RA], colWidth[F_RA], 4);
    writeD(r[0][F_DEC], colWidth[F_DEC], 4);

    writeS(r[0][F_ASTCAT], colWidth[F_ASTCAT]);

    writeD(r[0][F_RMSRA], colWidth[F_RMSRA], 3);
    writeD(r[0][F_RMSDEC], colWidth[F_RMSDEC], 3);
    writeD(r[0][F_RMSCORR], colWidth[F_RMSCORR], 2);
    writeD(r[0][F_MAG], colWidth[F_MAG], 3);

    writeS(r[0][F_BAND], colWidth[F_BAND]);
    writeS(r[0][F_PHOTCAT], colWidth[F_PHOTCAT]);

    writeD(r[0][F_RMSMAG], colWidth[F_RMSMAG], 2);
    writeD(r[0][F_PHOTAP], colWidth[F_PHOTAP], 3);
    writeD(r[0][F_LOGSNR], colWidth[F_LOGSNR], 2);
    writeD(r[0][F_SEEING], colWidth[F_SEEING], 2);

    writeS(r[0][F_EXP], colWidth[F_EXP]);
    writeS(r[0][F_NOTES], -colWidth[F_NOTES]);

    for (int c = F_OBSID; c < F_REMARKS; c++)
      if (colWidth[c])
        writeS(r[0][c], colWidth[c]);

    v = r[0][F_REMARKS];
    fprintf(fpsv, "|%s\n", v ? v : "");
  }
}

void writeMinAlignedRecs(int colWidth[], obsList * ol)
{
  obsRec *r = ol->observations;
  for (int i = 0; i < ol->len; i++, r++) {
    dVal v = r[0][F_PERMID];
    fprintf(fpsv, "%*s", colWidth[F_PERMID], v ? v : "");

    for (int c = F_PROVID; c < F_NUM; c++)
      if (colWidth[c])
        writeS(r[0][c], colWidth[c]);

    fputc('\n', fpsv);
  }
}

void writeObsAligned(_Bool colList[], obsList * ol)
{
  int colWidth[F_NUM];
  memset(colWidth, 0, sizeof colWidth);
  setColWidth(colWidth, colList, ol);

  // write column headings
  fprintf(fpsv, "%-*s", colWidth[F_PERMID], "permID");
  if (algn == PA_HDRS) {
    for (int c = F_PROVID; c < F_NUM; c++)
      if (colList[c])
        fprintf(fpsv, "|%-*s", colWidth[c], fldNames[c]);
  } else {
    for (int c = F_PROVID; c < F_NUM; c++)
      if (colList[c])
        fprintf(fpsv, "|%s", fldNames[c]);
  }
  fputc('\n', fpsv);

  // write data
  if (defp)
    writeDefpAlignedRecs(colWidth, ol);
  else
    writeMinAlignedRecs(colWidth, ol);
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

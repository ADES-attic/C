#include <ds.h>

// fldNames must parallel the struct obsRec.
char *fldNames[] = {
  // group of names specified in "Default PSV"
  "permID",
  "provID",
  "trkSub",
  "mode",
  "stn",
  "prg",
  "obsTime",
  "ra",
  "dec",
  "astCat",
  "rmsRA",
  "rmsDec",
  "rmsCorr",
  "mag",
  "band",
  "photCat",
  "rmsMag",
  "photAp",
  "logSNR",
  "seeing",
  "exp",
  "notes",

  // remaining names in order of definition in ADES
  "trkID",
  "obsID",
  "deltaRA",
  "deltaDec",
  "raStar",
  "decStar",
  "frame",
  "nucMag",
  "rmsFit",
  "nStars",
  "ref",
  "disc",
  "subFmt",
  "precTime",
  "precRA",
  "precDec",
  "uncTime",
  "sys",
  "ctr",
  "pos1",
  "pos2",
  "pos3",
  "posCov11",
  "posCov12",
  "posCov13",
  "posCov22",
  "posCov23",
  "posCov33",

  // radar specific
  "valRad",
  "rmsRad",
  "com",
  "frq",
  "trx",
  "rcv",

  // residual specific
  "resRA",
  "resDec",
  "orbID",
  "selAst",
  "sigRA",
  "sigDec",
  "sigCorr",
  "sigTime",
  "biasRA",
  "biasDec",
  "biasTime",
  "resMag",
  "selPhot",
  "sigMag",
  "biasMag",
  "photMod",

  // radar residual specific
  "resRad",
  "selRad",
  "sigRad",

  // final field of "Default PSV"
  "remarks",
};

int nFlds = sizeof fldNames / sizeof *fldNames;

_Bool isFldName(char *col)
{
  // linear search.  common field names are near the beginning.
  // questionable value in a fancier search.
  for (int i = 0; i < nFlds; i++)
    if (!strcmp(fldNames[i], col))
      return 1;
  return 0;
}

void setPermID(obsRec * o, char *p)
{
  o->permID = p;
}

void setProvID(obsRec * o, char *p)
{
  o->provID = p;
}

void setTrkSub(obsRec * o, char *p)
{
  o->trkSub = p;
}

void setMode(obsRec * o, char *p)
{
  o->mode = p;
}

void setStn(obsRec * o, char *p)
{
  o->stn = p;
}

void setPrg(obsRec * o, char *p)
{
  o->prg = p;
}

void setObsTime(obsRec * o, char *p)
{
  o->obsTime = p;
}

void setRA(obsRec * o, char *p)
{
  o->ra = p;
}

void setDec(obsRec * o, char *p)
{
  o->dec = p;
}

void setAstCat(obsRec * o, char *p)
{
  o->astCat = p;
}

void setRMSra(obsRec * o, char *p)
{
  o->rmsRA = p;
}

void setRMSdec(obsRec * o, char *p)
{
  o->rmsDec = p;
}

void setRMScorr(obsRec * o, char *p)
{
  o->rmsCorr = p;
}

void setMag(obsRec * o, char *p)
{
  o->mag = p;
}

void setBand(obsRec * o, char *p)
{
  o->band = p;
}

void setPhotCat(obsRec * o, char *p)
{
  o->photCat = p;
}

void setRMSmag(obsRec * o, char *p)
{
  o->rmsMag = p;
}

void setPhotAp(obsRec * o, char *p)
{
  o->photAp = p;
}

void setLogSNR(obsRec * o, char *p)
{
  o->logSNR = p;
}

void setSeeing(obsRec * o, char *p)
{
  o->seeing = p;
}

void setExp(obsRec * o, char *p)
{
  o->exp = p;
}

void setNotes(obsRec * o, char *p)
{
  o->notes = p;
}

void setObsID(obsRec * o, char *p)
{
  o->obsID = p;
}

void setTrkID(obsRec * o, char *p)
{
  o->trkID = p;
}

void setDeltaRA(obsRec * o, char *p)
{
  o->deltaRA = p;
}

void setDeltaDec(obsRec * o, char *p)
{
  o->deltaDec = p;
}

void setRAstar(obsRec * o, char *p)
{
  o->raStar = p;
}

void setDecStar(obsRec * o, char *p)
{
  o->decStar = p;
}

void setFrame(obsRec * o, char *p)
{
  o->frame = p;
}

void setNucMag(obsRec * o, char *p)
{
  o->nucMag = p;
}

void setRMSfit(obsRec * o, char *p)
{
  o->rmsFit = p;
}

void setNStars(obsRec * o, char *p)
{
  o->nStars = p;
}

void setRef(obsRec * o, char *p)
{
  o->ref = p;
}

void setDisc(obsRec * o, char *p)
{
  o->disc = p;
}

void setSubFmt(obsRec * o, char *p)
{
  o->subFmt = p;
}

void setPrecTime(obsRec * o, char *p)
{
  o->precTime = p;
}

void setPrecRA(obsRec * o, char *p)
{
  o->precRA = p;
}

void setPrecDec(obsRec * o, char *p)
{
  o->precDec = p;
}

void setUncTime(obsRec * o, char *p)
{
  o->uncTime = p;
}

void setSys(obsRec * o, char *p)
{
  o->sys = p;
}

void setCtr(obsRec * o, char *p)
{
  o->ctr = p;
}

void setPos1(obsRec * o, char *p)
{
  o->pos1 = p;
}

void setPos2(obsRec * o, char *p)
{
  o->pos2 = p;
}

void setPos3(obsRec * o, char *p)
{
  o->pos3 = p;
}

void setPosCov11(obsRec * o, char *p)
{
  o->posCov11 = p;
}

void setPosCov12(obsRec * o, char *p)
{
  o->posCov12 = p;
}

void setPosCov13(obsRec * o, char *p)
{
  o->posCov13 = p;
}

void setPosCov22(obsRec * o, char *p)
{
  o->posCov22 = p;
}

void setPosCov23(obsRec * o, char *p)
{
  o->posCov23 = p;
}

void setPosCov33(obsRec * o, char *p)
{
  o->posCov33 = p;
}

void setValRad(obsRec * o, char *p)
{
  o->valRad = p;
}

void setRMSrad(obsRec * o, char *p)
{
  o->rmsRad = p;
}

void setCom(obsRec * o, char *p)
{
  o->com = p;
}

void setFrq(obsRec * o, char *p)
{
  o->frq = p;
}

void setTrx(obsRec * o, char *p)
{
  o->trx = p;
}

void setRcv(obsRec * o, char *p)
{
  o->rcv = p;
}

void setResRA(obsRec * o, char *p)
{
  o->resRA = p;
}

void setResDec(obsRec * o, char *p)
{
  o->resDec = p;
}

void setOrbID(obsRec * o, char *p)
{
  o->orbID = p;
}

void setSelAst(obsRec * o, char *p)
{
  o->selAst = p;
}

void setSigRA(obsRec * o, char *p)
{
  o->sigRA = p;
}

void setSigDec(obsRec * o, char *p)
{
  o->sigDec = p;
}

void setSigCorr(obsRec * o, char *p)
{
  o->sigCorr = p;
}

void setSigTime(obsRec * o, char *p)
{
  o->sigTime = p;
}

void setBiasRA(obsRec * o, char *p)
{
  o->biasRA = p;
}

void setBiasDec(obsRec * o, char *p)
{
  o->biasDec = p;
}

void setBiasTime(obsRec * o, char *p)
{
  o->biasTime = p;
}

void setResMag(obsRec * o, char *p)
{
  o->resMag = p;
}

void setSelPhot(obsRec * o, char *p)
{
  o->selPhot = p;
}

void setSigMag(obsRec * o, char *p)
{
  o->sigMag = p;
}

void setBiasMag(obsRec * o, char *p)
{
  o->biasMag = p;
}

void setPhotMod(obsRec * o, char *p)
{
  o->photMod = p;
}

void setResRad(obsRec * o, char *p)
{
  o->resRad = p;
}

void setSelRad(obsRec * o, char *p)
{
  o->selRad = p;
}

void setSigRad(obsRec * o, char *p)
{
  o->sigRad = p;
}

void setRemarks(obsRec * o, char *p)
{
  o->remarks = p;
}

typedef void (*obsRecSetter) (obsRec *, char *);
obsRecSetter setObsRec[] = {
  setPermID,
  setProvID,
  setTrkSub,
  setMode,
  setStn,
  setPrg,
  setObsTime,
  setRA,
  setDec,
  setAstCat,
  setRMSra,
  setRMSdec,
  setRMScorr,
  setMag,
  setBand,
  setPhotCat,
  setRMSmag,
  setPhotAp,
  setLogSNR,
  setSeeing,
  setExp,
  setNotes,
  setObsID,
  setTrkID,
  setDeltaRA,
  setDeltaDec,
  setRAstar,
  setDecStar,
  setFrame,
  setNucMag,
  setRMSfit,
  setNStars,
  setRef,
  setDisc,
  setSubFmt,
  setPrecTime,
  setPrecRA,
  setPrecDec,
  setUncTime,
  setSys,
  setCtr,
  setPos1,
  setPos2,
  setPos3,
  setPosCov11,
  setPosCov12,
  setPosCov13,
  setPosCov22,
  setPosCov23,
  setPosCov33,
  setValRad,
  setRMSrad,
  setCom,
  setFrq,
  setTrx,
  setRcv,
  setResRA,
  setResDec,
  setOrbID,
  setSelAst,
  setSigRA,
  setSigDec,
  setSigCorr,
  setSigTime,
  setBiasRA,
  setBiasDec,
  setBiasTime,
  setResMag,
  setSelPhot,
  setSigMag,
  setBiasMag,
  setPhotMod,
  setResRad,
  setSelRad,
  setSigRad,
  setRemarks,
};

char *H1Names[] = {
  "observation",
  "observatory",
  "contact",
  "observers",
  "measurers",
  "telescope",
  "software",
  "comment",
  "coinvestigators",
  "collaborators",
  "fundingSource",
  "orbProd",
  "photProd"
};

int nH1Names = sizeof H1Names / sizeof *H1Names;

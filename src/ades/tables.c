//
// tables.c
//
// Really just one table, the list of observation record field names,
// and a function to look up a name and return an index.
//
// Currently intended for use within the ades library.

#include <string.h>

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

int fldNum(const char *col)
{
  // linear search.  common field names are near the beginning.
  // questionable value in a fancier search.
  for (int i = F_PERMID; i < F_NUM; i++)
    if (!strcmp(fldNames[i], col))
      return i;
  return -1;
}

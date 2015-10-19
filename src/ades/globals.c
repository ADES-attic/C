//
// globals.c
//
// Currently intended for use within the ades library.

#include <stdio.h>
#include <string.h>

#include <globals.h>
#include <ds.h>

// common data for reading and parsing files
int lineNum;                    // for error messages
char line[512];                 // buffer for holding a single line
char line2[512];                // for copies of line

xmlDocPtr doc;
xmlNodePtr root_node;

// common error buffer for use within the ades library.
char errLine[512];

int error(char *msg)
{
  strcpy(errLine, msg);
  return -1;
}

int error1(char *msg, char *arg)
{
  snprintf(errLine, sizeof errLine, msg, arg);
  return -1;
}

// exit function for use from executables.
// not to be used from library functions.
void errExit(int r)
{
  if (*errLine) {
    fputs(errLine, stderr);
    fputc('\n', stderr);
  } else
    fprintf(stderr, "Fatal error %d\n.", r);

  exit(r);
}

// List of observation record field names,
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

// look up a name and return an index.
int fldNum(const char *col)
{
  // linear search.  common field names are near the beginning.
  // questionable value in a fancier search.
  for (int i = F_PERMID; i < F_NUM; i++)
    if (!strcmp(fldNames[i], col))
      return i;
  return -1;
}

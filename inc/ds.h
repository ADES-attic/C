typedef char *dVal;             // null terminated UTF-8

typedef struct {
  dVal permID;
  dVal provID;
  dVal trkSub;
  dVal obsID;
  dVal trkID;
  dVal mode;
  dVal stn;
  dVal prg;
  dVal obsTime;
  dVal ra;
  dVal dec;
  dVal deltaRA;
  dVal deltaDec;
  dVal raStar;
  dVal decStar;
  dVal frame;
  dVal astCat;
  dVal rmsRA;
  dVal rmsDec;
  dVal rmsCorr;
  dVal mag;
  dVal band;
  dVal photCat;
  dVal rmsMag;
  dVal photAp;
  dVal nucMag;
  dVal logSNR;
  dVal seeing;
  dVal exp;
  dVal rmsFit;
  dVal nStars;
  dVal ref;
  dVal disc;
  dVal subFmt;
  dVal precTime;
  dVal precRA;
  dVal precDec;
  dVal uncTime;
  dVal notes;
  dVal remarks;
  dVal sys;
  dVal ctr;
  dVal pos1;
  dVal pos2;
  dVal pos3;
  dVal posCov11;
  dVal posCov12;
  dVal posCov13;
  dVal posCov22;
  dVal posCov23;
  dVal posCov33;

  // residual specific
  dVal resRA;
  dVal resDec;
  dVal orbID;
  dVal selAst;
  dVal sigRA;
  dVal sigDec;
  dVal sigCorr;
  dVal sigTime;
  dVal biasRA;
  dVal biasDec;
  dVal biasTime;
  dVal resMag;
  dVal selPhot;
  dVal sigMag;
  dVal biasMag;
  dVal photMod;
} opticalRec;

typedef struct {
  dVal valRad;
  dVal rmsRad;
  dVal com;
  dVal frq;
  dVal trx;
  dVal rcv;
  dVal orbProd;
  dVal photProd;
  dVal resRA;
  dVal resDec;
  dVal orbID;
  dVal selAst;
  dVal sigRA;
  dVal sigDec;
  dVal sigCorr;
  dVal sigTime;
  dVal biasRA;
  dVal biasDec;
  dVal biasTime;
  dVal resMag;
  dVal selPhot;
  dVal sigMag;
  dVal biasMag;
  dVal photMod;

  // residual specific
  dVal resRad;
  dVal selRad;
  dVal sigRad;
} radarRec;

// one member must have valid data, the other must be NULL;
typedef struct {
  opticalRec *o;
  radarRec *r;
} obsRec;

// a list of observations
typedef struct {
  int len;                      // number of obRecs in array
  obsRec *observations;         // an array
} obsList;

typedef struct {
  dVal count;                   // number of observations
  dVal type;                    // target type, NEOCP, TNO...
} ctxObservation;

typedef struct {
  dVal mpcCode;
  dVal name;
} ctxObservatory;

typedef struct {
  dVal name;
  dVal address;
  dVal ackMessage;
  dVal ackEmail;
} ctxContact;

typedef struct {
  int len;
  dVal *names;
} ctxNameList;

typedef struct {
  dVal name;
  dVal design;
  dVal aperture;
  dVal detector;
  dVal fRatio;
  dVal filter;
  dVal pixelScale;
} ctxTelescope;

typedef struct {
  dVal astrometry;
  dVal fitOrder;
  dVal photometry;
  dVal objectDetection;
} ctxSoftware;

typedef struct {
  ctxObservation observation;
  ctxObservatory observatory;
  ctxContact contact;
  ctxNameList observers;
  ctxNameList measurers;
  ctxTelescope telescope;
  ctxSoftware software;
  dVal comment;
  ctxNameList coinvestigators;
  ctxNameList collaborators;
  dVal fundingSource;
  dVal orbProd;
  dVal photProd;
} observationContext;

typedef struct {
  observationContext ctx;
  obsList obs;
} mpcDataSet;

// a list of mpcDataSets
typedef struct {
  int len;
  mpcDataSet *sets;
} mpcSubmission;

#ifndef DS_H
#define DS_H

typedef char *dVal;             // null terminated UTF-8

typedef struct {
  // group of names in order specified in "Default PSV"
  dVal permID;
  dVal provID;
  dVal trkSub;
  dVal mode;
  dVal stn;
  dVal prg;
  dVal obsTime;
  dVal ra;
  dVal dec;
  dVal astCat;
  dVal rmsRA;
  dVal rmsDec;
  dVal rmsCorr;
  dVal mag;
  dVal band;
  dVal photCat;
  dVal rmsMag;
  dVal photAp;
  dVal logSNR;
  dVal seeing;
  dVal exp;
  dVal notes;

  // remaining names in order of definition in ADES
  dVal obsID;
  dVal trkID;
  dVal deltaRA;
  dVal deltaDec;
  dVal raStar;
  dVal decStar;
  dVal frame;
  dVal nucMag;
  dVal rmsFit;
  dVal nStars;
  dVal ref;
  dVal disc;
  dVal subFmt;
  dVal precTime;
  dVal precRA;
  dVal precDec;
  dVal uncTime;
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

  // radar specific
  dVal valRad;
  dVal rmsRad;
  dVal com;
  dVal frq;
  dVal trx;
  dVal rcv;

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

  // radar residual specific
  dVal resRad;
  dVal selRad;
  dVal sigRad;

  // final field of "Default PSV"
  dVal remarks;
} obsRec;

enum fldNum {
  // group of names in order specified in "Default PSV"
  F_PERMID,
  F_PROVID,
  F_TRKSUB,
  F_MODE,
  F_STN,
  F_PRG,
  F_OBSTIME,
  F_RA,
  F_DEC,
  F_ASTCAT,
  F_RMSRA,
  F_RMSDEC,
  F_RMSCORR,
  F_MAG,
  F_BAND,
  F_PHOTCAT,
  F_RMSMAG,
  F_PHOTAP,
  F_LOGSNR,
  F_SEEING,
  F_EXP,
  F_NOTES,

  // remaining names in order of definition in ADES
  F_OBSID,
  F_TRKID,
  F_DELTARA,
  F_DELTADEC,
  F_RASTAR,
  F_DECSTAR,
  F_FRAME,
  F_NUCMAG,
  F_RMSFIT,
  F_NSTARS,
  F_REF,
  F_DISC,
  F_SUBFMT,
  F_PRECTIME,
  F_PRECRA,
  F_PRECDEC,
  F_UNCTIME,
  F_SYS,
  F_CTR,
  F_POS1,
  F_POS2,
  F_POS3,
  F_POSCOV11,
  F_POSCOV12,
  F_POSCOV13,
  F_POSCOV22,
  F_POSCOV23,
  F_POSCOV33,

  // radar specific
  F_VALRAD,
  F_RMSRAD,
  F_COM,
  F_FRQ,
  F_TRX,
  F_RCV,

  // residual specific
  F_RESRA,
  F_RESDEC,
  F_ORBID,
  F_SELAST,
  F_SIGRA,
  F_SIGDEC,
  F_SIGCORR,
  F_SIGTIME,
  F_BIASRA,
  F_BIASDEC,
  F_BIASTIME,
  F_RESMAG,
  F_SELPHOT,
  F_SIGMAG,
  F_BIASMAG,
  F_PHOTMOD,

  // radar residual specific
  F_RESRAD,
  F_SELRAD,
  F_SIGRAD,

  // final field of "Default PSV"
  F_REMARKS,

  // number of fields
  F_NUM
};

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
  dVal arraySize;
  dVal pixelScale;
} ctxTelescope;

typedef struct {
  dVal astrometry;
  dVal fitOrder;
  dVal photometry;
  dVal objectDetection;
} ctxSoftware;

typedef struct {
  ctxObservation *observation;
  ctxObservatory *observatory;
  ctxContact *contact;
  ctxNameList *observers;
  ctxNameList *measurers;
  ctxTelescope *telescope;
  ctxSoftware *software;
  dVal comment;
  ctxNameList *coinvestigators;
  ctxNameList *collaborators;
  dVal fundingSource;
  dVal orbProd;
  dVal photProd;
} observationContext;

typedef struct {
  observationContext *ctx;
  obsList *obs;
} observationSegment;

// root element: a list of observationSegments
typedef struct {
  int len;
  observationSegment *segments;
} observationBatch;

#endif

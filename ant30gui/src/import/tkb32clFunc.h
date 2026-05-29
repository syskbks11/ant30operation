/*!
\file tkb32clFunc.h
\date 2007.01.06
\author Y.Koide
\brief tkb32 Central Program Client Function
*/
#ifndef __TKB32CLFUNC_H__
#define __TKB32CLFUNC_H__
#include "tkb32Protocol.h"

#ifdef __cplusplus
extern "C"{
#endif

  int tkb32clFuncInit();
  int tkb32clFuncEnd();
  int tkb32clFuncReqInit();
  int tkb32clFuncReqStatus();
  
  //!trk関連
  int tkb32clFuncReqPosoff(double* posoff);
  int tkb32clFuncReqTrkStop();
  int tkb32clFuncReqUnStow();
  int tkb32clFuncReqStow();
  int tkb32clFuncReqProgramTracking();
  int tkb32clFuncReqACUStandby();
  int tkb32clFuncReqDriveUnLock();
  int tkb32clFuncReqDriveLock();
  int tkb32clFuncReqTrkStart();
  
  //!chop関連
  int tkb32clFuncReqRclose();
  int tkb32clFuncReqRopen();
  int tkb32clFuncReqRorigine();
  int tkb32clFuncReqRstop();

  //!if関連
  int tkb32clFuncReqSetLo1Freq(double freq);
  int tkb32clFuncReqSetLo1Amp(double amp);
  int tkb32clFuncReqSetLo1Att(double att);
  int tkb32clFuncReqSetLo2Freq(double freq);
  int tkb32clFuncReqSetLo2Amp(double amp);
  int tkb32clFuncReqSetSaIntegTime(double time);
  
  //!saacq関連
  int tkb32clFuncReqSaStart(int OnOffR);
  int tkb32clFuncReqSaStop();
  const tAnsState* tkb32clFuncGetAnsData();
  
  //!Obs関連
  int tkb32clFuncReqStartObs();
  int tkb32clFuncReqStopObs();
  int tkb32clFuncReqEnd();

#ifdef __cplusplus
}
#endif
#endif

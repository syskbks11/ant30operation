/*!
\file tkb32clObservationFunc.h
\date 2009.08.03
\author NAGAI Makoto
\brief tkb32 Central Program Client Function for the 32-m telescope & the 30-cm telescope. 
*/
#ifndef __TKB32CLOBSERVATIONFUNC_H__
#define __TKB32CLOBSERVATIONFUNC_H__
#include "tkb32Protocol.h"

#ifdef __cplusplus
extern "C"{
#endif

  
  //!ObsŠÖ˜A
/*
  int tkb32clObservationFuncReqInit();
  int tkb32clObservationFuncReqStart();
  int tkb32clObservationFuncReqStop();
  int tkb32clObservationFuncReqEnd();
*/
	int tkb32clObservationFuncSetTable(const char* dir, const char* grpName, const char* obsrName, const char* obsFile, char* msg);
	int tkb32clObservationFuncSetSource(const char* dir, const char* obsFlag, const char* raH, const char* raM, const char* raS, const char* decH, const char* decM, const char* decS, char* msg);


	int tkb32clObservationFuncCheckTable(const char* dir, const char* grpName, const char* obsrName, const char* obsFile, char* msg);
//	int tkb32clObservationFuncSetSchedule(const char* grpName, const char* obsrName, const char* obsFile, char* msg);

#ifdef __cplusplus
}
#endif
#endif

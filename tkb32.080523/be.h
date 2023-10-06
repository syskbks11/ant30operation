/*!
\file be.h
\author Y.Koide
\date 2006.12.20
\brief バックエンドとやり取りする
*/
#ifndef __BE_H__
#define __BE_H__

#ifdef __cplusplus
extern "C"{
#endif

  int beInit();
  int beEnd();
  int beReqInit();
  int beReqState();
  int beReqStart(int OnOffRoad, int returnFlag, unsigned long startTime,
		    double* RADEC, double* LB, double* AZEL,
		    double* DRADEC, double* DLB, double* DAZEL,
		    double* PAZEL, double* RAZEL,
		    double PA, double FQIF1, double VRAD, double IFATT,
		    double* weath, double* wind, double TAU,
		    double BATM, double IPINT);
  int beReqStop();
  int beGetAns(int beId, int* endExeFlag, int* acqErr, double* tsys, double* totPow);

#ifdef __cplusplus
}
#endif
#endif

#ifndef INCLUDE_GUARD_UUID_47641f0a_7d88_4368_9a33_c6405e69874d
#define INCLUDE_GUARD_UUID_47641f0a_7d88_4368_9a33_c6405e69874d
/*!
\file be.h
\author R.Enohi
\date 2024.11.26
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
		    double PA,double FQTRK, double FQTRK2, double FQTRK3, double FQTRK4, double FQIF1,
                    double VRAD, double IFATT,double IFATT2, double IFATT3, double IFATT4,
                    double LO2_1, double LO2_2, double LO2_3, double LO2_4,
		    double* weath, double* wind, double TAU,
		    double BATM, double IPINT);
  int beReqStop();
  int beGetAns(int beId, int* endExeFlag, int* acqErr, double* tsys, double* totPow);

#ifdef __cplusplus
}
#endif
#endif
#endif


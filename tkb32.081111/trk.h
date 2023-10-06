/*!
\file trk.h
\author Y.Koide
\date 2006.11.20
\brief Tracking Program
*
* Changed by NAGAI Makoto
* 2008.11.13
*/

#ifndef __TRK_H__
#define __TRK_H__

#ifdef __cplusplus
extern "C"{
#endif

  int  trkInit();
  int  trkInitParam();
  int  trkStart();
  int  trkStop();
  int  trkEnd();
  int  trkRepeat();

  //int  trkSetAcuMood(int mood);//081113 moved to acu.h
  //int  trkSetAcuBan(int ban);//081113 moved to acu.h
  //int  trkSetAcuUnStow();//081113 moved to acu.h
  void trkSetPof(double* pof, int coord);
  void trkSetSof(double* sof, int coord);
  int  trkSetRastStart(time_t startTime);
  void trkSetWeath(double* weath);
  void trkSetOff();
  int  trkSetSofNext(int* id);
  int  trkSetXY(int coord, double* XY);

  //int  trkGetACU();//081113 moved to acu.h
  void trkGetXY(int* coord, double* xy);
  void trkGetRADEC(double* RADEC);
  void trkGetLB(double* LB);
  void trkGetAZEL(double* AZEL);
  void trkGetDRADEC(double* DRADEC);
  void trkGetDLB(double* DLB);
  void trkGetDAZEL(double* DAZEL);
  void trkGetPZONE(int* PZONE);
  void trkGetRZONE(int* RZONE);
  void trkGetPAZEL(double* PAZEL);
  void trkGetRAZEL(double* RAZEL);
  void trkGetEAZEL(double* EAZEL);
  void trkGetACUStatus(int* status);
  void trkGetSof(int* coord, double* sof);
  void trkGetPof(int* coord, double* pof);
  void trkGetTrackStatus(int* status);
  void trkGetLST(unsigned int* lst);

#ifdef __cplusplus
}
#endif
#endif

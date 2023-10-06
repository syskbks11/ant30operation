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
  void trkSetPof(const double* pof, int coord);
  int  trkSetRastStart(time_t startTime);
  //void trkSetWeath(const double* weath);//081114 moved to calc.h
  //void trkSetOff();
  int  trkSetSofNext(int* id);
  int  trkSetXY(int coord, const double* XY);

  //int  trkGetACU();//081113 moved to acu.h
  void trkGetXY(int* coord, double* xy);
  //void trkGetRADEC(double* RADEC);//081115 moved to calc.h
  //void trkGetLB(double* LB);//081115 moved to calc.h
  //void trkGetAZEL(double* AZEL);//081114 moved to calc.h
  //void trkGetDRADEC(double* DRADEC);//081115 moved to calc.h
  //void trkGetDLB(double* DLB);//081115 moved to calc.h
  //void trkGetDAZEL(double* DAZEL);//081115 moved to calc.h
  void trkGetPZONE(int* PZONE);
  //void trkGetRZONE(int* RZONE);//081113 moved to acu.h
  //void trkGetPAZEL(double* PAZEL);//081114 moved to calc.h
  //void trkGetRAZEL(double* RAZEL);//081113 moved to acu.h
  void trkGetEAZEL(double* EAZEL);
  //void trkGetACUStatus(int* status);//081113 moved to acu.h
  //void trkGetSof(int* coord, double* sof);//081115 moved to calc.h
  //void trkGetPof(int* coord, double* pof);//081115 moved to calc.h
  int trkGetTrackStatus();
  //void trkGetTrackStatus(int* status);
  //unsigned int trkGetLST();//081115 moved to calc.h
  //void trkGetLST(unsigned int* lst);

#ifdef __cplusplus
}
#endif
#endif

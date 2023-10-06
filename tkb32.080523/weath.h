/*!
\file weath.h
\author Y.Koide
\date 2006.12.12
\brief 気象データを取得する
*/
#ifndef __WEATH_H__
#define __WEATH_H__

#ifdef __cplusplus
extern "C"{
#endif

  int weathInit();
  int weathEnd();
  int weathStart();
  int weathRepeat();
  int weathGetDat();
  int weathGetWeath(double* weath);
  int weathGetWind(double* wind);

#ifdef __cplusplus
}
#endif
#endif

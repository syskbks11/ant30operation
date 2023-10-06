/*!
\file tm.h
\author Y.Koide
\date 2006.12.17
\brief タイマーモジュール
*/
#ifndef __TM_H__
#define __TM_H__

#ifdef __cplusplus
extern "C"{
#endif
  const char* tmVersion();


typedef struct sTm tmClass_t;//081117 in
tmClass_t* tmInit();
//  void* tmInit();
int tmEnd(tmClass_t* _p);
//  int tmEnd(void* _p);
int tmReset(tmClass_t* _p);
//  int tmReset(void* _p);
double tmGetLag(const tmClass_t* _p);
//  double tmGetLag(void* _p);
const char* tmGetTimeStr(tmClass_t* _p, double offset);
//  const char* tmGetTimeStr(void* _p, double offset);

  double tmGetDiff(const time_t t, const double msec);
  int tmSleepMSec(double msec);
  void tmGetTime(double offset, time_t* t, double* msec);

#ifdef __cplusplus
}
#endif
#endif

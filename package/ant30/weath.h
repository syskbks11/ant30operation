/*!
\file chop.h
\author Y.Koide
\date 2006.11.20
\brief Chopper Wheel Function
*/

#ifndef __WEATH_H__
#define __WEATH_H__

#ifdef __cplusplus
extern "C"{
#endif

  int weathInit();
  int weathEnd();
  int weathRepeat();
  int weathGetState();
  int weathGetWeath(double* weath);
  int weathGetWind(double* wind);
  int weathSetReq(int reqSet);

#ifdef __cplusplus
}
#endif
#endif //__WEATH_H__



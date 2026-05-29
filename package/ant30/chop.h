#ifndef INCLUDE_GUARD_UUID_e9217cb6_2a1e_416b_b3e8_f2efcf896db4
#define INCLUDE_GUARD_UUID_e9217cb6_2a1e_416b_b3e8_f2efcf896db4
/*!
\file chop.h
\author Y.Koide
\date 2006.11.20
\brief Chopper Wheel Function
*/

#ifndef __CHOP_H__
#define __CHOP_H__

#ifdef __cplusplus
extern "C"{
#endif

  int chopInit();
  int chopEnd();
  int chopRepeat();
  int chopGetState();
  int chopGetMoter();
  double chopGetTemp1();
  double chopGetTemp2();
  int chopGetInterlock();
  int chopGetError();
  int chopSetReq(int reqSet);

#ifdef __cplusplus
}
#endif
#endif //__CHOP_H__
#endif


/*!
\file srvPow.h
\date 2008.01.24
\author Y.Koide
\brief 連続波観測用のプログラム。パワーメータを制御する。
* 2009.8.7
* Modified by NAGAI, for 30-cm telescope
*/
#ifndef __SRVPOWER_H__
#define __SRVPOWER_H__

#ifdef __cplusplus
extern "C"{
#endif

typedef struct srvPow_s srvPower_Class;

  srvPower_Class* srvPowerInit();
  int srvPowerEnd(srvPower_Class* _p);
  int srvPowerRepeat(srvPower_Class* _p);

#ifdef __cplusplus
}
#endif
#endif

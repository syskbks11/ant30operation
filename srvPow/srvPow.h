/*!
\file srvPow.h
\date 2008.01.24
\author Y.Koide
\brief 連続波観測用のプログラム。パワーメータを制御する。
*/
#ifndef __SRVPOW_H__
#define __SRVPOW_H__

#ifdef __cplusplus
extern "C"{
#endif

  void* srvPowInit();
  int srvPowEnd(void* _p);
  int srvPowRepeat(void* _p);

#ifdef __cplusplus
}
#endif
#endif

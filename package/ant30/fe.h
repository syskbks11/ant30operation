#ifndef INCLUDE_GUARD_UUID_19f0c857_2de0_4119_98df_9d10c0854709
#define INCLUDE_GUARD_UUID_19f0c857_2de0_4119_98df_9d10c0854709
/*!
\file fe.h
\date 2006.12.31
\author Y.Koide
\brief フロントエンド制御プログラム
*/
#ifndef __FE_H__
#define __FE_H__

#ifdef __cplusplus
extern "C"{
#endif
  
  int feInit();
  int feEnd();
  int feRepeat();
  double feGetTemperatureK();
  double feGetPressurePa();

#ifdef __cplusplus
}
#endif
#endif
#endif


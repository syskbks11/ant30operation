/*!
\file acu.h
\author NAGAI Makoto
\date 2008.11.13
\brief ACU Program for 32-m telescope & 30-cm telescope
*/

#ifndef __ACU_H__
#define __ACU_H__

#ifdef __cplusplus
extern "C"{
#endif
  int  acuSetAcuMood(int mood);
  int  acuSetAcuBan(int ban);
  int  acuSetAcuUnStow();

  int  acuGetACU();

#ifdef __cplusplus
}
#endif
#endif

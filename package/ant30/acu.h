#ifndef INCLUDE_GUARD_UUID_3071466e_4d57_4fc5_a8cb_2067c3dc57cb
#define INCLUDE_GUARD_UUID_3071466e_4d57_4fc5_a8cb_2067c3dc57cb
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


int acuInit();
//int acuInit(tParamTrk *p);
int acuEnd();

int  acuSetAcuMood(int mood);
int  acuSetAcuBan(int ban);
int  acuSetAcuUnStow();

int  acuGetACU();
int acuSetACU(int antZone);
int acuSafetyCheck();

void acuSetDoAzC(double az);
void acuSetDoElC(double el);
void acuGetRAZEL(double* RAZEL);
int acuGetRZONE();
//void acuGetRZONE(int* RZONE);
void acuGetACUStatus(int* status);
void acuGetACULoadStatus(int* status);

#ifdef __cplusplus
}
#endif
#endif
#endif


/*!
\file calc.h
\author NAGAI Makoto
\date 2008.11.14
\brief Tracking calculation Program for 32-m telescope & 30-cm telescope
*/

#ifndef __CALC_H__
#define __CALC_H__

#ifdef __cplusplus
extern "C"{
#endif

#include "trkCalc.h"

//functions derived from calc.h
void calcSetWeath(const double* weath);

void calcGetRADEC(double* RADEC);
void calcGetLB(double* LB);

void calcGetAZEL(double* AZEL);
void calcGetPAZEL(double* PAZEL);

void calcGetDRADEC(double* DRADEC);
void calcGetDLB(double* DLB);
void calcGetDAZEL(double* DAZEL);
void calcGetScanOff(int* coord, double* sof);
void calcGetPointOff(int* coord, double* pof);

unsigned int calcGetLST();


//functions derived from calc2.h
double calc2GetVRAD();

#ifdef __cplusplus
}
#endif
#endif

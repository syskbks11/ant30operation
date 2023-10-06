/*!
¥file acu.cpp
¥author NAGAI Makoto
¥date 2008.11.14
¥brief Tracking Calculation Program for 32-m telescope & 30-cm telescope
*/

#ifndef __TRKCALC_H__
#define __TRKCALC_H__

#ifdef __cplusplus
extern "C"{
#endif

#include "trk45Sub.h"
#include "celestialObject.h"
#include "scanTable.h"


int calcInit();//081117 in
//int calcInit(tParamTrk *param);
int calcInitParam();
void calcEnd();

int calcTrkCal(int currOffFlag, int currOffNo);
//int trkCal();

void calcSetSof(const double* sof, int coord, double posAngleRad);

double calcGetDoAz2();
double calcGetDoEl2();
double calcGetMapCenterDec();
double calcGetAntennaLat();

void calcSetXinDscn(int coord, double dscnst0, double dscnst1, double dscned0, double dscned1);
void calcSetXinDpof(int coord, double dpofst0, double dpofst1, double dpofaz0, double dpofaz1);

#ifdef __cplusplus
}
#endif
#endif

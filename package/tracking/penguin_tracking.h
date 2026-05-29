/*!
¥file penguin_tracking.h
¥author NAGAI Makoto
¥date 2009.02.20
¥brief Tracking Calculation Program for 32-m telescope & 30-cm telescope
*/

#ifndef __PENGUIN_TRACKING_H__
#define __PENGUIN_TRACKING_H__

#ifdef __cplusplus
extern "C"{
#endif

#include "glacier_celestialObject.h"
#include "glacier_trackingInformation.h"
#include "glacier_scanOffset.h"

int penguin_tracking_Init();
void penguin_tracking_end();

int penguin_tracking_setTime(int year, int month, int day, int hour, int minute, int second, double mircoSecond);
//int penguin_tracking_setTime(int year, int month, int day, int hour, int minute, double second);
int penguin_tracking_setCurrentTime(double mircoSecondOffset);
int penguin_tracking_loadConfigWithConfInit();
int penguin_tracking_loadConfig();

int penguin_tracking_setScanOffset(glacier_ScanOffsetClass_t* offset);
int penguin_tracking_setPointingOffset(glacier_ScanOffsetClass_t* offset);


//For position
int penguin_tracking_setObject(glacier_CelestialObjectClass_t* object);
glacier_TrackingInformationClass_t* penguin_tracking_work();

//For velocity
int penguin_tracking_setObjectForVelocity(glacier_CelestialObjectClass_t* object);
void penguin_tracking_setRestFrequencyForVelocity(double freq);
double penguin_tracking_workForVelocity();
double penguin_tracking_getDopplerFrequency();

#ifdef __cplusplus
}
#endif

#endif

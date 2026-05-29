/*!
¥file glacier_trackingInformation.h
¥author NAGAI Makoto
¥date 2009.02.24
¥brief trk45用の天体を表すオブジェクト
*/
#ifndef __GLACIERTRACKINGINFORMATION_H__
#define __GLACIERTRACKINGINFORMATION_H__

#ifdef __cplusplus
extern "C"{
#endif


typedef struct sTrackingInformation glacier_TrackingInformationClass_t;
glacier_TrackingInformationClass_t* glacier_TrackingInformationInit(const double* result, double lst, const double* azel);
int glacier_TrackingInformationEnd(glacier_TrackingInformationClass_t* _p);

const double* glacier_TrackingInformationGet0(glacier_TrackingInformationClass_t* _p);
const double* glacier_TrackingInformationGet1(glacier_TrackingInformationClass_t* _p);
const double* glacier_TrackingInformationGet2(glacier_TrackingInformationClass_t* _p);
const double* glacier_TrackingInformationGet3(glacier_TrackingInformationClass_t* _p);
const double* glacier_TrackingInformationGet4(glacier_TrackingInformationClass_t* _p);
const double* glacier_TrackingInformationGet5(glacier_TrackingInformationClass_t* _p);
const double* glacier_TrackingInformationGet6(glacier_TrackingInformationClass_t* _p);
const double* glacier_TrackingInformationGet7(glacier_TrackingInformationClass_t* _p);
const double* glacier_TrackingInformationGetAzEl(glacier_TrackingInformationClass_t* p);
double glacier_TrackingInformationGetLocalSideralTime(glacier_TrackingInformationClass_t* _p);

#ifdef __cplusplus
}
#endif
#endif

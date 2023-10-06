/*!
¥file glacier_trackingInformation.cpp
¥author NAGAI Makoto
¥date 2009.02.25
¥brief trk45用の計算結果を表すオブジェクト
*/
#include <memory.h>//memset()
#include <stdlib.h>//malloc(), free()


#include "glacier_trackingInformation.h"

typedef struct sTrackingInformation{
	double lst;
	double result0[2];
	double result1[2];
	double result2[2];
	double result3[2];
	double result4[2];
	double result5[2];
	double result6[2];
	double result7[2];
	double azel[2];
}tTrackingInformation;

glacier_TrackingInformationClass_t* glacier_TrackingInformationInit(const double* result, double lst, const double* azel){
	tTrackingInformation* p;

	p=(tTrackingInformation*)malloc(sizeof(tTrackingInformation));
	if(!p){
		return NULL;
	}

	memset(p, 0, sizeof(*p));

	p->lst = lst;
	memcpy(&p->result0, result, sizeof(double)*2);
	memcpy(&p->result1, result+2, sizeof(double)*2);
	memcpy(&p->result2, result+4, sizeof(double)*2);
	memcpy(&p->result3, result+6, sizeof(double)*2);
	memcpy(&p->result4, result+8, sizeof(double)*2);
	memcpy(&p->result5, result+10, sizeof(double)*2);
	memcpy(&p->result6, result+12, sizeof(double)*2);
	memcpy(&p->azel, azel, sizeof(double)*2);

	return p;

}

int glacier_TrackingInformationEnd(tTrackingInformation* p){
	if(p){
		free(p);
		p=NULL;
	}
	return 0;
}
/*
* The result of trk45 seems to be (longitude, latitude) of the telescope.
*/
const double* glacier_TrackingInformationGet0(glacier_TrackingInformationClass_t* p){
	return p->result0;
}

/*
* strange.
*/
const double* glacier_TrackingInformationGet1(glacier_TrackingInformationClass_t* p){
	return p->result1;
}

/*
* strange.
* If the scan offset is (0, 0),
* the result of trk45 seems to be the same with glacier_TrackingInformationGet5().
*/
const double* glacier_TrackingInformationGet2(glacier_TrackingInformationClass_t* p){
	return p->result2;
}

/*
* strange.
* If the scan offset is (0, 0),
* the result of trk45 seems to be the same with glacier_TrackingInformationGetAzEl().
*/
const double* glacier_TrackingInformationGet3(glacier_TrackingInformationClass_t* p){
	return p->result3;
}

/*
* The result of trk45 seems to be (R.A., Dec.) of the same point with glacier_TrackingInformationGetAzEl().
*/
const double* glacier_TrackingInformationGet4(glacier_TrackingInformationClass_t* p){
	return p->result4;
}

/*
* The result of trk45 seems to be (l, b) of the same point with glacier_TrackingInformationGetAzEl().
*/
const double* glacier_TrackingInformationGet5(glacier_TrackingInformationClass_t* p){
	return p->result5;
}

/*
* The result of trk45 seems to be the same with glacier_TrackingInformationGetAzEl().
*/
const double* glacier_TrackingInformationGet6(glacier_TrackingInformationClass_t* p){
	return p->result6;
}

/*
* The result of trk45 seems to be always 0.
*/
const double* glacier_TrackingInformationGet7(glacier_TrackingInformationClass_t* p){
	return p->result7;
}

const double* glacier_TrackingInformationGetAzEl(glacier_TrackingInformationClass_t* p){
	return p->azel;
}

double glacier_TrackingInformationGetLocalSideralTime(glacier_TrackingInformationClass_t* p){
	return p->lst;
}



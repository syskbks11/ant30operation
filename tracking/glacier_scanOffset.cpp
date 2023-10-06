/*!
¥file glacier_celestialObject.cpp
¥author NAGAI Makoto
¥date 2009.02.24
¥brief trk45用の天体を表すオブジェクト
*/

#include <memory.h>//memset()
#include <stdlib.h>//malloc(), free()
#include <stdio.h>//printf()

#include "glacier_scanOffset.h"


typedef struct sScanOffset{
	int coordinate;           //!< 位置座標系 0:惑星 1:RADEC 2:LB 3:AZEL
	double xy[2];     //!< 天体位置 [rad]
}tScanOffset;


/*! ¥fn glacier_ScanOffsetClass_t* glacier_ScanOffsetInit()
¥brief glacier_ScanOffsetクラスのコンストラクタ
¥return NULL:Error Other:データ構造体へのポインタ
*/

tScanOffset* glacier_ScanOffsetInit(){
	tScanOffset* p;

	p=(tScanOffset*)malloc(sizeof(tScanOffset));
	if(!p){
		return NULL;
	}

	memset(p, 0, sizeof(*p));

	return p;
}

/*! int glacier_ScanOffsetEnd(glacier_ScanOffsetClass_t* p)
¥brief tmの使用を終了するときにメモリなどを開放する。
¥return 0:Success
*/
int glacier_ScanOffsetEnd(tScanOffset* p){
	if(p){
		free(p);
		p=NULL;
	}
	return 0;
}


void glacier_ScanOffsetSetInRADec(glacier_ScanOffsetClass_t* p, double rightAscention, double declination){
	p->coordinate = CELESTIAL_SCAN_COORDINATE_RADEC;
	p->xy[0] = rightAscention;
	p->xy[1] = declination;
}

void glacier_ScanOffsetSetInLB(glacier_ScanOffsetClass_t* p, double gLongitude, double gLatitude){
	p->coordinate = CELESTIAL_SCAN_COORDINATE_LB;
	p->xy[0] = gLongitude;
	p->xy[1] = gLatitude;
}
void glacier_ScanOffsetSetInAzEl(glacier_ScanOffsetClass_t* p, double azimuth, double elevation){
	p->coordinate = CELESTIAL_SCAN_COORDINATE_AZEL;
	p->xy[0] = azimuth;
	p->xy[1] = elevation;
}
int glacier_ScanOffsetSetXY(glacier_ScanOffsetClass_t* p, int coordinate, double x, double y){
	switch(coordinate){
	case CELESTIAL_SCAN_COORDINATE_RADEC:
	case CELESTIAL_SCAN_COORDINATE_LB:
	case CELESTIAL_SCAN_COORDINATE_AZEL:
		p->coordinate = coordinate;
		p->xy[0] = x;
		p->xy[1] = y;	
		break;
	default:
		printf("invalid coordinate for glacier_ScanOffsetSetXY(); %d\n", coordinate);
		return 1;
	}
	return 0;//normal end
}


int glacier_ScanOffsetGetCoordinate(glacier_ScanOffsetClass_t* p){
	return p->coordinate;
}
double glacier_ScanOffsetGetX(glacier_ScanOffsetClass_t* p){
	return p->xy[0];
}
double glacier_ScanOffsetGetY(glacier_ScanOffsetClass_t* p){
	return p->xy[1];
}


/*!
¥file glacier_scanOffset.h
¥author NAGAI Makoto
¥date 2009.05.14
¥brief trk45用のスキャン・オフセットを表すオブジェクト
*/
#ifndef __GLACIERSCANOFFSET_H__
#define __GLACIERSCANOFFSET_H__

#ifdef __cplusplus
extern "C"{
#endif

#define CELESTIAL_SCAN_COORDINATE_RADEC 1
#define CELESTIAL_SCAN_COORDINATE_LB 2
#define CELESTIAL_SCAN_COORDINATE_AZEL 3

typedef struct sScanOffset glacier_ScanOffsetClass_t;
glacier_ScanOffsetClass_t* glacier_ScanOffsetInit();
int glacier_ScanOffsetEnd(glacier_ScanOffsetClass_t* _p);

void glacier_ScanOffsetSetInRADec(glacier_ScanOffsetClass_t* p, double rightAscention, double declination);
void glacier_ScanOffsetSetInLB(glacier_ScanOffsetClass_t* p, double gLongitude, double gLatitude);
void glacier_ScanOffsetSetInAzEl(glacier_ScanOffsetClass_t* p, double azimuth, double elevation);
/*
* glacier_ScanOffsetSetXY(p, CELESTIAL_SCAN_COORDINATE_RADEC, x, y) is equivalent to glacier_ScanOffsetSetInRADec(p, x, y).
* glacier_ScanOffsetSetXY(p, CELESTIAL_SCAN_COORDINATE_LB, x, y) is equivalent to glacier_ScanOffsetSetInLB(p, x, y).
* glacier_ScanOffsetSetXY(p, CELESTIAL_SCAN_COORDINATE_AZEL, x, y) is equivalent to glacier_ScanOffsetSetInAzEl(p, x, y).
*/
int glacier_ScanOffsetSetXY(glacier_ScanOffsetClass_t* p, int coordinate, double x, double y);

int glacier_ScanOffsetGetCoordinate(glacier_ScanOffsetClass_t* p);
double glacier_ScanOffsetGetX(glacier_ScanOffsetClass_t* p);
double glacier_ScanOffsetGetY(glacier_ScanOffsetClass_t* p);

#ifdef __cplusplus
}
#endif
#endif

/*!
¥file glacier_celestialObject.cpp
¥author NAGAI Makoto
¥date 2009.02.24
¥brief trk45用の天体を表すオブジェクト
*/

#include <memory.h>//memset()
#include <stdlib.h>//malloc(), free()

#include "glacier_celestialObject.h"

typedef struct sCelestialObject{
  int source;            //!< 天体フラグ 0:太陽系外 1-10:惑星 11:太陽 12:COMET 99:AZEL
  int coordinate;           //!< 位置座標系 0:惑星 1:RADEC 2:LB 3:AZEL
  int epoch;           //!< 分点フラグ 1:B1950 2:J2000
  double velocity;         //!< 天体の視線速度 [km/s]
  double xy[2];     //!< 天体位置 [rad]
	int definition;
	int frame;
}tCelestialObject;


/*! ¥fn glacier_CelestialObjectClass_t* glacier_CelestialObjectInit()
¥brief glacier_CelestialObjectクラスのコンストラクタ
¥return NULL:Error Other:データ構造体へのポインタ
*/
tCelestialObject* glacier_CelestialObjectInit(){
	tCelestialObject* p;

	p=(tCelestialObject*)malloc(sizeof(tCelestialObject));
	if(!p){
		return NULL;
	}

	memset(p, 0, sizeof(*p));

	return p;
}

/*! int glacier_CelestialObjectEnd(glacier_CelestialObjectClass_t* p)
¥brief tmの使用を終了するときにメモリなどを開放する。
¥return 0:Success
*/
int glacier_CelestialObjectEnd(tCelestialObject* p){
	if(p){
		free(p);
		p=NULL;
	}
	return 0;
}

int glacier_CelestialObjectSetSource(glacier_CelestialObjectClass_t* p, int source){
	p->source = source;
	return 0;
}

int glacier_CelestialObjectSetCoordinate(glacier_CelestialObjectClass_t* p, int coordinate, double x, double y){
	p->coordinate = coordinate;
	p->xy[0] = x;
	p->xy[1] = y;
	return 0;
}

int glacier_CelestialObjectSetEpoch(glacier_CelestialObjectClass_t* p, int epoch){
	p->epoch = epoch;
	return 0;
}

int glacier_CelestialObjectSetVelocity(glacier_CelestialObjectClass_t* p, double velocity){
	p->velocity = velocity;
	return 0;
}


int glacier_CelestialObjectGetSource(glacier_CelestialObjectClass_t* p){
	return p->source;
}

int glacier_CelestialObjectGetCoordinate(glacier_CelestialObjectClass_t* p){
	return p->coordinate;
}

int glacier_CelestialObjectGetEpoch(glacier_CelestialObjectClass_t* p){
	return p->epoch;
}

double glacier_CelestialObjectGetVelocity(glacier_CelestialObjectClass_t* p){
	return p->velocity;
}

double* glacier_CelestialObjectGetXY(glacier_CelestialObjectClass_t* p){
	return p->xy;
}
int glacier_CelestialObjectSetFrame(glacier_CelestialObjectClass_t* p, int frame){
	p->frame = frame;
	return 0;
}
int glacier_CelestialObjectSetDefinition(glacier_CelestialObjectClass_t* p, int definition){
	p->definition = definition;
	return 0;
}
int glacier_CelestialObjectGetFrame(glacier_CelestialObjectClass_t* p){
	return p->frame;
}
int glacier_CelestialObjectGetDefinition(glacier_CelestialObjectClass_t* p){
	return p->definition;
}



/*!
¥file glacier_celestialObject.h
¥author NAGAI Makoto
¥date 2009.02.24
¥brief trk45用の天体を表すオブジェクト
*/
#ifndef __GLACIERCELESTIALOBJECT_H__
#define __GLACIERCELESTIALOBJECT_H__

#ifdef __cplusplus
extern "C"{
#endif


#define CELESTIAL_OBJECT_SOURCE_OUT 0
#define CELESTIAL_OBJECT_SOURCE_MERCURY 1
#define CELESTIAL_OBJECT_SOURCE_VENUS 2
#define CELESTIAL_OBJECT_SOURCE_MARS 4
#define CELESTIAL_OBJECT_SOURCE_JUPITER 5
#define CELESTIAL_OBJECT_SOURCE_SATURN 6
#define CELESTIAL_OBJECT_SOURCE_URANUS 7
#define CELESTIAL_OBJECT_SOURCE_NEPTUNE 8
#define CELESTIAL_OBJECT_SOURCE_PLUTO 9
#define CELESTIAL_OBJECT_SOURCE_MOON 10
#define CELESTIAL_OBJECT_SOURCE_SUN 11
#define CELESTIAL_OBJECT_SOURCE_AZEL 99

#define CELESTIAL_OBJECT_COORDINATE_PLANET 0
#define CELESTIAL_OBJECT_COORDINATE_RADEC 1
#define CELESTIAL_OBJECT_COORDINATE_LB 2
#define CELESTIAL_OBJECT_COORDINATE_AZEL 3

#define CELESTIAL_OBJECT_EPOCH_B1950 1
#define CELESTIAL_OBJECT_EPOCH_J2000 2

#define CELESTIAL_OBJECT_FRAME_LSR 1
#define CELESTIAL_OBJECT_FRAME_HELIO 2

#define CELESTIAL_OBJECT_DEEFINITION_RADIO 1
#define CELESTIAL_OBJECT_DEEFINITION_OPTICAL 2


typedef struct sCelestialObject glacier_CelestialObjectClass_t;
glacier_CelestialObjectClass_t* glacier_CelestialObjectInit();
int glacier_CelestialObjectEnd(glacier_CelestialObjectClass_t* _p);

int glacier_CelestialObjectSetSource(glacier_CelestialObjectClass_t* _p, int source);
int glacier_CelestialObjectSetCoordinate(glacier_CelestialObjectClass_t* _p, int coordinate, double x, double y);
int glacier_CelestialObjectSetEpoch(glacier_CelestialObjectClass_t* _p, int epoch);
int glacier_CelestialObjectSetVelocity(glacier_CelestialObjectClass_t* _p, double velocity);
int glacier_CelestialObjectSetFrame(glacier_CelestialObjectClass_t* _p, int frame);
int glacier_CelestialObjectSetDefinition(glacier_CelestialObjectClass_t* _p, int definition);

int glacier_CelestialObjectGetSource(glacier_CelestialObjectClass_t* _p);
int glacier_CelestialObjectGetCoordinate(glacier_CelestialObjectClass_t* _p);
int glacier_CelestialObjectGetEpoch(glacier_CelestialObjectClass_t* _p);
double glacier_CelestialObjectGetVelocity(glacier_CelestialObjectClass_t* _p);
double* glacier_CelestialObjectGetXY(glacier_CelestialObjectClass_t* _p);
int glacier_CelestialObjectGetFrame(glacier_CelestialObjectClass_t* _p);
int glacier_CelestialObjectGetDefinition(glacier_CelestialObjectClass_t* _p);

#ifdef __cplusplus
}
#endif
#endif

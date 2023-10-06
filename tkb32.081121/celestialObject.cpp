/*!
¥file celestialObject.cpp
¥author NAGAI Makoto
¥date 2008.11.15
¥brief Manages the celestial object (or the source of the observation table) for 32-m telescope & 30-cm telescope
*/

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/types.h>

#include "configuration.h"
#include "celestialObject.h"
#include "errno.h"

#define PI M_PI


typedef struct {
  int sourceFlag;            //!< 天体フラグ 0:太陽系外 1-10:惑星 11:太陽 12:COMET 99:AZEL
  int coordinate;           //!< 位置座標系 0:惑星 1:RADEC 2:LB 3:AZEL
  int epoch;           //!< 分点フラグ 1:B1950 2:J2000
  double velocity;         //!< 天体の視線速度 [m/s]
  double xy[2];     //!< 天体位置 [rad]
}celestialObject_t;


//! celestialObjectのパラメーター構造体
static celestialObject_t p;

int celestialObjectInit(){
	// 0 fill

	p.sourceFlag = 0;
	p.coordinate = 0;
	p.epoch = 0;
	p.velocity = 0;
	memset(p.xy, 0, sizeof(p.xy[0]) * 2);

	//Load from configuration files. 
	//! Source
	if(confSetKey("SourceFlag"))
		p.sourceFlag = atoi(confGetVal());  //!< 天体フラグ 0:太陽系外 1-9:惑星 10:月 11:太陽 12:COMET 99:AZEL
	if(confSetKey("Coordinate"))
		p.coordinate = atoi(confGetVal()); //!< 位置座標系 0:惑星 1:RADEC 2:LB 3:AZEL
	if(confSetKey("X_Rad"))
		p.xy[0] = atof(confGetVal());
	if(confSetKey("Y_Rad"))
		p.xy[1] = atof(confGetVal());
	if(confSetKey("Epoch"))
		p.epoch = atoi(confGetVal()); //!< 分点フラグ 1:B1950 2:J2000
	if(confSetKey("Velocity"))
		p.velocity = atof(confGetVal());

	//check the paramters
    //! 観測関連
    if(p.sourceFlag < 0 || (p.sourceFlag > 12 && p.sourceFlag != 99)){
      uM1("SourceFlag(%d) error!!", p.sourceFlag);
      return -1;
    }
    if(p.coordinate < 0 || p.coordinate > 3){
      uM1("Coorinate(%d) error!!", p.coordinate);
      return -1;
    }
    if(p.epoch < 1 || p.epoch > 2){
      uM1("Epoch(%d) error!!", p.epoch);
      return -1;
    }
    if(p.xy[0] < 0.0 || p.xy[0] > 2.0*PI){
      uM1("X_Rad(%lf) error!!", p.xy[0]);
      return -1;
    }
    if(p.xy[1] < -PI/2.0 || p.xy[1] > PI/2.0){
      uM1("Y_Rad(%lf) error!!", p.xy[1]);
      return -1;
    }

	if(p.sourceFlag != 0 && p.coordinate != 0){
		uM2("SourceFlag(%d) and Coorinate(%d) inconsistent! ", p.sourceFlag, p.coordinate);
	}

	return 0;//normal end

}

void celestialObjectInitLeeTracking(){
	//! Lee Tracking
	p.sourceFlag = 99;                  //!< 99:AZEL
	p.coordinate = 3;                  //!< 3:AZEL
	p.epoch = 2;                  //!< J2000.0
	p.velocity = 0;
	p.xy[0] = 0 * PI / 180.0;  //!< 適当な値 AZ
	p.xy[1] = 10 * PI / 180.0; //!< 適当な値 EL
}

int celestialObjectSetXY(int coord, const double* xy){
	if(coord < 1 || coord > 3){
		uM1("celestialObjectXY(); invalid coord: %d\n", coord);
		return TRK_SET_ERR;
	}
	else if(xy[0] < 0.0 || xy[0] > 2.0 * PI || xy[1] < - PI / 2.0 || xy[1] > PI / 2.0){
		uM2("celestialObjectXY(); invalid xy: (%f, %f)", xy[0], xy[1]);
		return TRK_SET_ERR;
	}
	p.coordinate = coord;
	memcpy(p.xy, xy, sizeof(*p.xy) * 2);
	return 0;//normal end
}

int celestialObjectGetSourceFlag(){
	return p.sourceFlag;
}
int celestialObjectGetCoordinate(){
	return p.coordinate;
}
int celestialObjectGetEpoch(){
	return p.epoch;
}
double celestialObjectGetVelocity(){
	return p.velocity;
}
const double* celestialObjectGetXY(){
	return p.xy;
}


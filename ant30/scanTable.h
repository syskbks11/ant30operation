/*!
\file scanTable.h
\author NAGAI Makoto
\date 2008.11.15
\brief Manages the scan table for 32-m telescope & 30-cm telescope
*/

#ifndef __SCANTABLE_H__
#define __SCANTABLE_H__

#ifdef __cplusplus
extern "C"{
#endif

//!< スキャンの定義 1:OnOff 2:5Points 3:Grid 4:Random 5:9Point 6:Raster 1001:SourceTracking 1002:Lee Tracking
// 090611 in
#define SCANTABLE_FLAG_ONOFF 1
#define SCANTABLE_FLAG_5 2
#define SCANTABLE_FLAG_GRID 3
#define SCANTABLE_FLAG_RANDOM 4
#define SCANTABLE_FLAG_9 5
#define SCANTABLE_FLAG_RASTER 6
#define SCANTABLE_FLAG_CROSS 7
#define SCANTABLE_FLAG_SOURCE 1001
#define SCANTABLE_FLAG_LEE 1002


/* there is only one instance of scan table for the program.
 * All of the following functions access to the instance. 
 */

int scanTableInit();
void scanTableEnd();

/* These functions returns a constant value; the results change only when scanTableInit() or scanTableEnd() are invoked */
int scanTableGetOnCoord();
int scanTableGetOffCoord();
int scanTableGetOffNumber();
int scanTableGetScanFlag();
//int scanTableGetOffInterval();
int scanTableGetOffMode();
//char* scanTableGetSetPattern();
int scanTableGetOnNumber();
double scanTableGetPosAngleRad();
double scanTableGetLineTime();

const double* scanTableGetOn(int index);
const double* scanTableGetOff(int index);
double scanTableGetOnValue(int index);
double scanTableGetOffValue(int index);
double scanTableGetLinePath(int index);
int scanTableGetNumberOfPoints();
int scanTableGetPointNumber(int index);

#ifdef __cplusplus
}
#endif
#endif

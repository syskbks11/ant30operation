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

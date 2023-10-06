/*!
\file celestialObject.h
\author NAGAI Makoto
\date 2008.11.15
\brief Manages the celestial object (or the source of the observation table) for 32-m telescope & 30-cm telescope
*/

#ifndef __CELESTIALOBJECT_H__
#define __CELESTIALOBJECT_H__

#ifdef __cplusplus
extern "C"{
#endif


/* there is only one instance of celestial object for the program.
 * All of the following functions access to the instance. 
 */

int celestialObjectInit();
void celestialObjectInitLeeTracking();

int celestialObjectSetXY(int coord, const double* xy);

int celestialObjectGetSourceFlag();
int celestialObjectGetCoordinate();
int celestialObjectGetEpoch();
double celestialObjectGetVelocity();
const double* celestialObjectGetXY();

#ifdef __cplusplus
}
#endif
#endif

/*!
\file instrument.h
\author NAGAI Makoto
\date 2009.2.6
\brief Manage Instrument Parameter for 32-m telescope & 30-cm telescope
*/

#ifndef __INSTRUMENT_H__
#define __INSTRUMENT_H__

#ifdef __cplusplus
extern "C"{
#endif

int instrumentInitParam();
void instrumentCalInst(double X, double Y, double* dX, double* dY);

#ifdef __cplusplus
}
#endif
#endif

#ifndef INCLUDE_GUARD_UUID_8dcfeaaf_f4d9_417d_bf79_c39f6e9e6c8d
#define INCLUDE_GUARD_UUID_8dcfeaaf_f4d9_417d_bf79_c39f6e9e6c8d
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
void instrumentRadioModel(double X, double Y, double* dX, double* dY);

#ifdef __cplusplus
}
#endif
#endif
#endif


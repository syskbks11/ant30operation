#ifndef INCLUDE_GUARD_UUID_5ecc79b8_a434_4ba2_90f9_fd50b0fc5624
#define INCLUDE_GUARD_UUID_5ecc79b8_a434_4ba2_90f9_fd50b0fc5624
/*!
\file ifCalc2.h
\author NAGAI Makoto
\date 2008.11.15
\brief Manages calculation of frequency tracking for 32-m telescope & 30-cm telescope
*/

#ifndef __IFCALC2_H__
#define __IFCALC2_H__

#ifdef __cplusplus
extern "C"{
#endif




int calc2Init();
int calc2Start();
void calc2End();
int calc2IfCal();
void calc2PrintResults();
double calc2GetXoutDflkfq();

#ifdef __cplusplus
}
#endif
#endif
#endif


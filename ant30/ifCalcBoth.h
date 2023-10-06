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

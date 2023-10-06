/*!
\file trk45Sub.h
\date 2007.01.31
\author Y.Koide
\brief trk45 header for C++
*/
#ifndef __TRK45SUB_H__
#define __TRK45SUB_H__

#include "track.h"
#include "antvl.h"

extern "C"{
  int trk_00(char*, char*, char*);
  int trk_02(double, double*, double*);
  int trk_03_cmd(char*, char*, int*, double*, double**, double*, char*);
  int trk_10(int, int, int, int, double*, double);
  int trk_20(double, XTRKI, XTRKO*);
  int trk_20_cmd(double, double*, double*, double*);
  int trk_30(double*, double*, double*, double, double*, double*);
  int tjcd2m(char*, double*);
  int tjm2cd(double, char*);
  int trk_ant_(double*, double*, double*, double*, double*, double*, double*, double*, double*);
  int trk_antvl_(XANTVL, double*, char*);
  int trk_velo_(double*);
}

#endif

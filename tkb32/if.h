/*!
\file if.h
\author Y.Koide
\date 2006.11.28
\brief IFÀ©¸æÍÑ
* 
* Changed by NAGAI Makoto
* for 32-m telescope & 30-cm telescope
* 
*/
#ifndef __IF_H__
#define __IF_H__
#ifdef __cplusplus
extern "C"{
#endif

  int ifInit();
  int ifEnd();
  int ifStart();
  int ifRepeat();
  int ifUpdate();
  void ifSetLo1Freq(double Freq);
  void ifSetLo1Amp(double Amp);
  void ifSetLo1Att(double Att);
  void ifSetLo2Freq(double Freq);
  void ifSetLo2Amp(double Amp);
  int ifSetAttOnOffR(int OnOffR);

double ifGetLo1PFreq();
double ifGetLo1PAmp();
double ifGetLo1PAtt();
double ifGetLo1RFreq();
double ifGetLo1RAmp();
double ifGetLo1RAtt();
double ifGetLo2PFreq();
double ifGetLo2PAmp();
double ifGetLo2RFreq();
double ifGetLo2RAmp();
//double ifGetVRAD();//081121 moved to calc2.h
/*
  void ifGetLo1PFreq(double* Lo1PFreq);
  void ifGetLo1PAmp(double* Lo1PAmp);
  void ifGetLo1PAtt(double* Lo1PAtt);
  void ifGetLo1RFreq(double* Lo1RFreq);
  void ifGetLo1RAmp(double* Lo1RAmp);
  void ifGetLo1RAtt(double* Lo1RAtt);
  void ifGetLo2PFreq(double* Lo2PFreq);
  void ifGetLo2PAmp(double* Lo2PAmp);
  void ifGetLo2RFreq(double* Lo2RFreq);
  void ifGetLo2RAmp(double* Lo2RAmp);
  void ifGetVRAD(double* VRAD);
*/

#ifdef __cplusplus
}
#endif
#endif

/*!
\file netProt.h
\author Y.Koide
\date 2006.11.23
\brief tkb32 network protocol
*/

#ifndef __PROT_H__
#define __PROT_H__


//!ChopperWheel
typedef struct sChopProt{
  int reqSet;
    
  int ansState;
  int ansMotor;
  float ansTemp1;
  float ansTemp2;
  int ansIntLock;
  int ansError;
}tChopProt;

//!Frontend
typedef struct sFrontProt{
  int reqState;

  double ansTemp;
  double ansVacuum;
  int ans;
}tFrontProt;

//!1stIF
typedef struct sIF1Prot{
  float reqAtt;
  float reqLo1Freq;
  float reqLo1Amp;

  float ansAtt;
  float ansLo1Freq;
  float ansLo1Amp;
}tIF1Prot;

//!2ndIF
typedef struct sIF2Prot{
  float reqLo2Freq;
  float reqLo2Amp;

  float ansLo2Freq;
  float ansLo2Amp;
}tIF2Prot;

#endif __PROT_H__

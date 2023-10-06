/*!
\file netu.cpp
\author Y.Koide
\date 2006.11.23
\brief Network Utility
*/
#include<memory.h>
#include"netu.h"

static const char version[] = "0.0.0";

/*! \fn const char* netuVersion()
\return version
*/
const char* netuVersion(){
  return version;
}

/*! \fn void netuCnvShort(unsigned char* pa)
\brief LabView�Ƃ̒ʐM���Ƀr�b�N�G���f�B�A���Ƃ̑��ݕϊ����s�Ȃ��B
\brief 2�o�C�g�̌^�ɑΉ�
*/
void netuCnvShort(unsigned char* pa){
  int i,n;
  unsigned char b[2];
  unsigned char* pb;

  n = 2;
  pb = (unsigned char*)b;
  memcpy(pb, pa, n);
  for(i=0; i<n; i++){
    pa[i]=pb[n-i-1];
  }
  return;
}

/*! \fn void netuCnvInt(unsigned char* pa)
\brief LabView�Ƃ̒ʐM���Ƀr�b�N�G���f�B�A���Ƃ̑��ݕϊ����s�Ȃ��B
\brief 4�o�C�g�̌^�ɑΉ�
*/
void netuCnvInt(unsigned char* pa){
  int i,n;
  unsigned char b[4];
  unsigned char* pb;

  n = 4;
  pb = (unsigned char*)b;
  memcpy(pb, pa, n);
  for(i=0; i<n; i++){
    pa[i]=pb[n-i-1];
  }
  return;
}

/*! \fn void netuCnvLong(unsigned char* pa)
\brief LabView�Ƃ̒ʐM���Ƀr�b�N�G���f�B�A���Ƃ̑��ݕϊ����s�Ȃ��B
\brief 8�o�C�g�̌^�ɑΉ�
*/
void netuCnvLong(unsigned char* pa){
  int i,n;
  unsigned char b[8];
  unsigned char* pb;

  n = 8;
  pb = (unsigned char*)b;
  memcpy(pb, pa, n);
  for(i=0; i<n; i++){
    pa[i]=pb[n-i-1];
  }
  return;
}

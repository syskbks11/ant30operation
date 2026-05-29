/*!
\file netu.h
\author Y.Koide
\date 2006.11.23
\brief Network Utility
*/
#ifndef __NETU_H__
#define __NETU_H__

#ifdef __cplusplus
extern "C"{
#endif

  const char* netuVersion();
  void netuCnvShort(unsigned char* a);
  void netuCnvInt(unsigned char* a);
  void netuCnvLong(unsigned char* a);

#ifdef __cplusplus
}
#endif

#endif

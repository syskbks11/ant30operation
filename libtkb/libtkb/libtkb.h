/*!
\file libtkb.h
\date 2007.11.12
\author Y.Koide
\brief 主要なモジュールをまとめたライブラリ
*/
#ifndef __LIBTKB_H__
#define __LIBTKB_H__

#include <stdio.h>
/*! u */
#define uMWrite(str, x1, x2, x3, x4) \
{ \
  fprintf(stdout, "%s ", uGetTime()); \
  fprintf(stdout, str, x1, x2, x3, x4); \
  fprintf(stdout, "\n"); \
  if(uFp()){ \
    fprintf(uFp(), "[%s-%s-%d] ", uGetDate(), __FILE__, __LINE__); \
    fprintf(uFp(), str, x1, x2, x3, x4); \
    fprintf(uFp(), "\n"); \
  } \
}

#define uEWrite(str, x1, x2, x3, x4) \
{ \
  fprintf(stderr, "%s ", uGetTime()); \
  fprintf(stderr, str, x1, x2, x3, x4); \
  fprintf(stderr, "\n"); \
  if(uFp()){ \
    fprintf(uFp(), "[%s-%s-%d] ERROR ", uGetDate(), __FILE__, __LINE__); \
    fprintf(uFp(), str, x1, x2, x3, x4); \
    fprintf(uFp(), "\n"); \
    exit(1); \
  } \
}

#define uM(str) { uMWrite((str), NULL, NULL, NULL, NULL); }
#define uM1(str,x1) { uMWrite((str), (x1), NULL, NULL, NULL); }
#define uM2(str,x1,x2) { uMWrite((str), (x1), (x2), NULL, NULL); }
#define uM3(str,x1,x2,x3) { uMWrite((str), (x1), (x2), (x3), NULL); }
#define uM4(str,x1,x2,x3,x4) { uMWrite((str), (x1), (x2), (x3), (x4)); }

#define uE(str) { uEWrite((str), NULL, NULL, NULL, NULL); }
#define uE1(str,x1) { uEWrite((str), (x1), NULL, NULL, NULL); }
#define uE2(str,x1,x2) { uEWrite((str), (x1), (x2), NULL, NULL, NULL); }
#define uE3(str,x1,x2,x3) { uEWrite((str), (x1), (x2), NULL, NULL); }
#define uE4(str,x1,x2,x3,x4) { uEWrite((str), (x1), (x2), (x3), NULL); }



#ifdef __cplusplus
template <class T> T uEndian(T* p);
extern "C"{
#endif

  /*! conf */
  const char* confVersion();
  int confInit();
  int confAddFile(const char* fileName);
  int confAddStr(const char* str);
  const char* confGetStr(const char* key);
  int confSetKey(const char* _setKey);
  const char* confGetVal();
  const char* confGetAllKeyVal();
  void confPrint();

  /*! conf2 */
  const char* conf2Version();
  void* conf2Init();
  int conf2End(void* _p);
  int conf2AddFile(void* _p, const char* fileName);
  int conf2AddStr(void* _p, const char* str);
  const char* conf2GetStr(void* _p, const char* key);
  int conf2SetKey(void* _p, const char* _setKey);
  const char* conf2GetVal(void* _p);
  const char* conf2GetAllKeyVal(void* _p);
  const char** conf2GetAllKeyVal2(void* _p);
  void conf2Print(void* _p);

  /*! netcl */
  const char* netclVersion();
  void* netclInit(const char* serverName, const int serverPort);
  void* netclInitWithTimeOut(const char* serverName, const int serverPort, const double t);
  int netclEnd(void* _param);
  void netclSetTimeOut(void* _param, const double t);
  int netclWrite(void* _param, const unsigned char* buf, const size_t bufSize);
  int netclRead(void* _param, unsigned char* buf, size_t bufSize);

  /*! netsv */
  const char* netsvVersion();
  void* netsvInit(const int serverPort);
  void* netsvInitWithProcessMax(const int serverPort, const int processMax);
  int netsvEnd(void* _param);
  int netsvWaiting(void* _param);
  void* netsvWaitingProcess(void* _param);
  int netsvDisconnect(void* _param);
  int netsvDisconnectProcess(void* _param);
  void netsvSetTimeOut(void* _param, const double t);
  int netsvWrite(void* _param, const unsigned char* buf, const size_t bufSize);
  int netsvRead(void* _param, unsigned char* buf, size_t bufSize);

  /*! netu */
  const char* netuVersion();
  void netuCnvShort(unsigned char* a);
  void netuCnvInt(unsigned char* a);
  void netuCnvLong(unsigned char* a);

  /*! rs232c */
  const char* rs232cVersion();
  int rs232cInit(int comNo, int bps, int byteSize, int parityBit, int stopBit);
  int rs232cEnd(int comNo);
  int rs232cWrite(int comNo, unsigned char* buf, const unsigned int bufSize);
  int rs232cRead(int comNo, unsigned char* buf, const unsigned int bufSize);

  /*! thrd */
  void* thrdInit(void* (*func)(void*), void* arg);
  int thrdEnd(void* _p);
  int thrdStart(void* _p);
  int thrdStop(void* _p);

  /*! tm */
  const char* tmVersion();
  void* tmInit();
  int tmEnd(void* _p);
  int tmReset(void* _p);
  double tmGetLag(void* _p);
  const char* tmGetTimeStr(void* _p, double offset);
  double tmGetDiff(const time_t t, const double msec);
  int tmSleepMSec(double msec);
  void tmGetTime(double offset, time_t* t, int* msec);

  /*! u */
  const char* uVersion();
  int uInit(const char* projectName);
  int uEnd();
  const char* uGetLogName();
  void uLock();
  void uUnLock();
  FILE* uFp();
  const char* uGetDate();
  const char* uGetTime();
  short uEndianShort(short* pa);
  int uEndianInt(int* pa);
  long long uEndianLongLong(long long* pa);
  int uSigFunc(void(*_sigFunc)(void));

#ifdef __cplusplus
}
#endif

#endif /*!< __LIBTKB_H__ */

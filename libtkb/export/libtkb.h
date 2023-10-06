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
typedef struct sNetParam netClass_t;//081125 in
  const char* netclVersion();
// 081125 in
netClass_t* netclInit(const char* serverName, const int serverPort);
netClass_t* netclInitWithTimeOut(const char* serverName, const int serverPort, const double t);
int netclEnd(netClass_t* _param);
void netclSetTimeOut(netClass_t* _param, const double t);
int netclSelectWrite(netClass_t* _param);
int netclSelectRead(netClass_t* _param);
int netclWrite(netClass_t* _param, const unsigned char* buf, const size_t bufSize);
int netclRead(netClass_t* _param, unsigned char* buf, size_t bufSize);
/* 081125 out
  int netclEnd(void* _param);
  void netclSetTimeOut(void* _param, const double t);
  int netclSelectWrite(void* _param);
  int netclSelectRead(void* _param);
  int netclWrite(void* _param, const unsigned char* buf, const size_t bufSize);
  int netclRead(void* _param, unsigned char* buf, size_t bufSize);
*/
  /*! netsv */
typedef struct sNetParam netServerClass_t;//081125 in

  const char* netsvVersion();
netServerClass_t* netsvInit(const int serverPort);
netServerClass_t* netsvInitWithProcessMax(const int serverPort, const int processMax);
int netsvEnd(netServerClass_t* _param);
int netsvWaiting(netServerClass_t* _param);
netServerClass_t* netsvWaitingProcess(netServerClass_t* _param);
int netsvDisconnect(netServerClass_t* _param);
int netsvDisconnectProcess(netServerClass_t* _param);
void netsvSetTimeOut(netServerClass_t* _param, const double t);
int netclSelectWrite(netServerClass_t* _param);
int netclSelectRead(netServerClass_t* _param);
int netsvWrite(netServerClass_t* _param, const unsigned char* buf, const size_t bufSize);
int netsvRead(netServerClass_t* _param, unsigned char* buf, size_t bufSize);

  /*! netu */
  const char* netuVersion();
  void netuCnvShort(unsigned char* a);
  void netuCnvInt(unsigned char* a);
  void netuCnvLong(unsigned char* a);

  /*! rs232c */
  const char* rs232cVersion();
  int rs232cInit(int comNo, int bps, int byteSize, int parityBit, int stopBit);
  int rs232cEnd(int comNo);
  int rs232cWrite(int comNo, const unsigned char* buf, const unsigned int bufSize);
  int rs232cRead(int comNo, unsigned char* buf, const unsigned int bufSize);

  /*! thrd */
/* 090810 out
  void* thrdInit(void* (*func)(void*), void* arg);
  int thrdEnd(void* _p);
  int thrdStart(void* _p);
  int thrdStop(void* _p);
*/
typedef struct thrd_s thrdClass_t;//090810 in
thrdClass_t* thrdInit(void* (*func)(void*), void* arg);
int thrdEnd(thrdClass_t* _p);
int thrdStart(thrdClass_t* _p);
int thrdStop(thrdClass_t* _p);

  /*! tm */
  const char* tmVersion();

typedef struct sTm tmClass_t;//081117 in
tmClass_t* tmInit();
//  void* tmInit();
int tmEnd(tmClass_t* _p);
//  int tmEnd(void* _p);
int tmReset(tmClass_t* _p);
//  int tmReset(void* _p);
double tmGetLag(const tmClass_t* _p);
//  double tmGetLag(void* _p);
const char* tmGetTimeStr(tmClass_t* _p, double offset);
//  const char* tmGetTimeStr(void* _p, double offset);

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

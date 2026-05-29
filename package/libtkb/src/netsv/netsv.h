/*!
\file netsv.h
\author Y.Koide
\date 2006.12.10
\brief Network TCP Server Program.
\brief Auto connection version.
*/
#ifndef __NETSV_H__
#define __NETSV_H__

#ifdef __cplusplus
extern "C"{
#endif

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
/*
  void* netsvInit(const int serverPort);
  void* netsvInitWithProcessMax(const int serverPort, const int processMax);
  int netsvEnd(void* _param);
  int netsvWaiting(void* _param);
  void* netsvWaitingProcess(void* _param);
  int netsvDisconnect(void* _param);
  int netsvDisconnectProcess(void* _param);
  void netsvSetTimeOut(void* _param, const double t);
  int netclSelectWrite(void* _param);
  int netclSelectRead(void* _param);
  int netsvWrite(void* _param, const unsigned char* buf, const size_t bufSize);
  int netsvRead(void* _param, unsigned char* buf, size_t bufSize);
*/
#ifdef __cplusplus
}
#endif
#endif

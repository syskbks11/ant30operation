/*!
\file netcl.h
\author Y.Koide
\date 2006.11.24
\brief Network TCP Client Program.
\brief Auto connection version.
*/

#ifndef __NETCL_H__
#define __NETCL_H__

#ifdef __cplusplus
extern "C"{
#endif

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
  void* netclInit(const char* serverName, const int serverPort);
  void* netclInitWithTimeOut(const char* serverName, const int serverPort, const double t);
  int netclEnd(void* _param);
  void netclSetTimeOut(void* _param, const double t);
  int netclSelectWrite(void* _param);
  int netclSelectRead(void* _param);
  int netclWrite(void* _param, const unsigned char* buf, const size_t bufSize);
  int netclRead(void* _param, unsigned char* buf, size_t bufSize);
*/
#ifdef __cplusplus
}
#endif
#endif

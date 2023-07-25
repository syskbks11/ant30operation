/*!
\file netcl.cpp
\author Y.Koide
\date 2006.11.24
\brief Network TCP Client Program.
\brief Auto connection version.
*/

#ifdef WIN32
#pragma comment (lib, "c:/Program Files//Microsoft Visual Studio 8/VC/PlatformSDK/Lib/WSock32.lib")
#pragma comment (lib, "c:/Program Files//Microsoft Visual Studio 8/VC/PlatformSDK/Lib/WinInet.lib")
//#pragma comment (lib, "\"c:/Program Files/Microsoft Visual Studio 8/VC/PlatformSDK/Lib/WSock32.lib\"")
//#pragma comment (lib, "\"c:/Program Files/Microsoft Visual Studio 8/VC/PlatformSDK/Lib/WinInet.lib\"")
#include <winsock2.h>
#include <ws2tcpip.h>
#include <io.h>
#else
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/uio.h>
#include <unistd.h>
#include <sys/param.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

#include <stdio.h>
#include <memory>
#include <string.h>
#include <time.h>
#include "../u/u.h"
#include "netcl.h"

static const char version[] = "0.0.0";

typedef struct sNetParam{
#ifdef WIN32
  WSADATA wsaData;
#endif
  char* srvName;
  int srvPort;
  struct hostent *hostEntry;
  int sock;
  struct sockaddr_in sockadd;
  timeval waitTimeRead;
}tNetParam;

static int _netclConnect(tNetParam* param);

/*! \fn const char* netclVersion()
\return netclのバージョン文字列を返す 
*/
const char* netclVersion(){
  return version;
}

/*! \fn void* netclInit(const char* serverName, const int serverPort)
\brief 初期化を行う。
\param[in] serverName 接続先サーバのホストネームまたはIP
\param[in] serverPort 接続先サーバのポート
\return netclパラメータ NULL:Error
*/
tNetParam* netclInit(const char* serverName, const int serverPort){
//void* netclInit(const char* serverName, const int serverPort){
  return netclInitWithTimeOut(serverName, serverPort, -1);
}

/*! \fn void* netclInitWithTimeOut(const char* serverName, const int serverPort, const double t)
\brief タイムアウトつきで初期化を行う。
\param[in] serverName 接続先サーバのホストネームまたはIP
\param[in] serverPort 接続先サーバのポート
\param[in] t タイムアウト時間 sec
\return NULL netclパラメータ
*/
tNetParam* netclInitWithTimeOut(const char* serverName, const int serverPort, const double t){
//void* netclInitWithTimeOut(const char* serverName, const int serverPort, const double t){
  tNetParam* param;
  int ret;

  if(strlen(serverName)<=0 || serverPort<=0){
    uM2("netclInit(); serverName:serverPort %s:%d error",serverName,serverPort);
    return NULL;
  }

  param = (tNetParam*)malloc(sizeof(tNetParam));
  memset(param, 0, sizeof(tNetParam));
#ifdef WIN32
  if(WSAStartup(MAKEWORD(1,1), &param->wsaData)){
    uM("_netclConnect(); WSAStartup() error!!\n");
    netclEnd((void*)param);
    return NULL;
  }
#endif
  param->srvName = (char*)malloc((strlen(serverName)+1));
  memcpy(param->srvName, serverName, strlen(serverName)+1);
  param->srvPort = serverPort;
  param->sock = -1;
  param->waitTimeRead.tv_sec = -1; //!< 待ち時間sec(-1でselect処理しない)
  param->waitTimeRead.tv_usec = 0; //!< 待ち時間usec
  if(t >= 0){
    netclSetTimeOut(param, t);
//    netclSetTimeOut((void*)param, t);
  }

  ret = _netclConnect(param);
  if(ret < 0){
    uM2("netclInit(); Connection %s:%d error!!", param->srvName, param->srvPort);
    //netclEnd(param);
    param->sock = -1;
  }
  else{
    uM2("netclInit(); Connection %s:%d", param->srvName, param->srvPort);
  }
  return param;
//  return (void*)param;
}

/*! \fn int netclEnd(void* _param)
\brief 終了処理を行う。
\param[in] _param netclパラメータ
\return 0:正常終了 -1:Error
*/
int netclEnd(tNetParam* _param){
//int netclEnd(void* _param){
  tNetParam* param=(tNetParam*)_param;
  if(param){
    if(param->sock!=-1){
#ifdef WIN32
      closesocket(param->sock);
#else
      close(param->sock);
#endif
    }
    if(param->srvName){
      free(param->srvName);
      param->srvName=NULL;
    }
#ifdef WIN32
    if(WSACleanup() == SOCKET_ERROR)    {
      uM("netclEnd(); WSACleanup() error!\n");
    }
#endif
    free(param);
    param=NULL;
  }
  else{
    return -1;
  }
  return 0;
}

/*! \fn void netclSetTimeOut(void* _param, double t)
\brief 受信時のタイムアウトを定義する
\param[in] _param netclパラメータ
\param[in] t タイムアウト時間 sec
*/
void netclSetTimeOut(tNetParam* _param, const double t){
//void netclSetTimeOut(void* _param, const double t){
  tNetParam* param = (tNetParam*)_param;
  long sec;
  long usec;

  if(t >= 0){
    sec = (long)t;
    usec = 1000000 * (t - (double)sec);
    param->waitTimeRead.tv_sec = sec;
    param->waitTimeRead.tv_usec = usec;
  }
  else{
    param->waitTimeRead.tv_sec = -1;
    param->waitTimeRead.tv_usec = 0;
  }   
  return;
}

/*! \fn int netclSelectWrite(void* _param)
\brief データ送信が可能かを確認する
\param[in] _param netclパラメータ
\reutrn 0:送信可能 -1:送信不能 -2:Error
*/
int netclSelectWrite(tNetParam* _param){
//int netclSelectWrite(void* _param){
  tNetParam* param = (tNetParam*)_param;
  fd_set fdw;
  fd_set fde;

  FD_ZERO(&fdw);
  FD_ZERO(&fde);
  FD_SET(param->sock, &fdw);
  FD_SET(param->sock, &fde);
  select(FD_SETSIZE, NULL, &fdw, &fde, &param->waitTimeRead);
  if(FD_ISSET(param->sock, &fde)){
    //! 例外あり
    return -2;
  }
  if(!FD_ISSET(param->sock, &fdw)){
    //! 書き込み不能(データなし)
    return -1;
  }
  return 0;
}

/*! \fn int netclSelectRead(void* _param)
\brief データが届いているかを確認する
\param[in] _param netclパラメータ
\return 0:Success 1:ReciveData -1:Error
*/
int netclSelectRead(tNetParam* _param){
//int netclSelectRead(void* _param){
  tNetParam* param = (tNetParam*)_param;
  fd_set fdr;
  fd_set fde;

  FD_ZERO(&fdr);
  FD_ZERO(&fde);
  FD_SET(param->sock, &fdr);
  FD_SET(param->sock, &fde);
  select(FD_SETSIZE, &fdr, NULL, &fde, &param->waitTimeRead);
  if(FD_ISSET(param->sock, &fde)){
    //! 例外あり
    return -1;
  }
  if(FD_ISSET(param->sock, &fdr)){
    //! 受信データあり
    return 1;
  }
  return 0;
}

/*! \fn int netclWrite(void* _param, const unsigned char* buf, const size_t bufSize)
\brief データ送信。
\param[in] _param netclパラメータ
\param[in] buf 送信データ
\param[in] bufSize 送信データサイズ
\return >=0:送信したバイト数　<0:Errror
*/
int netclWrite(tNetParam* _param, const unsigned char* buf, const size_t bufSize){
//int netclWrite(void* _param, const unsigned char* buf, const size_t bufSize){
  int sockaddLen;
  int ret;
  fd_set fdw;
  fd_set fde;

  tNetParam* param = (tNetParam*)_param;
  if(param->sock==-1){
    ret = _netclConnect(param);
    if(ret==0){
      uM2("netclWrite(); Reconnect %s:%d", param->srvName, param->srvPort);
    }
    else{
#ifdef WIN32
      closesocket(param->sock);
#else
      close(param->sock);
#endif
      param->sock=-1;
      return -2;
    }
  }

  //! タイムアウト処理
  if(param->waitTimeRead.tv_sec >= 0){
    FD_ZERO(&fdw);
    FD_ZERO(&fde);
    FD_SET(param->sock, &fdw);
    FD_SET(param->sock, &fde);
    select(FD_SETSIZE, NULL, &fdw, &fde, &param->waitTimeRead);
    if(!FD_ISSET(param->sock, &fdw)){
      //! 書き込み不能(データなし)
      //return 0;
    }
    if(FD_ISSET(param->sock, &fde)){
      //! 例外あり
      return -1;
    }
  }

  sockaddLen = (int)sizeof(param->sockadd);
  ret=sendto(param->sock, (const char*)buf, bufSize, 0, (const sockaddr*)&param->sockadd, (socklen_t)sockaddLen);
  //if(ret<=0){
  if(ret<0){
    //if(ret<0){
      uM1("netclWrite(); write(); reuturn %d", ret);
    //}
    uM2("netclWrite(); Disconnect %s:%d", param->srvName, param->srvPort);
#ifdef WIN32
      closesocket(param->sock);
#else
      close(param->sock);
#endif
    param->sock=-1;
    return -1;
  }
  return ret;
}

/*! \fn int netclRead(void* _param, unsigned char* buf, size_t bufSize)
\brief データを受信する
\param[in] _param netclパラメータ
\param[in] buf 受信データ
\param[in] bufSize 受信データ格納領域サイズ
\return >=0:受信したバイト数 <0:Error
*/
int netclRead(tNetParam* _param, unsigned char* buf, size_t bufSize){
//int netclRead(void* _param, unsigned char* buf, size_t bufSize){
  int sockaddLen;
  int ret;
  fd_set fdr;
  fd_set fde;

  tNetParam* param = (tNetParam*)_param;
  if(param->sock==-1){
    ret = _netclConnect(param);
    if(ret==0){
      uM2("netclRead(); Reconnect %s:%d", param->srvName, param->srvPort);
    }
    else{
#ifdef WIN32
      closesocket(param->sock);
#else
      close(param->sock);
#endif
      param->sock=-1;
      return -2;
    }
  }

  //! タイムアウト処理
  if(param->waitTimeRead.tv_sec >= 0){
    FD_ZERO(&fdr);
    FD_ZERO(&fde);
    FD_SET(param->sock, &fdr);
    FD_SET(param->sock, &fde);
    select(FD_SETSIZE, &fdr, NULL, &fde, &param->waitTimeRead);
    if(!FD_ISSET(param->sock, &fdr)){
      //! 読み込み不能(データなし)
      return -1;
    }
    if(FD_ISSET(param->sock, &fde)){
      //! 例外あり
      return -1;
    }
  }

  sockaddLen = (int)sizeof(param->sockadd);
  ret = recvfrom(param->sock, (char*)buf, bufSize, 0, (struct sockaddr*)&param->sockadd, (socklen_t*)&sockaddLen);
  //if(ret<=0){
  if(ret<0){
    //if(ret<0){
      uM1("netclRead(); read(); return %d", ret);
    //}
    uM2("netclRead(); Disconnect %s:%d", param->srvName, param->srvPort);
#ifdef WIN32
      closesocket(param->sock);
#else
      close(param->sock);
#endif
    param->sock=-1;
    return -1;
  }
  return ret;
}


/*! \fn int _netclConnect(tNetParam* param)
\brief Connect server.
*/
int _netclConnect(tNetParam* param){
  unsigned int addr;

  if(param->sock != -1){
    uM1("_netclConnect(); Socket already opened socket %d", param->sock);
    return 0;
  }
  //uM("_netclConnect();");
  //ソケットオープン
  param->sock=socket(PF_INET, SOCK_STREAM, 0);
  if(param->sock == -1){
    //uM("_netclConnect(); Socket open error!!\n");
    return -1;
  }
  //サーバのIPを取得
  param->hostEntry=gethostbyname(param->srvName);
  if(param->hostEntry == NULL){
    addr=inet_addr(param->srvName);
    param->hostEntry=gethostbyaddr((char*)&addr, 4, AF_INET);
    if(param->hostEntry==NULL){
      //uM1("_netclConnect(); Not found serverName [%s]\n", param->srvName);
#ifdef WIN32
      closesocket(param->sock);
#else
      close(param->sock);
#endif
      return -2;
    }
  }
  memset(&param->sockadd, 0, sizeof(param->sockadd));
  param->sockadd.sin_family=AF_INET;
  param->sockadd.sin_port=htons(param->srvPort);
  param->sockadd.sin_addr=*((struct in_addr*)*param->hostEntry->h_addr_list);
  //サーバに接続
  if(connect(param->sock, (struct sockaddr*)&param->sockadd, sizeof(param->sockadd))==-1){
    //uM2("_netclConnect(); Connection %s:%d error!!\n", param->srvName, param->srvPort);
#ifdef WIN32
      closesocket(param->sock);
#else
      close(param->sock);
#endif
    return -3;
  }
  return 0;
}

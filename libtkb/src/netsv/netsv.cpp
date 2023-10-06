/*!
\file netsv.cpp
\author Y.Koide
\date 2006.12.10
\brief Network TCP Server Program.
\brief Auto connection version.
*/
#ifdef WIN32
#pragma comment (lib, "c:/Program Files/Microsoft Visual Studio 8/Vc/PlatformSDK/Lib/WSock32.lib")
#pragma comment (lib, "c:/Program Files/Microsoft Visual Studio 8/Vc/PlatformSDK/Lib/WinInet.lib")
//#pragma comment (lib, "\"c:/Program Files/Microsoft Visual Studio 8/Vc/PlatformSDK/Lib/WSock32.lib\"")
//#pragma comment (lib, "\"c:/Program Files/Microsoft Visual Studio 8/Vc/PlatformSDK/Lib/WinInet.lib\"")
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
#include <memory.h>
#include <stdlib.h>
#include <time.h>
#include "../u/u.h"
#include "netsv.h"

static const char version[] = "0.0.0";

typedef struct sNetParam{
#ifdef WIN32
  WSADATA wsaData;
#endif
  int srvPort;                 //!< Port number
  int processMax;              //!< Number of maximum process(connections)
  struct hostent *hostEntry;   //!< 
  int sockListen;              //!< Listen socket
  int* sock;                   //!< �����̃\�P�b�g�Ǘ�����
  int sockNo;                  //!< ������sock�Ǘ��ԍ��B0:Listen socket  over 1:connection socket
  struct sockaddr_in sockadd;  //!< ip,port,...
  struct sockaddr_in sockFrom; //!< 
  int sock_optval;             //!< 
  timeval waitTimeRead;        //!< Select time out
}tNetParam;

/*! \fn const char* netsvVersion()
\return  netsv�̃o�[�W�����������Ԃ� 
*/
const char* netsvVersion(){
  return version;
}

/*! \fn void* netsvInit(const int serverPort)
\brief netsv������������ۂɌĂяo���B�P��N���C�A���g�̂ݎ�t�\�B
\param[in] serverPort �҂���������|�[�g�ԍ�
\return NULL:���s other:�f�[�^�\���̂̃|�C���^
*/
tNetParam* netsvInit(const int serverPort){
//void* netsvInit(const int serverPort){
  const int processMax = 1;
  return netsvInitWithProcessMax(serverPort, processMax);
}

/*! \fn netsvInitWithProcessMax(const int serverPort, const int processMax)
\brief netsv������������ۂɌĂяo���BprocessMax�̐������N���C�A���g����t�\�B
\param[in] serverPort �҂���������|�[�g�ԍ�
\param[in] processMax �ڑ�����N���C�A���g�̍ő吔
\return NULL:���s other:�f�[�^�\���̂̃|�C���^
*/
tNetParam* netsvInitWithProcessMax(const int serverPort, const int processMax){
//void* netsvInitWithProcessMax(const int serverPort, const int processMax){
  tNetParam* param;

  if(serverPort <= 0){
    uM1("netsvInitWithProcessMax(); serverPort %d<=0 error", serverPort);
    return NULL;
  }
  if(processMax <= 0){
    uM1("netsvInitWithProcessMax(); processMax %d<=0 error!!", processMax);
    return NULL;
  }

  param = (tNetParam*) malloc(sizeof(tNetParam));
  memset(param, 0, sizeof(tNetParam));
#ifdef WIN32
  if(WSAStartup(MAKEWORD(1,1), &param->wsaData)){
    printf("netsvInitWithProcessMax();WSAStartup() error!!\n");
    netsvEnd(param);
//    netsvEnd((void*)param);
    return NULL;
  }
#endif
  param->srvPort = serverPort;
  param->processMax = processMax;
  param->sock = (int*) malloc(sizeof(int) * param->processMax);
  memset(param->sock, -1, sizeof(int) * param->processMax);
  param->sockNo = -1;
  param->waitTimeRead.tv_sec = -1; //!< �҂�����sec(-1��select�������Ȃ�)
  param->waitTimeRead.tv_usec = 0; //!< �҂�����usec
  //!Listen Socket Open
  param->sockListen=(unsigned int)socket(AF_INET, SOCK_STREAM, 0);
  if(param->sockListen == -1){
    netsvEnd(param);
//    netsvEnd((void*)param);
    uM1("netsvInitWithProcessMax(); Socket open error port %d",param->srvPort);
    return NULL;
  }
  else{
    uM1("netsvInitWithProcessMax(); Socket open port %d", param->srvPort);
  }

  param->sock_optval=1;
  if(setsockopt(param->sockListen, SOL_SOCKET, SO_REUSEADDR, (const char*)&param->sock_optval, sizeof(param->sock_optval))==-1){
    netsvEnd(param);
//    netsvEnd((void*)param);
    uM("netsvInitWithProcessMax(); setsockopt error!!");
    return NULL;
  }

  memset(&param->sockadd, 0, sizeof(param->sockadd));
  param->sockadd.sin_family=AF_INET;
  param->sockadd.sin_port=htons(param->srvPort);
  param->sockadd.sin_addr.s_addr=htonl(INADDR_ANY);
  if(bind(param->sockListen, (struct sockaddr*)&param->sockadd, sizeof(param->sockadd))==-1){
      netsvEnd(param);
//      netsvEnd((void*)param);
      uM("netsvInitWithProcess(); Socket bind error!!");
      return NULL;
  }
  if(listen(param->sockListen, SOMAXCONN)==-1){
    netsvEnd(param);
//    netsvEnd((void*)param);
    uM("netsvInitWithProcess(); Socket listen error!!");
    return NULL;
  }

  return param;
//  return (void*)param;
}

/*! \fn int netsvEnd(void* _param)
\brief �I�����ɌĂяo���B
\param[in] _param netsv �p�����[�^�[
\return 0:Success
*/
int netsvEnd(tNetParam* _param){
//int netsvEnd(void* _param){
  tNetParam* param=(tNetParam*)_param;
  if(param){
    if(param->sockListen!=-1){
#ifdef WIN32
      closesocket(param->sockListen);
#else
      close(param->sockListen);
#endif
      param->sockListen=-1;
    }
    if(param->sock){
      for(int i = 0; i < param->processMax; i++){
	if(param->sock[i] == -1){
	  continue;
	}
#ifdef WIN32
	closesocket(param->sock[i]);
#else
	close(param->sock[i]);
#endif
	param->sock[i]=-1;
      }
      free(param->sock);
      param->sock = NULL;
    }
#ifdef WIN32
    if(WSACleanup() == SOCKET_ERROR){
      printf("netsvEnd(); WSACleanup() error!\n");
    }
#endif
    free(param);
    param=NULL;
  }
  return 0;
}

/*! \fn int netsvWaiting(void* _param)
\brief Server listening
\param[in] _param 
\retval 0 Success
\retval -1 Not connection
\retval -2 Select error catche
\retval -3 Over processMax
*/
int netsvWaiting(tNetParam* _param){
//int netsvWaiting(void* _param){
  tNetParam* param=(tNetParam*)_param;
  fd_set fdr;
  fd_set fde;

  //! �^�C���A�E�g����
  if(param->waitTimeRead.tv_sec >= 0){
    FD_ZERO(&fdr);
    FD_ZERO(&fde);
    FD_SET(param->sockListen, &fdr);
    FD_SET(param->sockListen, &fde);
    select(FD_SETSIZE, &fdr, NULL, &fde, &param->waitTimeRead);
    if(!FD_ISSET(param->sockListen, &fdr)){
      //! �ǂݍ��ݕs�\(�f�[�^�Ȃ�)
      return -1;
    }
    if(FD_ISSET(param->sockListen, &fde)){
      //! ��O����
      return -2;
    }
  }

  //! socket�ԍ����i�[
  int i = 0;
  for(i = 0; i < param->processMax; i++){
    if(param->sock[i] == -1){
      param->sockNo = i;
      break;
    }
  }
  if(i == param->processMax){
    //! �ڑ��ő吔�ɒB���Ă��邽�ߐڑ��҂�����
    return -3;
  }

  //! �ڑ����s����܂őҋ@
  int sockFromLen = (int)sizeof(param->sockFrom);
  param->sock[param->sockNo]=accept(param->sockListen, (struct sockaddr*)&param->sockFrom, (socklen_t*)&sockFromLen);
  if(param->sock[param->sockNo]==-1){
    uM2("netsvWaiting(); Connection accept error %s:%d",
	inet_ntoa(param->sockFrom.sin_addr),
	ntohs(param->sockFrom.sin_port));
    return -1;
  }

  //uM("netsvWaiting(); gethostbyaddr();");
  param->hostEntry=gethostbyaddr((char*)&param->sockFrom.sin_addr.s_addr, sizeof(param->sockFrom.sin_addr), AF_INET);
  if(param->hostEntry!=NULL){
    uM3("netsvWaiting(); Connection accept %s(%s):%d",
  	param->hostEntry->h_name,
  	inet_ntoa(param->sockFrom.sin_addr),
  	ntohs(param->sockFrom.sin_port));
  }
  else{
    uM2("netsvWaiting(); Connection accept %s:%d",
	inet_ntoa(param->sockFrom.sin_addr),
	ntohs(param->sockFrom.sin_port));
  }
  return 0;
}

/*! \fn void* netsvWaitingProcess(void* _param)
\brief netsv�p�����[�^���q�v���Z�X�p�ɃR�s�[����B�ڑ��𕡐��s���ꍇ�Ɏg�p�B
\param[in] _param netsv�̃p�����[�^�[
\return NULL:error other:netsv pointer
*/
tNetParam* netsvWaitingProcess(tNetParam* _param){
//void* netsvWaitingProcess(void* _param){
  tNetParam* param = (tNetParam*)_param;
  tNetParam* param2;
  int ret;

  ret = netsvWaiting(param);
//  ret = netsvWaiting((void*)param);
  if(ret < 0){
    uM1("netsvWaitingProcess(); netsvWaiting(); ret = %d", ret);
    return NULL;
  }

  //! �R�s�[
  param2 = (tNetParam*) malloc(sizeof(tNetParam));
  if(!param2){
    uM("netsvProcess(); malloc(); error!!");
    return NULL;
  }
  memcpy(param2, param, sizeof(tNetParam));       //!< sock���ւ̃A�h���X���R�s�[���Ďg�p����
  param->sockNo = 0;
  param2->sockListen = -1;                        //!< Listen socket������

	return param2;
//  return (void*)param2;
}

/*! \fn int netsvDisconnect(void* _param)
\brief �\�P�b�g�����
\param[in] _param netsv�̃p�����[�^�[
\return 0:����
*/
int netsvDisconnect(tNetParam* _param){
//int netsvDisconnect(void* _param){
  tNetParam* param=(tNetParam*)_param;

  if(param->sock[param->sockNo] != -1){
#ifdef WIN32
    closesocket(param->sock[param->sockNo]);
#else
    close(param->sock[param->sockNo]);
#endif
    param->sock[param->sockNo] = -1;
  }
  if(param->hostEntry != NULL){
    uM3("netsvDisconnect(); Disconnect port %s(%s):%d",
	param->hostEntry->h_name,
	inet_ntoa(param->sockFrom.sin_addr),
	param->srvPort);
  }

  return 0;
}

/*! \fn int int netsvDisconnectProcess(void* _param)
\brief �\�P�b�g����A�qProcess�ɐ������ꂽparam���폜
\param[in] _param netsv�̃p�����[�^�[
\return 0:����
*/
int netsvDisconnectProcess(tNetParam* _param){
//int netsvDisconnectProcess(void* _param){
  tNetParam* param=(tNetParam*)_param;
  int ret;

  ret = netsvDisconnect(param);
//  ret = netsvDisconnect((void*)param);
  if(!ret){
    //! �����Disconnect���ꂽ��sockListen�������Ȃ�
    //! netsvWaitingProcess();�Ő������ꂽparam���폜
    if(param->sockNo != 0){
      free(param);
    }
  }
  else{
    uM1("netsvDisconnectProcess(); netsvDisconnect(); ret = %d", ret);
    //return ret;
  }
  return ret;
}

/*! \fn int int netsvDisconnectListen(void* _param)
\brief Listen�\�P�b�g��������(fork()�p)
\param[in] _param netsv�̃p�����[�^�[
\return 0:����
*/
/*int netsvDisconnectListen(void* _param){
  tNetParam* param=(tNetParam*)_param;
  if(param){
    if(param->sockListen!=-1){
#ifdef WIN32
      closesocket(param->sockListen);
#else
      close(param->sockListen);
#endif
      param->sockListen=-1;
    }
  }
  return 0;
}*/

/*! \fn void netsvSetTimeOut(void* _param, double t)
\brief ��M���̃^�C���A�E�g���`����
\param[in] _param netcl�p�����[�^
\param[in] t �^�C���A�E�g���ԁB-1�Ȃ�Select�����Ȃ� sec
*/
void netsvSetTimeOut(tNetParam* _param, const double t){
//void netsvSetTimeOut(void* _param, const double t){
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

/*! \fn int netsvSelectWrite(void* _param)
\brief �f�[�^���M���\�����m�F����
\param[in] _param netsv�p�����[�^
\reutrn 0:���M�\ -1:���M�s�\ -2:Error
*/
int netsvSelectWrite(tNetParam* _param){
//int netsvSelectWrite(void* _param){
  tNetParam* param = (tNetParam*)_param;
  fd_set fdw;
  fd_set fde;

  FD_ZERO(&fdw);
  FD_ZERO(&fde);
  FD_SET(param->sock[param->sockNo], &fdw);
  FD_SET(param->sock[param->sockNo], &fde);
  select(FD_SETSIZE, NULL, &fdw, &fde, &param->waitTimeRead);
  if(FD_ISSET(param->sock[param->sockNo], &fde)){
    //! ��O����
    return -2;
  }
  if(!FD_ISSET(param->sock[param->sockNo], &fdw)){
    //! �������ݕs�\(�f�[�^�Ȃ�)
    return -1;
  }
  return 0;
}

/*! \fn int netsvSelectRead(void* _param)
\brief �f�[�^���͂��Ă��邩���m�F����
\param[in] _param netsv�p�����[�^
\return 0:Success 1:ReciveData -1:Error
*/
int netsvSelectRead(tNetParam* _param){
//int netsvSelectRead(void* _param){
  tNetParam* param = (tNetParam*)_param;
  fd_set fdr;
  fd_set fde;

  FD_ZERO(&fdr);
  FD_ZERO(&fde);
  FD_SET(param->sock[param->sockNo], &fdr);
  FD_SET(param->sock[param->sockNo], &fde);
  select(FD_SETSIZE, &fdr, NULL, &fde, &param->waitTimeRead);
  if(FD_ISSET(param->sock[param->sockNo], &fde)){
    //! ��O����
    return -1;
  }
  if(FD_ISSET(param->sock[param->sockNo], &fdr)){
    //! ��M�f�[�^����
    return 1;
  }
  return 0;
}

/*! \fn int netsvWrite(void* _param, const unsigned char* buf, const size_t bufSize)
\brief �f�[�^���M�B
\param[in] _param netsv�p�����[�^
\param[in] buf ���M�f�[�^
\param[in] bufSize ���M�f�[�^�T�C�Y
\return >=0 ���M�����o�C�g��
\return <0 Errror
*/
int netsvWrite(tNetParam* _param, const unsigned char* buf, const size_t bufSize){
//int netsvWrite(void* _param, const unsigned char* buf, const size_t bufSize){
  int sockaddLen;
  int ret;
  fd_set fdw;
  fd_set fde;

  tNetParam* param = (tNetParam*)_param;
  if(param->sock[param->sockNo]==-1){
    return -1;
  }

  //! �^�C���A�E�g����
  if(param->waitTimeRead.tv_sec >= 0){
    FD_ZERO(&fdw);
    FD_ZERO(&fde);
    FD_SET(param->sock[param->sockNo], &fdw);
    FD_SET(param->sock[param->sockNo], &fde);
    select(FD_SETSIZE, NULL, &fdw, &fde, &param->waitTimeRead);
    if(!FD_ISSET(param->sock[param->sockNo], &fdw)){
      //! �������ݕs�\(�f�[�^�Ȃ�)
      //return 0;
    }
    if(FD_ISSET(param->sock[param->sockNo], &fde)){
      //! ��O����
      return -1;
    }
  }

  sockaddLen = (int)sizeof(param->sockadd);
  ret=sendto(param->sock[param->sockNo], (const char*)buf, bufSize, 0, (const sockaddr*)&param->sockadd, (socklen_t)sockaddLen);
  //if(ret<=0){
  if(ret<0){
    //if(ret<0){
      uM1("netsvWrite(); write(); return %d", ret);
      //}
    uM2("netsvWrite(); Disconnect %s:%d",
      inet_ntoa(param->sockFrom.sin_addr),
      ntohs(param->sockFrom.sin_port));
#ifdef WIN32
    closesocket(param->sock[param->sockNo]);
#else
    close(param->sock[param->sockNo]);
#endif
    param->sock[param->sockNo]=-1;
  }
  return ret;
}

/*! \fn int netsvRead(void* _param, unsigned char* buf, size_t bufSize)
\brief �f�[�^����M����
\param[in] _param netsv�p�����[�^
\param[in] buf ��M�f�[�^
\param[in] bufSize ��M�f�[�^�i�[�̈�T�C�Y
\return >=0 ��M�����o�C�g��
\return <0 Error
*/
int netsvRead(tNetParam* _param, unsigned char* buf, size_t bufSize){
//int netsvRead(void* _param, unsigned char* buf, size_t bufSize){
  int sockaddLen;
  int ret;
  fd_set fdr;
  fd_set fde;

  tNetParam* param = (tNetParam*)_param;
  if(param->sock[param->sockNo]==-1){
    return -1;
  }

  //! �^�C���A�E�g����
  if(param->waitTimeRead.tv_sec >= 0){
    FD_ZERO(&fdr);
    FD_ZERO(&fde);
    FD_SET(param->sock[param->sockNo], &fdr);
    FD_SET(param->sock[param->sockNo], &fde);
    select(FD_SETSIZE, &fdr, NULL, &fde, &param->waitTimeRead);
    if(!FD_ISSET(param->sock[param->sockNo], &fdr)){
      //! �ǂݍ��ݕs�\(�f�[�^�Ȃ�)
      return 0;
    }
    if(FD_ISSET(param->sock[param->sockNo], &fde)){
      //! ��O����
      return -1;
    }
  }

  sockaddLen = (int)sizeof(param->sockadd);
  ret = recvfrom(param->sock[param->sockNo], (char*)buf, bufSize, 0, (struct sockaddr*)&param->sockadd, (socklen_t*)&sockaddLen);
  //if(ret<=0){
  if(ret<0){
    //if(ret<0){
      uM1("netsvRead(); read(); return %d", ret);
    //}
    uM2("netsvRead(); Disconnect %s:%d",
	inet_ntoa(param->sockFrom.sin_addr),
	ntohs(param->sockFrom.sin_port));
#ifdef WIN32
    closesocket(param->sock[param->sockNo]);
#else
    close(param->sock[param->sockNo]);
#endif
    param->sock[param->sockNo]=-1;
  }
  return ret;
}

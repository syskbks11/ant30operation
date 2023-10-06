/*!
\file tkb32/tkb32.cpp
\author Y.Koide
\date 2007.01.01
\brief Tsukuba 32m Central Program
*/
#include <stdlib.h>
#include <memory.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>

#include "../libtkb/export/libtkb.h"
#include "compileDate.h"
#include "tkb32Func.h"

static char logPath[] = "../log";

typedef struct sParamTkb32{
  int tkb32Port;
  void* net;
}tParamTkb32;

static tParamTkb32 p;
static int act = 0;
//static int endFlg = 0;

void sigFunc(int sig);
static void setParam();
static void atexitFunc();

/*! \fn int MAIN__(void)
\brief ��������T�[�o�[�̃��C���������B
\brief ��������N���C�A���g(tkb32cl,tkb32clGUI)���烊�N�G�X�g���󂯎��A������tkb32Func.cpp�̊֐��ŏ�������B
\brief �ϑ��������ɒʐM���ؒf���Ă��A�Đڑ�����ƌp�����Ċϑ��ł���悤�ɂȂ��Ă���B
\brief tkb32�̒��ōŏ��Ɏ��s�����֐��BC�����main()�֐��ɑ�������B
\brief 45m�ǔ����C�u������Fortran�ō���Ă��邽�߁AFortran�p�̃��C���֐��ƂȂ�B
\retval 0 ����
*/
int MAIN__(int argc, char* argv[]){
  int ret;
  int reqSize;
  int ansSize;
  long size;
  long sizeCnt;
  unsigned char* req = NULL;
  unsigned char* ans = NULL;
  FILE* fp;
  char tmp[128];
  pid_t pid_me;
  pid_t pid;

  //! Check
  pid_me = 0;
  pid = 0;
  if((fp = popen("pgrep -f -x ./tkb32", "r")) != NULL){
    fgets(tmp, 127, fp);
    pid = atoi(tmp);
    pid_me = getpid();
    pclose(fp);
  }
  else{
    uM("command pgrep error");
  }
  
  //! �v���Z�X��KILL�������Ƃ�
  if(argc == 2 && strcmp(argv[1], "-k") == 0){
    if(pid != NULL && pid != pid_me){
      sprintf(tmp, "kill -s KILL %d", pid);
      system(tmp);
      uM(tmp);
    }
    else{
      uM("Proccess not found");
    }
    return 0;
  }

  //! ���łɋN���ς�
  if(pid != NULL && pid != pid_me){
    printf("Already running pid = %d\n", pid);
    return -1;
  }

  memset(&p, 0, sizeof(p));
  sprintf(tmp, "%s/tkb32", logPath);
  uInit(tmp);
  uM("Starting tkb32");
  uM1("This program is Compiled at %s", COMPILE_DATE);
  confInit();
  confAddFile("../etc/tkb32.conf");
  confPrint();
  setParam();
  p.net=netsvInit(p.tkb32Port);

  //! Regist signal function
  if(signal(SIGINT, sigFunc) == SIG_ERR){
    uM("signal(); SIGINT error!!");
  }
  if(signal(SIGTERM, sigFunc) == SIG_ERR){
    uM("signal(); SIGTERM error!!");  
  }
  if(atexit(atexitFunc)){
    uM("atexit(); error!!");
    return -1;
  }

  ret = init();
  if(ret){
    uM("MAIN__(); init(); error!!");
    end();
    return -1;
  }

  act = 1;
  netsvSetTimeOut(p.net, 1);
  while(act){
    //! �ʐM�҂�
    if(netsvWaiting(p.net) != 0){
      //printf("debug netsvWainiting()\n");
      tmSleepMSec(1000);
      continue;
    }
    while(act){
      ret = netsvRead(p.net, (unsigned char*)&size, 4);
      if(ret < 0){
	uM1("MAIN__(); netsvRead(); ret %d", ret);
	break;
      }
      else if(ret != 4){
	//! �^�C���A�E�g���]���f�[�^�s��
	continue;
      }
      reqSize = size;
      sizeCnt = 4;
      //uM1("debug MAIN__(); reqSize = %d", reqSize);
      req = new unsigned char[reqSize];
      memcpy(req, &size, 4);
      for(int i=0; sizeCnt < reqSize && i<50; i++){
	ret = netsvRead(p.net, req + sizeCnt, reqSize - sizeCnt);
	//uM1("debug MAIN_(); ret=%d", ret);
	if(ret < 0){
	  uM1("MAIN__(); netsvRead(); ret %d", ret);
	  break;
	}
	sizeCnt += ret;
	tmSleepMSec(100);
      }
      if(sizeCnt != reqSize){
	uM2("MAIN__(); netsvRead(); sizeCnt=%d reqSize=%d", ret, reqSize);
	continue;
      }
      ret = setReqData(req, reqSize);
      ret = getAnsData(&ans, &ansSize);
      ret = netsvWrite(p.net, ans, ansSize);
      if(ret < 0){
	uM1("MAIN__(); netsvWrite(); ret %d", ret);
	break;
      }
      else if(ret == 0){
	break;
      }
      if(ret != ansSize){
	uM2("MAIN__(); netsvWrite(); ret=%d ansSize=%d", ret, ansSize);
      }
      delete[] req;
      req = NULL;
    }
    netsvDisconnect(p.net);
  }

  netsvEnd(p.net);
  uM1("MAIN__(); End %s", argv[0]);
  end();
  exit(0);
  return 0;
}

/*! \fn void setParam()
\brief �p�����[�^�[�����擾����
*/
void setParam(){
  if(confSetKey("tkb32port"))
    p.tkb32Port = atoi(confGetVal());
}

/*! \fn void signal_handler(int sig)
\brief Signal�����B�����I�ɂ̓��C�u�����Ɉڍs���ׂ��B
\param[in] sig
*/
void sigFunc(int sig){
  //uM1("sigFunc(); debug sig=%d called", sig);
  switch(sig){
  case SIGINT:
  case SIGTERM:
  //case SIGKILL:
    //exit(0);
    atexitFunc();
    return;
  default:
    return;
  }
  //return;
}

/*! \fn void atexitFunc()
\brief ����I�������ۂɌĂяo�����
*/
void atexitFunc(){
  //uM("atexitFunc(); debug");
  act = 0;
}
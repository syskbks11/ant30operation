/*!
\file srvPow/main.cpp
\date 2008.01.24
\author Y.Koide
\brief �A���g�ϑ��p�v���O�����B
*/
//#ifdef WIN32
//#include <windows.h>
//#include <wincon.h>
//#endif

#include <stdio.h>
#include <stdlib.h>
#include "import/libtkb.h"
#include "srvPow.h"

const char programName[] = "srvPow";
static const char logPath[] = "./log";        //!< ���O��ۊǂ���f�B���N�g���̃p�X

static void callBackAtexit(void);
static void callBackSignal(void);

int active = 0;

int main(int argc, char* argv[]){
  void* srvPow;
  int ret;
  char tmp[256];

  //! �I���A�I�������֐��̓o�^
  ret = atexit(callBackAtexit);
  if(ret){
    return 1;
  }
  ret = uSigFunc(callBackSignal);
  if(ret){
    return 2;
  }

  //! ������
  sprintf(tmp, "%s/%s", logPath, programName);
  ret = uInit(tmp);
  if(ret){
    return 3;
  }
  srvPow = srvPowInit();
  if(!srvPow){
    return 4;
  }

  //! ���[�v����
  active = 1;
  while(active){
    ret = srvPowRepeat(srvPow);
    if(ret){
      break;
    }
  }

  //! �I������
  srvPowEnd(srvPow);
  uEnd();
  exit(0);
  
  return 0;
}

void callBackAtexit(void){
  active = 0;
}

void callBackSignal(void){
  callBackAtexit();
}

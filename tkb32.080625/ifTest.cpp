/*!
\file ifTest.cpp
\author Y.Koide
\date 2006.12.01
\brief Tsukuba 32m IF Controle Program
*/

#include <memory.h>
#include <stdio.h>
#include "../u/u.h"
#include "../conf/conf.h"
#include "../tm/tm.h"
#include "if.h"


typedef struct sParamIfTest{
  double LoInterval;
}tParamIfTest;

static void setParam(tParamIfTest* p);

int MAIN__(void){
  tParamIfTest p;
  void* ptm;
  static char cenvdt[3][128] = {
    "ANTFILE=./etc/ant_err.dat",
    "EPHFILE=./etc/ephfile/19241221-20500102.ORG",
    "TIMEFILE=./etc/time.dat"
  };
  int ret;
  int ONE[1]={1};

  //!����������
  try{
    //!���O�t�@�C���̐ݒ�
    uInit("./log/ifTest");
    uM("ifTest intialization");
    //!�p�����[�^�[�̓Ǎ���
    confInit();
    confAddFile("./etc/tkb32.conf");
    confAddFile("./etc/test.device");
    confAddFile("./etc/test.scan");
    confAddFile("./etc/test.source");
    confPrint();
    memset(&p, 0, sizeof(p));
    setParam(&p);
    //!�^�C�}�[������
    ptm=tmInit();
    //!���ϐ��ݒ�
    for (int i=0; i < 3; i++) {
      if (putenv(cenvdt[i]) != 0) {
	uE1("Cannot add value to environment (%s)\n", cenvdt[i]);
      }
    }
    //!�e�퐧�䃂�W���[��������
    ifInit(ONE);
  }
  catch(...){
    uE("Error in intialization");
  }

  //!���C������
  try{
    uM("ifTest starting");
    while(1){
      if((ret=ifRepeat())<0)
	uM1("main(); ifRepeat(); return %d", ret); 
    }
  }
  catch(...){
    uE("Error in repeat!!");
  }

  //!�I������
  try{
    uM("ifTest end");
    ifEnd();
    tmEnd(ptm);
  }
  catch(...){
    uE("Error in end");
  }
  return 0;
}

void setParam(tParamIfTest* p){
  if(confSetKey("LoInterval"))
    p->LoInterval = atof(confGetVal());
}

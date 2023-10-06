/*!
\file tkb32/tkb32Func.cpp
\author Y.Koide
\date 2007.01.01
\brief Tsukuba 32m Central Program Functions
\brief Auto observation : obsInit() -> obsStart() -> obsAuto() -> obsEnd()
\brief Tracking only : obsInit() -> obsStart() -> obsEnd()
*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <time.h>
#include <math.h>
#include <unistd.h>
#include <pthread.h>

#include "../libtkb/export/libtkb.h"
#include "compileDate.h"
#include "weath.h"
#include "trk.h"
#include "chop.h"
#include "fe.h"
#include "if.h"
#include "be.h"
#include "tkb32Protocol.h"
#include "errno.h"
#include "tkb32Func.h"

//#include"thread.h" // update080529

const double sleepTime = 1000;

static char tkb32ConfName[] = "../etc/tkb32.conf";
static char logPath[] = "../log";
const double PI = M_PI;

static int endFlg = 0; //! 0:NotEnd 1:End

typedef struct sParamTkb32{
  //! �e����̋N���t���O 0:�N������, 1:�N��, 2:�N��(�ʐM���ȊO)
  int useWeath;
  int useTrk;
  int useChop;
  int useFe;
  int useIf1;
  int useIf2;
  //int* BeUse;

  char FileAnt[128];
  char EnvAnt[128];
  char EnvEph[128];
  char EnvTime[128];
  double OnOffTime;
  int ScanFlag;        //!< 1:OnOff 2:5Point 3:Grid 4:Random 5:9Point 6:Rastar 1001:SourceTracking 1002:Lee Tracking
  int SetNumber;
  int RSkyTime;
  double LineTime;     //! ���X�^�[�̃A�v���[�`���Ԃ��l������1Line�̎��� [sec]
  double TimeScan;     //080530
  double TimeApp;      //080530
  int BeNum;           //!< �o�b�N�G���h�̍ő�䐔

  //! �����Ŏg�p����ϐ�
  int init;            //!< �������̏�� 0:�������O 1:�������ς�
  void* vtm;           //!< tm���W���[���p
  pthread_t thrdUpdateId;//!< �X�e�[�^�X�X���b�hID
  int thrdUpdateRun;   //!< �X�e�[�^�X�擾�X���b�h 0:��~�� 1:���s��
  pthread_t thrdObsId; //!< �ϑ��X���b�hID
  int thrdObsRun;      //!< �ϑ��X���b�h 0:��~�� 1:���s��
  int cntOn;           //!< On�_�ϑ��̃J�E���g���s��
  int isStow;          //!< stow�������Ă��邩 0:unStow 1:Stow

  //! �ʐM�p�̃f�[�^�i�[
  int reqRet;          //!< �N���C�A���g���N�G�X�g�̌��� 0:���� other:���s
  tReq* req;           //!< �N���C�A���g����̗v��
  tAns* ans;           //!< �N���C�A���g�ւ̉����f�[�^
  tAnsState stat;      //!< �S�X�e�[�^�X���
}tParamTkb32;

static tParamTkb32 p;

static void* thrdUpdate(void* _p);
static int obsInit();
static int obsEnd();
static int obsStart();
static int obsStop();
static void* obsAuto(void* _p);
static int R();
static int OnOff(int OnOff);
static int update();
static int trackingWait();
static int rOpenWait();
static int rCloseWait();
static int integStart(int OnOffR, const time_t startTime);
static int integWait(const double waitTime, const time_t startTime);
static void setParam();
static int checkParam0();
static int checkParam();

/*! \fn int init();
\brief ������
\brief �v���O�����N�����ɌĂяo���B
\brief �e����v���O�����֐ڑ�����B
*/
int init(){
  double weath[6] = {0};
  char tmp[1024] = {'\0'};
  //int err = 0;
  int ret;

  uM("init(); tkb32 Initialize");
  memset(&p, 0, sizeof(p));

  //! �p�����[�^�擾
  setParam();
  if(checkParam0()){
    uM("init(); checkParam(); error");
    return PARAM_ERR;
  }

  //! trk45Library�ŕK�v�Ȋ��ϐ���ݒ�
  if(putenv(p.EnvAnt) != 0){
    uM1("init(); Cannot add value to environnmet (%s)\n", tmp);
    return 1;
  }
  if(putenv(p.EnvEph) != 0){
    uM1("init(); Cannot add value to environnmet (%s)\n", tmp);
    return 1;
  }
  if(putenv(p.EnvTime) != 0){
    uM1("init(); Cannot add value to environnmet (%s)\n", tmp);
    return 1;
  }

  //! �e���W���[��������
  p.vtm = tmInit();
  if(!p.vtm){
    uM("init(); tmInit(); error!!");
    return 2;
  }
  ret = weathInit();
  if(ret){
    uM1("init(); weathInit(); ret = %d", ret);
    return 2;
  }
  ret = trkInit();
  if(ret){
    uM1("init(); trkInit(); ret = %d", ret);
    return 2;
  }
  ret = chopInit();
  if(ret){
    uM1("init(); chopInit(); ret = %d", ret);
    return 2;
  }
  ret = feInit();
  if(ret){
    uM1("init(); feInit(); ret = %d", ret);
    return 2;
  }
  ret = ifInit();
  if(ret){
    uM1("init(); ifInit(); ret = %d", ret);
    return 2;
  }

  //! �C�ۃ��W���[���N��
  ret = weathStart();
  p.stat.weathState = ret;
  if(ret){
    uM2("init(); weathStart(); return %d(%s)", ret, errnoStr(ret));
    return 3;
  }
  else{
    uM("init(); weathStart();");
    if(p.useWeath == 1){
      int e = 1;
      //! �f�[�^�擾�܂ōő�21�b�҂�
      for(int i=0; i < 21; i++){
	weathGetWeath(weath);
	if(weath[0] != 0 && weath[1] != 0 && weath[2] != 0){
	  char tmp[256] = {'\0'};
	  e = 0;
	  weathGetWind(&weath[3]);
	  sprintf(tmp, "Tamb=%lf Pamb=%lf Pwater=%lf WindD=.0%lf WindAvg=%lf WindMax=%lf",
		  weath[0], weath[1], weath[2], weath[3], weath[4], weath[5]);
	  uM1("%s", tmp);
	  break;
	}
	tmSleepMSec(sleepTime);
      }
      if(e){
	uM("init(); weathGetWeath(); time out");
	p.stat.weathState = 1;
	return 3;
      }
    }
  }

  //! ���j�^�����O�X���b�h�N��
  int state = pthread_create(&p.thrdUpdateId, NULL, thrdUpdate, NULL);
  if(state != 0){
    uM1("init(); pthred_create(thrdUpdate); state=%d error", state);
    return 2;
  }

  uM("init(); Initialize successfully");

  return 0;
}

/*! \fn int end()
\brief �I��
\brief �v���O�����I�����ɌĂ�
\brief �e����v���O�����Ƃ̒ʐM���I������B
*/
int end(){

  endFlg = 1;

  trkEnd();
  if(p.thrdObsRun == 1){
    obsStop();
  }
  if(p.thrdUpdateRun == 1){
    p.thrdUpdateRun = 0;
    pthread_join(p.thrdUpdateId, NULL);
    p.thrdUpdateId = NULL;
  }
  chopEnd();
  feEnd();
  ifEnd();
  weathEnd();
  uEnd();
  if(p.vtm){
    tmEnd(p.vtm);
    p.vtm = NULL;
  }

  return 0;
}

/*! \fn void* thrdUpdate(void* _p)
\brief �X�e�[�^�X�擾�p�X���b�h
\param[in] _p ���g�p
\return NULL
*/
void* thrdUpdate(void* _p){

  p.thrdUpdateRun = 1;
  while(p.thrdUpdateRun){
    update();
    tmSleepMSec(1000);
  }
  return NULL;
}

/*! \fn int obsInit()
\brief �ϑ��ݒ���e���W���[���ɓǂݍ��܂���B
\brief �p�����[�^��conf���W���[���ɓǂݍ��܂��Ă���ĂԂ��ƁB
\brief �ϑ��O�ɕK���Ăяo���B
\retval 0 ����
\retval 0�ȊO �G���[�R�[�h
*/
int obsInit(){
  char log[128];
  char tmp[1024];
  int ret;
  
  //! �ϑ��p�����[�^����������
  p.ScanFlag = 0;
  p.OnOffTime = 0;
  p.SetNumber = 0;
  p.RSkyTime = 0;
  p.LineTime = 0;
  p.TimeScan = 0;//080530
  p.TimeApp = 0;//080530
  p.cntOn = 0;
  p.isStow = 0;
  memset(&p.stat, 0, sizeof(p.stat));

  //! �p�����[�^��ݒ�
  setParam();
  if(checkParam()){
    uM("obsInit(); checkParam(); error");
    confPrint();
    return PARAM_ERR;
  }

  //! ���O�t�@�C�����𐶐��B*.obs��*�������g�p����B
  if(confSetKey("ObsTable")){
    strcpy(log, confGetVal());
    char* dot = strrchr(log, '.');
    if(dot != NULL){
      *dot = '\0';
    }
  }
  else{
    strcpy(log, "tkb32");
  }

  //! �������J�n
  sprintf(tmp, "%s/tkb32-%s", logPath, log);
  uInit(tmp);
  //! LogName���p�����[�^�[�ɒǉ�
  sprintf(tmp, "LogName\t%s", uGetLogName());
  confAddStr(tmp);

  uM1("This program is Compiled at %s", COMPILE_DATE);
  uM("### Initialization");

  //! �p�����[�^�[AntX,Y,Z��ǉ�
  FILE* fp;
  if((fp = fopen(p.FileAnt, "r")) == NULL){
    uM1("obsInit(); Ant file open error %s", p.EnvAnt);
    return 1;
  }
  else{
    while(fgets(tmp, 255, fp) != NULL){
      //printf(" %s\n", tmp);
      if(tmp[0] == ' '){
	double AntX;
	double AntY;
	double AntZ;
	sscanf(tmp, " %lf %lf %lf", &AntX, &AntY, &AntZ);
	sprintf(tmp, "AntX %lf", AntX);
	confAddStr(tmp);
	sprintf(tmp, "AntY %lf", AntY);
	confAddStr(tmp);
	sprintf(tmp, "AntZ %lf", AntZ);
	confAddStr(tmp);
	//uM3("AntPos %lf %lf %lf", AntX, AntY, AntZ);
	break;
      }
    }
    fclose(fp);
  }
  fp = NULL;

  uM("## Parameter list");
  confPrint();

  //! Make schedule file
  char sch1[256];
  char sch2[256];
  sprintf(sch1, "../log/sch1.tmp");
  sprintf(sch2, "../log/obs.sch");
  fp = fopen(sch1, "w");
  if(!fp){
    uM1("obsInit(); file[%s] open error!!", sch1);
    return 1;
  }
  fprintf(fp, "%s", confGetAllKeyVal());
  fclose(fp);

  sprintf(tmp, "./makeSch %s %s", sch1, sch2);
  ret = system(tmp);
  if(ret){
    uM1("obsInit(); system(makeSch); [%d] error!!", ret);
    return 1;
  }

  //! Read schedule file
  confInit();
  confAddFile(sch2);
  uM("obsInit(); SchFile Data");
  confPrint();
  setParam();
  if(checkParam()){
    uM("obsInit(); checkParam(); error");
    return PARAM_ERR;
  }
    
  //! �e���W���[��������
  //update();

  //! trk
  ret = trkInitParam();
  if(ret){
    uM2("obsInit(); trkInitParam(); [%d(%s)] error!!", ret, errnoStr(ret));
    return 2;
  }

  if(p.ScanFlag >= 1 && p.ScanFlag <= 6){
    //! be
    ret = beInit();
    if(ret){
      uM2("obsInit(); beInit(); [%d(%s)] error!!", ret, errnoStr(ret));
      return 2;
    }
  
    //! IF
    ret = ifStart();
    if(ret){
      uM2("obsInit(); ifStart(); [%d(%s)] error!!", ret, errnoStr(ret));
      //err += -1;
    }
    else{
      //! IF��Lo����M���𑗐M
      uM("obsInit(); ifUpdate();");
      const int OnOffR = 2; //!< ���������͌�����̐ݒ��R�p�ɐݒ肵�Ă���
      ifSetAttOnOffR(OnOffR);
      ret = ifUpdate();
      p.stat.if1State = ret;
      p.stat.if2State = ret;
      if(ret){
	uM2("obsInit(); ifUpdate(); [%d(%s)] error!!", ret, errnoStr(ret));
	return 2;
      }
    }
  }
  p.init = 1;
  uM("obsInit(); Success");

  return 0;
}

/*! \fn int obsEnd()
\brief �e���W���[���̏I���������s��
\brief �Ō�ɕK���Ăяo��
\return 0 ����
*/
int obsEnd(){
  uM("obsEnd(); End observation");

  trkStop();
  beReqStop();
  beEnd();

  uEnd();
  if(p.ans){
    delete[] p.ans;
    p.ans = NULL;
  }
  //if(p.BeUse){
  //  delete[] p.BeUse;
  //  p.BeUse = NULL;
  //}
  //tReq* tmpReq = p.req;
  //memset(&p, 0, sizeof(p));
  //p.req = tmpReq;
  p.init = 0;
  return 0;
}

/*! \fn int obsStart()
\brief �ǔ��X�^�[�g
\return 0:Success othre:Error
*/
int obsStart(){
  //double weath[3];
  int ret;

  //! �ǔ����W���[���N��
  ret = trkStart();
  if(ret){
    uM2("obsStart(); trkStart(); [%d(%s)] error!!", ret, errnoStr(ret));
    return ret;
  }
  uM("obsStart(); Success");
  
  return 0;
}

/*! \fn int obsStop()
\brief �ǔ���~
\return 0:success other:fault
*/
int obsStop(){
  if(p.thrdObsRun){
    //beReqStop();
    p.thrdObsRun = 0;
    pthread_join(p.thrdObsId, NULL);
  }
  return 0;
}

/*! \fn void* obsAuto(void* _p)
\brief �����ϑ������s�B�X���b�h�ŌĂяo���B
\return NULL�@����
*/
void* obsAuto(void* _p){
  int ret;

  //! �X�e�[�^�X�擾�X���b�h���I��
  if(p.thrdUpdateRun == 1){
    p.thrdUpdateRun = 0;
    pthread_join(p.thrdUpdateId, NULL);
  }

  uM("### Starting observation.");
  p.thrdObsRun = 1;

  //! RSky���܂������s��Ȃ��ꍇ(ON�_������ǔ�����悤�ȏꍇ)��Chopper��Open���Ă���
  if(p.RSkyTime <= 0 && p.thrdObsRun == 1){
    uM("obsAuto(); Chopper open");
    ret = rOpenWait();
    if(ret){
      uM2("obsAuto(); rOpenWait(); [%d(%s)] error!!", ret, errnoStr(ret));
    }
    else{
      uM("obsAuto(); Chopper open success");
    }
  }

  //! �X�L�����J�n
  if(p.ScanFlag >= 1 && p.ScanFlag <= 6){
    //! OnPoint,5Point,Grid,9Point,Random,Raster
    //! �����ł̏�����R��1Set�Ƃ��Ă���̂�1Set��R����������郉���_���ϑ����ȂǂɓK���Ȃ�
    //! �S��SetPattern�ŃX�P�W���[���ϑ����ׂ��B�������A���X�^�[�Ȃǂ͕ʈ����B
    int rFlg = 0; //!< r���ł���1:���Z�b�g����
    int id = 0;   //!< trkSEtSofNext()�̖߂�l���i�[����B
    for(int i = 0; i < p.SetNumber && p.thrdObsRun; i++){
      while(p.thrdObsRun){
	//! ���̃X�L�������擾����B0:R +:OnPoint -:OffPoint
	//uM("before trkSetSofNext()");
	ret = trkSetSofNext(&id);
	if(ret){
	  //! 1Set����
	  break;
	}

	if(id > 0){
	  //! OnPoint
	  uM1("obsAuto(); Antenna Moving to On-Point ID=%+d", id);
	  tmSleepMSec(1000); //!< trkSetSofNext();�����trkGetTrackStatus();���s���ƁA���O��Tracking���o�͂��Ă��܂�����1.0[sec]�҂�
	}
	else if (id < 0){
	  //! OffPoint
	  uM1("obsAuto(); Antenna Moving to Off-Point ID=%+d", id);
	  tmSleepMSec(1000); //!< trkSetSofNext();�����trkGetTrackStatus();���s���ƁA���O��Tracking���o�͂��Ă��܂�����1.0[sec]�҂�
	}

	//! �O���R�t���O�������Ă���Ώ�������
	if(rFlg){
	  //! R
	  rFlg = 0;
	  if(p.RSkyTime > 0){
	    uM("## R-Sky ID=0");
	    while(R() != 0 && p.thrdObsRun == 1){
	      //! R����������܂ōĎ��s����
	      tmSleepMSec(sleepTime);
	    }
	  }
	}

	//! R�̏ꍇ�͎��̊ϑ��_�Ɍ����Ă��珈������
	if(id == 0){
	  //! R
	  rFlg = 1;
	  continue; //!< 1Set�I��
	}
	else{
	  //! On or Off
	  if(p.OnOffTime > 0 || p.ScanFlag == 6){
	    //! OnOffTime >0 ��������Raster����������s
	    uM1("## OnOff ID=%+d", id);
	    while(OnOff(id) != 0 && p.thrdObsRun == 1){
	      //! OnOff����������܂ōĎ��s����B
	      tmSleepMSec(sleepTime);
	    }
	    //	    uM("finished to wait");
	  }
	}
      }
    }
    if(p.useChop != 0 && p.thrdObsRun == 1){
      uM("## R-Sky");
      R();
    }
  }
  else if(p.ScanFlag == 1001){
    //! Source Tracking
    uM("## Start Source Tracking Mode");
    tmReset(p.vtm);
    while(p.thrdObsRun == 1){
      //! Endress Loop
      for(int i = 0; i < 60 && p.thrdObsRun; i++){
	update();
	//if(p.stat.trkState){
	//  p.thrdObsRun = 0;
	//  break;
	//}
	tmSleepMSec(sleepTime);
      }
      int min = tmGetLag(p.vtm) / 60;
      uM1("# Source Tracking Running (%d minutes)", min);
    }
  }
  else if(p.ScanFlag == 1002){
    //! Lee Tracking
    uM("## Start Lee Tracking");
    tmReset(p.vtm);
    
    while(p.thrdObsRun == 1){
      //! Endress Loop
      double wind[3] = {0};  //!< ������(0:����(�J�[��) �k��360deg(0deg�Ɠ���) ��90dge �E���)�A����(m/s)�A�ő�u�ԕ���(m/s)�̏�
      double wd = 0;         //!< �����̕��p
      double xy[2] = {0};    //!< AZEL�̒l rad
      const int coordAzel = 3;

      xy[1] = 10.0 * PI / 180.0; //!< EL�̒l���Œ�

      for(int i = 0; i < 6 && p.thrdObsRun; i++){
	update();
	weathGetWind(wind);
	wd = wind[0];
	if(wd != 0){
	  wd -= 180.0; //! �������Ƌt������ݒ�
	  if(wd < 0){
	    wd += 360.0;
	  }
	  xy[0] = wd * PI/ 180.0;
	  ret = trkSetXY(coordAzel, xy);
	  if(ret){
	    uM("");
	  }
	}
	tmSleepMSec(10.0 * 1000.0);
      }
      int min = tmGetLag(p.vtm) / 60;
      uM1("# Lee Tracking Running (%d minutes)", min);
    }
  }

  //! �ϑ��I������
  trkStop();
  p.thrdObsRun = 0;
  p.reqRet = 0;  
  uM("### End observation. ");

  //! �X�e�[�^�X�擾�X���b�h�N��
  int state = pthread_create(&p.thrdUpdateId, NULL, thrdUpdate, NULL);
  if(state != 0){
    uM1("obsAuto(); pthred_create(thrdUpdate); state=%d error!!", state);
    return NULL;
  }

  return NULL;
}

/*! \fn int R()
\brief R-sky�ϑ����s��
\retval 0 ����
*/
int R(){
  int OnOffR;
  int ret;
  time_t t;

  OnOffR = 2;

  //! �ό�����̌����l��R�p�ɐݒ�
  ifSetAttOnOffR(OnOffR);
  ifRepeat();

  //! trk�̏��������܂ő҂B�A���e�i�̈ړ����͎�M�@�S�̂��傫���U�����Ă��邽��
  ret = trackingWait();
  if(ret){
    uM2("R(); trackingWait(); [%d(%s)] error!!", ret, errnoStr(ret));
    return ret;
  }

  //! R-Sky Chopper close
  if(p.thrdObsRun){
    ret = rCloseWait();
    if(ret){
      uM2("R(); rCloseWait(); [%d(%s)] error!!", ret, errnoStr(ret));
      return ret;
    }
  }

  //! R-Sky Obs
  if(p.thrdObsRun){
    uM("# R-Sky     Integ start");
    time(&t);
    t += 1;
    ret = integStart(OnOffR, t);
    if(ret){
      //uM4("R(); Backend answer exe=%d err=%d tsys=%lf totPow=%lf",
      //  p.stat.saacqExe, p.stat.saacqErr, p.stat.saacqTsys, p.stat.saacqTotPow);
      uM2("R(); integStart(); [%d(%s)] error!!", ret, errnoStr(ret)); 
      return ret;
    }
  }

  //! �ϕ������҂�
  if(p.thrdObsRun){
    ret = integWait(p.RSkyTime, t);
    //if(p.useSaacq == 1){
      //uM4("R(); Saacq answer exe=%d saerr=%d tsys=%lf totPow=%lf",
      //  p.stat.saacqExe, p.stat.saacqErr, p.stat.saacqTsys, p.stat.saacqTotPow);      
      if(ret){
	uM2("R(); integWait(); [%d(%s)] error!!", ret, errnoStr(ret));
	return ret;
      }
      //if(p.stat.saacqErr != 1){
      //uM2("R(); saerr=%d(%s)",  p.stat.saacqErr, errnoSaacqErr(p.stat.saacqErr));
      //}
    //}
  }
  uM("# R-Sky     Integ end");

  //! R-Sky Chopper Open
  if(p.thrdObsRun){
    ret = rOpenWait();
    if(ret){
      uM2("R(); rOpenWait(); [%d(%s)]", ret, errnoStr(ret));
      //return ret;
    }
  }

  return 0;
}

/*! \fn int OnOff(int OnOff)
\brief On or Off�ϑ����s��
\param[in] OnOff 1:On-Point -1:Off-Point observation
\retval 0 ����
*/
int OnOff(int OnOff){
  //int exe;
  //int saerr;
  //double tsys;
  //double totPow;
  int OnOffR;
  int ret;
  double waitTime = 0;
  time_t t;

  //! R Open check
  if(p.thrdObsRun){
    ret = rOpenWait();
    if(ret){
      uM2("OnOff(); rOpenWait(); [%d(%s)]", ret, errnoStr(ret));
      return ret;
    }
  }

  //! �ό�����̌����l��OnOff�p�ɐݒ�
  ifSetAttOnOffR(1);

  if(OnOff > 0){
    //! OnPoint
    ifRepeat();
    OnOffR = 0;
  }
  else{
    //! OffPoint
    OnOffR = 1;
    //! ���[�J�����g����doppler�␳
    ret = ifUpdate();
    p.stat.if1State = ret;
    p.stat.if2State = ret;
    if(ret && (p.useIf1 || p.useIf2)){
      uM2("OnOff(); ifUpdate(); [%d(%s)] error!!", ret, errnoStr(ret));
      return IF_STATE_ERR;
    }
    else{
      //! ���������܂ő҂�(�ۗ�)
    }
  }

  uM("before trackingWait()");
  //! trk�̏��������܂ő҂�
  ret = trackingWait();
  if(ret){
    uM2("OnOff(); trackingWait(); [%d(%s)] error!!", ret, errnoStr(ret));
    return ret;
  }

  //! OnOff Obs
  if(p.thrdObsRun){
    //! �ϑ��_��\��
    if(OnOff > 0){
      uM("# On-Point  Integ start");
    }
    else{
      uM("# Off-Point Integ start");
    }

    //! ����1���b�ォ��ϑ��ɓ���
    time(&t);
    t += 1;

    if(OnOff > 0){//080530
      if(p.ScanFlag == 6){
	ret = trkSetRastStart(t+1);
	if(ret){
	  uM2("OnOff(); trkSetRastStart(); [%d(%s)] error!!", ret, errnoStr(ret));
	  return ret;
	}
      }
      if(p.ScanFlag == 6){//080530 in IntegStart()�܂�TimeApp�������҂�
	uM1("OnOff(); AppWaiting %lf seconds. ", p.TimeApp);
	sleep(p.TimeApp + 1.0); // 080602 change ���Őϕ��J�n��1.0s �x�点��e�����l�����Ă���
	uM("OnOff(); AppWaiting end. ");
      }
    }
    //! �ϕ��w��
    double timeToWaitIntegStart = 1.0;//080530 in �ϕ��J�n��1.0s �x�点�邽�߂̒l
    //    if(p.ScanFlag == 6){//080530 in
    //  timeToWaitIntegStart += p.TimeApp;
    //}
    double tIntegS = t + timeToWaitIntegStart;
    if(p.ScanFlag == 6 && OnOff > 0){ // 080602 in Raster��OnScan�̏ꍇ
      tIntegS += p.TimeApp; // 080604 in App�����l�������ϕ��J�n������integStart()�ɓn��
    }
    uM("before integStart()");
    ret = integStart(OnOffR, tIntegS);//! Raster��OnScan�ȊO�́A���̂܂܂̐ϕ��J�n������n��
    if(ret){
      uM2("OnOff(); integStart(); [%d(%s)] error!!", ret, errnoStr(ret)); 
      return ret;
    }

    if(OnOff > 0){
      //! On
      if(p.ScanFlag != 6){
	//! Not Raster
	waitTime = p.OnOffTime;
      }
      else{
	//! Raster
	waitTime = p.LineTime;
	//ret = trkSetRastStart(t+1);
	//	if(ret){
	//  uM2("OnOff(); trkSetRastStart(); [%d(%s)] error!!", ret, errnoStr(ret));
	//  return ret;
	//}
      }
    }
    else{
      //! Off
      waitTime = p.OnOffTime;
    }
  }

  //! �ϕ������҂�
  if(p.thrdObsRun){
    uM("before integWait()");
    ret = integWait(waitTime, t);
    //if(p.useSaacq == 1){
      //uM4("OnOff(); Saacq answer exe=%d saerr=%d tsys=%lf totPow=%lf",
      //  p.stat.saacqExe, p.stat.saacqErr, p.stat.saacqTsys, p.stat.saacqTotPow);
      if(ret){
	uM2("OnOff(); integWait(); [%d(%s)] error!!", ret, errnoStr(ret));
	return ret;
      }
      //if(p.stat.saacqErr != 1){
      //uM2("OnOff(); saerr [%d(%s)]",  p.stat.saacqErr, errnoSaacqErr(p.stat.saacqErr));
      //}
      //}
  }  

  //! ���������ꍇ�͌��݂̊ϑ��_��On�_�̏ꍇ�͂��̃J�E���g����\��
  if(p.thrdObsRun){
    if(OnOff > 0){
      // if(p.ScanFlag == 6){//update080528*1
      //while(!g_rastEndFlag)//edited by hagiwara
      //  usleep(100);
      //g_rastEndFlag = RUN_RASTENDF;
      //}//update080528*1
      p.cntOn++;
      uM1("# On-Point  Integ end (On-Count:%d)", p.cntOn);
    }
    else{
      uM("# Off-Point Integ end");
    }
  }
  //uM("debug end onoff check time");
  return 0;
}

/*! \fn int update()
\brief �e���W���[���̏�Ԃ��`�F�b�N����
\brief ���W���[���ɂ���Ă̓f�o�C�X�̏����X�V����
\brief �ȑO�̃X�e�[�^�X�ƈقȂ�ꍇ�̂ݕ\������B
\retval 0 ���� other:fault
*/
int update(){
  int ret;
  double weath[3];
  int err = 0;

  //uM("update() start");
  if(endFlg == 1){
    return 0;
  }

  //! weath
  ret = weathRepeat();
  if(ret != 0 && ret != p.stat.weathState){
    uM2("update(); weathRepeat(); [%d(%s)] error!!", ret, errnoStr(ret));
    err = 1;
  }
  else if(ret != p.stat.weathState){
    uM2("update(); weathRepeat(); [%d(%s)] recovery", ret, errnoStr(ret));
  }
  p.stat.weathState = ret;
  if(ret == 0){
    weathGetWeath(weath);
    trkSetWeath(weath);
  }
  
  //! trk
  ret = trkRepeat();
  if(ret != 0 && ret != p.stat.trkState){
    uM2("update(); trkRepeat(); [%d(%s)] error!!", ret, errnoStr(ret));
    err = 1;
  }
  else if(ret != p.stat.trkState){
    uM2("update(); trkRepeat(); [%d(%s)] recovery", ret, errnoStr(ret));
  }
  p.stat.trkState = ret;
  if(ret == 0){
    const unsigned char STOW = 0x32;
    trkGetACUStatus(p.stat.trkACUStatus);
    if((p.stat.trkACUStatus[2] & STOW) == STOW){
      p.isStow = 1;
    }
    else{
      p.isStow = 0;
    }
  }

  //! chop
  /*chopSetReq(1); //!< 1:status
  ret = chopRepeat();
  if(ret != 0 && ret != p.stat.chopState){
    uM2("update(); chopRepeat(); [%d(%s)] error!!", ret, errnoStr(ret));
    err = 1;
  }
  else if(ret != p.stat.chopState){
    uM2("update(); chopRepeat(); [%d(%s)] recovery", ret, errnoStr(ret));
  }  
  p.stat.chopState = ret;*/

  //! fe
  ret = feRepeat();
  if(ret != 0 && ret != p.stat.feState){
    uM2("update(); feRepeat(); [%d(%s)] error!!", ret, errnoStr(ret));
    err = 1;
  }
  else if(ret != p.stat.feState){
    uM2("update(); feRepeat(); [%d(%s)] recovery", ret, errnoStr(ret));
  }
  p.stat.feState = ret;

  //! if
  /*ret = ifRepeat();
  if(ret != 0 && ret != p.stat.if1State){
    uM2("update(); ifRepeat(); [%d(%s)] error!!", ret, errnoStr(ret));
    err = 1;
  }
  else if(ret != p.stat.if1State){
    uM2("update(); ifRepeat(); [%d(%s)] recovery", ret, errnoStr(ret));  
  }
  p.stat.if1State = ret;
  p.stat.if2State = ret;*/

  //! saacq
  /*ret = saacqReqState();
  if(ret != 0 && ret != p.stat.saacqState){
    uM2("update(); saacqReqState(); [%d(%s)] error!!", ret, errnoStr(ret));
    err = 1;
  }
  else if(ret != p.stat.saacqState){
    uM2("update(); saacqReqState(); [%d(%s)] recovery", ret, errnoStr(ret));
  }
  p.stat.saacqState = ret;*/

  return err;
}

/*! \fn int trackingWait()
\brief Tracking��ԂɂȂ�܂ő҂�
\return 0:Success othre:Error
*/
int trackingWait(){
  //uM("trackingWait() start");
  //! trk�̏��������܂ő҂B�A���e�i�̈ړ����͎�M�@�S�̂��傫���U�����Ă��邽��
  if(p.useTrk == 1){
    int stat=1;
    //!1����(�A���e�i1/4Az����)�܂ő҂�
    for(int i=0; i<30 && p.thrdObsRun; i++){
      update();
      if(p.stat.trkState == 0){
	trkGetTrackStatus(&stat);
	if(stat==0){
	  break;
	}
      }
      tmSleepMSec(sleepTime);
    }
    //! Error
    if(p.stat.trkState){
      return p.stat.trkState;
    }
    else if(stat){
      return TRK_TRACKING_TIMEOUT_ERR;
    }
  }
  else{
    update();
  }
  //uM("trackingWait() end");

  return 0;
}

/*! \fn int rOpenWait()
\brief R��Open�w�߂��o���AOpen����܂őҋ@����B
\retunr 0:Success othre:Error
*/
int rOpenWait(){
  int err;
  int ret;

  //! ���݂�R�̏�Ԃ��擾��Open�Ȃ�I������B
  if(p.useChop == 1 && p.thrdObsRun){
    update();
    chopSetReq(1);  //!< 1:status
    ret = chopRepeat();
    p.stat.chopState = ret;
    if(ret == 0 && chopGetState() == 2 && chopGetMoter() == 4){ //!< 2:open, 4:sotp���
      return 0;
    }
  }
  else{
    return 0;
  }

  //! Open�w�߂��o��
  uM("rOpenWait(); Chopper open");
  if(p.useChop == 1 && p.thrdObsRun){
    chopSetReq(3);     //!< 3:open
    ret = chopRepeat();
    p.stat.chopState = ret;
    if(ret){
      return CHOP_STATE_ERR;
    }
  }

  //! ���������܂ő҂�(�^�C�����~�b�g10�b)
  if(p.useChop == 1 && p.thrdObsRun){
    err = 1;
    for(int i = 0; i < 10 && p.thrdObsRun; i++){
      update();
      chopSetReq(1);  //!< 1:status
      ret = chopRepeat();
      p.stat.chopState = ret;
      if(ret){
	return CHOP_STATE_ERR;
      }
      else{
	if(chopGetState() == 2 && chopGetMoter() == 4){ //!< 2:open, 4:sotp���
	  err=0;
	  break;
	}
      }
      tmSleepMSec(sleepTime);
    }
    if(err){
      return CHOP_MOVING_TIMEOUT_ERR;
    }
  }
  uM("rOpenWait(); Chopper open success");
  return 0;
}

/*! \fn int rCloseWait()
\brief R��Close�w�߂��o���AClose����܂őҋ@����B
\retunr 0:Success othre:Error
*/
int rCloseWait(){
  int err;
  int ret;

  //! ���݂�R�̏�Ԃ��擾��Close�Ȃ�I������B
  if(p.useChop == 1 && p.thrdObsRun){
    update();
    chopSetReq(1);  //!< 1:status
    ret = chopRepeat();
    p.stat.chopState = ret;
    if(ret == 0 && chopGetState() == 1 && chopGetMoter() == 4){ //!< 1:close, 4:sotp���
      return 0;
    }
  }

  //! Close�w�߂��o��
  uM("rCloseWait(); Chopper close");
  if(p.useChop == 1 && p.thrdObsRun){
    chopSetReq(2);        //!< 2:close
    ret = chopRepeat();
    p.stat.chopState = ret;
    if(ret){
      return CHOP_STATE_ERR;
    }
  }

  //! ���������܂ő҂�(�^�C�����~�b�g10�b)
  if(p.useChop == 1 && p.thrdObsRun){
    err=1;
    for(int i=0; i<10 && p.thrdObsRun; i++){
      chopSetReq(1);    //!< 1:status
      ret = chopRepeat();
      p.stat.chopState = ret;
      if(ret){
	return CHOP_STATE_ERR;
      }
      else{
	if(chopGetState() == 1 && chopGetMoter() == 4){ //!< 1:close, 4:sotp���
	  err = 0;
	  break;
	}
      }
      tmSleepMSec(sleepTime);
    }
    if(err){
      return CHOP_MOVING_TIMEOUT_ERR;
    }
  }
  uM("rCloseWait(); Chopper close success");
  return 0;
}

/*! \fn int integStart(int OnOffR)
\brief �����v�ϕ��J�n
\param[in] OnOffR 0:On-Point 1:Off-Point 2:R-Sky
\param[in] startTime �ϕ����J�n���鎞��time(&t);�Ŏ擾���鎞���\�L
\retval 0 ����
\retval 0�ȊO �G���[�R�[�h
*/
int integStart(int OnOffR, const time_t startTime){
  double RADEC[2];
  double LB[2];
  double AZEL[2];
  double DRADEC[2];
  double DLB[2];
  double DAZEL[2];
  double PAZEL[2];
  double RAZEL[2];
  double PA = 0;
  double FQIF1;
  double VRAD;
  double IFATT;
  double weath[3];
  double wind[3];
  double TAU = 0;        //!< ��C�̌���
  double BATM = 0;       //!< ��C�̉��x [K]
  double IPINT;
  int exe;
  int saerr;
  double tsys;
  double totPow;
  int ret;
  time_t t;
  struct tm* stm;
  char timeStr[16];

  //! �g�p���Ă��Ȃ��ꍇ�͏I��
  //if(p.useSaacq != 1){
  //  return SAACQ_NOT_USE;
  //}

  //! ���݂̃X�e�[�^�X���m�F����
  ret = beReqState();
  p.stat.saacqState = ret;
  if(ret){
    uM("Cannot get back end status!");
    return ret;
  }
  for(int i = 0; i < p.BeNum; i++){
    beGetAns(i, &exe, &saerr, &tsys, &totPow);
    p.stat.saacqTsys = tsys;
    p.stat.saacqTotPow = totPow;
    p.stat.saacqExe = exe;
    p.stat.saacqErr = saerr;
    if(exe != 0 && exe != 1 && exe != 2){
      //!< -1:Failed 1:Success 2:Standby 3:Execution 4:Accept 5:Reject
      uM2("integStart(); Be%02d exeFlag(%d) error!! ", i + 1, exe);
      p.stat.saacqState = SAACQ_STATE_ERR;
      return p.stat.saacqState;
    }
  }

  //! �ϕ��J�n�w��
  t = startTime;
  stm = localtime(&t);
  sprintf(timeStr, "%02d:%02d:%02d", stm->tm_hour, stm->tm_min, stm->tm_sec);
  uM1("integStart(); startTime=%s", timeStr);

  trkGetRADEC(RADEC);
  trkGetLB(LB);
  trkGetAZEL(AZEL);
  trkGetDRADEC(DRADEC);
  trkGetDLB(DLB);
  trkGetDAZEL(DAZEL);
  trkGetPAZEL(PAZEL);
  trkGetRAZEL(RAZEL);
  ifGetLo1PFreq(&FQIF1);
  ifGetVRAD(&VRAD);
  ifGetLo1PAtt(&IFATT);
  weathGetWeath(weath);
  weathGetWind(wind);
  IPINT = chopGetTemp2();
  ret = beReqStart(OnOffR, 0, startTime,
		   RADEC, LB, AZEL,
		   DRADEC, DLB, DAZEL,
		   PAZEL, RAZEL,
		   PA, FQIF1, VRAD, IFATT,
		   weath, wind, TAU,
		   BATM, IPINT);

  p.stat.saacqState = ret;
  return ret;
}

/*! \fn int integWait()
\brief �����v�̃f�[�^���擾����܂őҋ@����B
\return 0:Success other:Error
*/
int integWait(const double waitTime, time_t startTime){
  int exe;
  int saerr;
  double tsys;
  double totPow;
  int ret;

  uM("integWait() start");
  //! �ϕ��J�n�܂ő҂�
  while(tmGetDiff(startTime, 0) > 0.001){
    tmSleepMSec(1);
  }

  uM("before while #2");
  //! �ϕ����ԑ҂�
  tmReset(p.vtm);
  while(tmGetLag(p.vtm) < waitTime && p.thrdObsRun){
    update();
    tmSleepMSec(sleepTime);
  }

  uM("after while #2");
  //! �ϕ����Ԃ��߂����番���v�̌��ʂ��`�F�b�N���ɍs��
  if(p.thrdObsRun){
    ret = beReqState();
    uM("beReqState() returned");
    p.stat.saacqState = ret;
    if(ret){
      return ret;
    }
    //beGetAns(&exe, &saerr, &tsys, &totPow);
    //p.stat.saacqTsys = tsys;
    //p.stat.saacqTotPow = totPow;
    //p.stat.saacqExe = exe;
    //p.stat.saacqErr = saerr;
    //if(exe != 3){
      //!< -1:Failed 1:Success 2:Standby 3:Execution 4:Accept 5:Reject
      //if(exe != 1){
    //p.stat.saacqState = BE_STATE_ERR;
    //return p.stat.saacqState;
    //}
    // break;
    //}
    update();
    uM("update() returned");
    //tmSleepMSec(sleepTime);
  }
  return 0;
}

/*! \fn int setReqData(unsigned char* req, int reqSize)
\brief ��������N���C�A���g(tkb32cl,tkb32clGUI)���痈�����N�G�X�g���Z�b�g���ď�������B
\brief �ʐM�v���g�R����tkb32Protocol.h�ɂĒ�`����B
\param[in] req ���N�G�X�g�f�[�^
\param[in] reqSize ���N�G�X�g�f�[�^�̃T�C�Y[Byte]
\retval 0 ����
\retval -1 ���s
*/
int setReqData(unsigned char* req, int reqSize){
  int ret;

  if(((tReq*)req)->size != reqSize){
    uM2("setReqData(); size=%d not reqSize=%d", ((tReq*)req)->size, reqSize);
    return -1;
  }
  p.req = (tReq*)req;

  //! �v���g�R���o�[�W�����`�F�b�N
  if(p.req->reqVer != TKB32_REQ_VER){
    uM2("setReqData(); reqVer(%d) != TKB32_REQ_VER(%d)", p.req->reqVer, TKB32_REQ_VER);
    p.reqRet = 1;
    return 0;
  }

  //uM1("setReqData(); reqFlg=%d", p.req->reqFlg);
  if(p.req->reqFlg == 0){
    //! Status
  }
  else if(p.req->reqFlg == 1){
    //! Initialize
    uM("setReqData(); Request Initialize");
    if(p.thrdObsRun){
      uM("Reject Alreqdy Startobs");
      p.reqRet = 1;
    }
    else{
      if(p.init){
	uM("End current obs");
	obsEnd();
	tmSleepMSec(1000); //!< �����v�̃l�b�g���[�N�\�P�b�g�̍ċN�����I���܂�1sec�҂B
      }
      tReqInit* ini = (tReqInit*)(&(p.req->data));
      (&ini->param)[(ini->paramSize)-1] = '\0';
      //uM2("setReqData(); reqSize=%d paramSize=%d", reqSize, ini->paramSize);
      if((&ini->param)[ini->paramSize - 1] != '\0'){
	uM("setReqData(); invalid request data");
	p.reqRet = 1;
	return -1;
      }
      confInit();
      confAddFile(tkb32ConfName);
      confAddStr((const char*)&ini->param);
      ret = obsInit();
      if(ret){
	uM("Error in obsInit();");
	p.reqRet = 1;
	obsEnd();
	return -1;
      }
      p.reqRet = 0;
    }
  }
  else if(p.req->reqFlg == 2){
    //! RemoteControle
    p.reqRet = 0;
    uM("setReqData(); Request remote controle");
    tReqRemote* rem = (tReqRemote*)&p.req->data;
    //! debug
    //uM1("trkReq = %u", rem->trkReq); 
    //uM1("chopReq = %u", rem->chopReq);
    //uM1("saacqReq = %u", rem->saacqReq);
    //uM1("sasoftReq = %u", rem->sasoftReq);
    if(rem->trkReq & 1){
      //! AzEl
    }
    if(rem->trkReq & 2){
      //! Pointing Offset
      //trkSetPof(rem->trkPof, rem->trkPofCoord);
      //! ��肠����AZEL�w��ŁB
      trkSetPof(rem->trkPof, 3);
    }
    if(rem->trkReq & 4){
      //! Trcking Stop
      uM("Request Trcking Stop");
      trkStop();
    }
    if(rem->trkReq & 8){
      //! Stow unlock
      uM("Request Stow unlock");
      p.reqRet = trkSetAcuUnStow();
    }
    if(rem->trkReq & 16){
      //! Stow lock
      uM("Request Stow lock");
      if(p.thrdObsRun){
	uM("Reject  Already startobs");
	p.reqRet = 1;
      }
      else{
	ret = trkSetAcuMood(3);
	if(ret){
	  p.reqRet = 1;
	}
      }
    }
    if(rem->trkReq & 32){
      //! Progrum tracking mood
      uM("Request program tracking mood");
      ret = trkSetAcuMood(2);
      if(ret){
	p.reqRet = 1;
      }
    }
    if(rem->trkReq & 64){
      //! ACU Standby
      uM("Request ACU Standby");
      ret = trkSetAcuMood(0);
      if(ret){
	p.reqRet = 1;
      }
    }
    if(rem->trkReq & 128){
      //! Drive unlock
      uM("Request Drive unlock");
      ret = trkSetAcuBan(0);
      if(ret){
	p.reqRet = 1;
      }
    }
    if(rem->trkReq & 256){
      //! Drive lock
      uM("Request Drive lock");
      //if(p.thrdObsRun){
      //uM("Reject  Already startobs");
      //p.reqRet = 1;
      //}
      //else{
      ret = trkSetAcuBan(1);
      if(ret){
	p.reqRet = 1;
      }
      //}
    }
    //if(rem->trkReq & 512){
      //!Tracking Start
      //uM("Request Tracking Start");
    //}
    if(rem->chopReq == 1){
      //! close
      uM("Requset Chopper close");
      chopSetReq(2);       //!< 2:close
      ret = chopRepeat();
      p.stat.chopState = ret;
      if(ret){
	uM2("setReqData(); chopRepeat(); [%d(%s)] error!!", ret, errnoStr(ret));
	p.reqRet = ret;
      }
    }
    else if(rem->chopReq == 2){
      //! open
      uM("Requset Chopper open");
      chopSetReq(3);       //!< 3:open
      ret = chopRepeat();
      p.stat.chopState = ret;
      if(ret){
	uM2("setReqData(); chopRepeat(); [%d(%s)] error!!", ret, errnoStr(ret));
	p.reqRet = ret;
      }
    }
    else if(rem->chopReq == 3){
      //! origine
      uM("Request Chopper origine");
      chopSetReq(4);       //!< 4:origine
      ret = chopRepeat();
      p.stat.chopState = ret;
      if(ret){
	uM2("setReqData(); chopRepeat(); [%d(%s)]", ret, errnoStr(ret));
	p.reqRet = ret;	
      }
    }
    else if(rem->chopReq == 4){
      //! stop
      uM("Request Chopper stop");
      chopSetReq(5);       //!< 5:stop
      ret = chopRepeat();
      p.stat.chopState = ret;
      if(ret){
	uM2("setReqData(); chopRepeat(); [%d(%s)]", ret, errnoStr(ret));
	p.reqRet = ret;
      }
    }
    if(rem->if1Req & 1){
      //! SetFreq
      ifSetLo1Freq(rem->if1Freq);
    }
    if(rem->if1Req & 2){
      //! SetAmplitude
      ifSetLo1Amp(rem->if1Amp);
    }
    if(rem->if1Req & 4){
      //! SetAtt
      ifSetLo1Att(rem->if1Att);
    }
    if(rem->if2Req & 1){
      //! SetFreq
      ifSetLo2Freq(rem->if2Freq);
    }
    if(rem->if2Req & 2){
      //! SetAmp
      ifSetLo2Amp(rem->if2Amp);
    }
    if(rem->if1Req || rem->if2Req){
      //! Update if
      ifRepeat();
    }
    if(rem->saacqReq & 1){
      //! Backend Set Integ Time
      if(p.init == 0){
	uM("setReqData(); Not initialized");
	p.reqRet = 1;
	return -1;
      }
      //uM("Request Set SaAcq Integ");
    }
    if(rem->saacqReq & 2){
      //! Start integration
      uM1("Request start integration OnOffR=%d", rem->saacqOnOffR);
      if(p.init == 0){
	uM("setReqData(); Not initialized");
	p.reqRet = 1;
	return -1;
      }
      if(p.thrdObsRun){
	uM("Reject  Already startobs");
	p.reqRet = 1;
      }
      else{
	ret = beReqState();
	if(ret){
	  uM2("setReqData(); sacqReqState(); [%d(%s)] error!!", ret, errnoStr(ret));
	  p.reqRet = 1;
	}
	else{
	  int exe;
	  int saerr;
	  double tsys;
	  double totPow;
	  time_t t;
	  //beGetAns(&exe, &saerr, &tsys, &totPow);
	  //if(exe == 3){
	  //  uM("Request Integ start backend is in execution");
	  //  p.reqRet = 1;
	  //}
	  //else{
	    int OnOffR;
	    //! �����v�ւ̎w������
	    OnOffR = rem->saacqOnOffR;
	    time(&t);
	    t += 1;
	    ret = integStart(OnOffR, t);
	    if(ret){
	      uM2("setReqData(); integStart(); [%d(%s)] error!!", ret, errnoStr(ret));
	      p.reqRet = ret;	  
	    }
	  //}
	}//!< if(beReqState())
      }//!< if(p.thrdObsRun)
    }
    if(rem->saacqReq & 4){
      //! Stop backend
      if(p.init == 0){
	uM("setReqData(); Not initialized");
	p.reqRet = 1;
	return -1;
      }
      uM("Request Stop Backend");
      p.reqRet = beReqStop();
    }
    if(rem->sasoftReq & 1){
      //! Dummy
      if(p.init == 0){
	uM("setReqData(); Not initialized");
	p.reqRet = 1;
	return -1;
      }
    }
  }
  else if(p.req->reqFlg == 3){
    //! Strat Obs
    p.reqRet = 1;
    uM("setReqData(); Start observation");
    if(p.init == 0){
      uM("Reject Not initialized");
    }
    else if(p.thrdObsRun){
      uM("Reject Alreqdy Startobs");
    }
    else{
      ret = obsStart();
      if(ret){
	uM1("setReqData(); obsStart(); ret = %d", ret);
	obsStop();
	return ret;
      }
      int state = pthread_create(&p.thrdObsId, NULL, obsAuto, NULL);
      if(state != 0){
	uM1("setReqData(); pthred_create(obsAuto); state=%d error", state);
	return -2;
      }
      p.reqRet = 0;
    }
  }
  else if(p.req->reqFlg == 4){
    //! Stop observation
    p.reqRet = 1;
    uM("setReqData(); Request stop observation");
    if(p.init == 0){
      uM("Reject  Not initialized");
    }
    else if(p.thrdObsRun == 0){
      uM("Reject  Not Startobs");
    }
    else{
      obsStop();
      p.reqRet = 0;
    }
  }
  else if(p.req->reqFlg == 5){
    //! End Obs
    p.reqRet = 1;
    uM("setReqData(); Request end observation");
    if(p.init == 0){
      uM("Reject  Not Initialized");
    }
    else{
      if(p.thrdObsRun){
	p.reqRet = obsStop();
      }
      obsEnd();
      p.reqRet = 0;
    }
  }
  else if(p.req->reqFlg == 6){
    //! Paramter List
  }

  //uM1("debug p.req->reqFlg=%d", p.req->reqFlg);
  return 0;
}

/*! \fn int getAnsData(unsigned char** ans, int *ansSize)
\brief �����f�[�^���擾����
\brief �ʐM�v���g�R����tkb32Protocol.h�ɂĒ�`����B
\param[out] ans �����f�[�^
\param[out] ansSize �����f�[�^�̃T�C�Y[Byte]
\retval 0 ����
*/
int getAnsData(unsigned char** ans, int *ansSize){
  const size_t headSize = sizeof(p.ans->size) + sizeof(p.ans->ansVer) + sizeof(p.ans->ansFlg);

  //uM("getAnsData();");
  if(p.ans){
    delete[] p.ans;
    p.ans = NULL;
  }
  long size;
  time_t t;

  if(p.req->reqFlg==0){
    //! Status
    time(&t);
    p.stat.UT = t;
    trkGetLST(&p.stat.LST);
    weathGetWeath(&p.stat.weathData[0]);
    weathGetWind(&p.stat.weathData[3]);
    p.stat.trkScanCnt = p.cntOn;
    trkGetXY(&p.stat.trkXYCoord, p.stat.trkXY);
    trkGetSof(&p.stat.trkSofCoord, p.stat.trkSof);
    trkGetPof(&p.stat.trkPofCoord, p.stat.trkPof);
    trkGetPZONE(&p.stat.trkPZONE);
    trkGetRZONE(&p.stat.trkRZONE);
    trkGetPAZEL(p.stat.trkP3AZEL);
    //printf("PAZEL %lf %lf\n", p.stat.trkPAZEL[0], p.stat.trkPAZEL[1]);
    trkGetRAZEL(p.stat.trkRAZEL);
    trkGetEAZEL(p.stat.trkEAZEL);
    trkGetTrackStatus(&p.stat.trkTracking);
    trkGetACUStatus(p.stat.trkACUStatus);
    //chopSetReq(1);
    //chopRepeat();
    p.stat.chopStat = chopGetState();
    p.stat.chopMoter = chopGetMoter();
    p.stat.chopTemp1 = chopGetTemp1();
    p.stat.chopTemp2 = chopGetTemp2();
    p.stat.feK = feGetK();
    p.stat.fePa = feGetPa();
    ifGetVRAD(&p.stat.ifVrad);
    ifGetLo1PFreq(&p.stat.if1PFreq);
    ifGetLo1PAmp(&p.stat.if1PAmp);
    ifGetLo1PAtt(&p.stat.if1PAtt);
    ifGetLo1RFreq(&p.stat.if1RFreq);
    ifGetLo1RAmp(&p.stat.if1RAmp);
    ifGetLo1RAtt(&p.stat.if1RAtt);
    ifGetLo2PFreq(&p.stat.if2PFreq);
    ifGetLo2PAmp(&p.stat.if2PAmp);
    ifGetLo2RFreq(&p.stat.if2RFreq);
    ifGetLo2RAmp(&p.stat.if2RAmp);
    //if(p.thrdObsRun  == 0 && p.useSaacq){
    //if(p.thrdObsRun == 0){
    //  p.stat.saacqState = beReqState();
    //  if(p.stat.saacqState){
    //beGetAns(0, &p.stat.saacqExe, &p.stat.saacqErr, &p.stat.saacqTsys, &p.stat.saacqTotPow);
    //  }
    //}
    //uM("getAnsData(); get data");
    //uM1("getAnsData(); p.req->reqFlg=%d", p.req->reqFlg)
    size = headSize + sizeof(p.stat);
    p.ans = (tAns*)new unsigned char[size];
    p.ans->size = size;
    p.ans->ansFlg = 0;
    memcpy(&(p.ans->data), &p.stat, sizeof(p.stat));
  }
  else if(p.req->reqFlg==1){
    //! Initialize
    size = headSize + sizeof(p.ans->data);
    p.ans = (tAns*)new unsigned char[size];
    p.ans->size = size;
    p.ans->ansFlg = p.req->reqFlg;
    p.ans->data = p.reqRet;
  }
  else if(p.req->reqFlg==2){
    //! RemoteControle
    size = headSize + sizeof(p.ans->data);
    p.ans = (tAns*)new unsigned char[size];
    p.ans->size = size;
    p.ans->ansFlg = p.req->reqFlg;
    p.ans->data = p.reqRet;
  }
  else if(p.req->reqFlg==3){
    //! Strat Obs
    size = headSize + sizeof(p.ans->data);
    p.ans = (tAns*)new unsigned char[size];
    p.ans->size = size;
    p.ans->ansFlg = p.req->reqFlg;
    p.ans->data = p.reqRet;
  }
  else if(p.req->reqFlg==4){
    //! Stop Obs
    size = headSize + sizeof(p.ans->data);
    p.ans = (tAns*)new unsigned char[size];
    p.ans->size = size;
    p.ans->ansFlg = p.req->reqFlg;
    p.ans->data = p.reqRet;
  }
  else if(p.req->reqFlg==5){
    //! End
    size = headSize + sizeof(p.ans->data);
    p.ans = (tAns*)new unsigned char[size];
    p.ans->size = size;
    p.ans->ansFlg = p.req->reqFlg;
    p.ans->data = p.reqRet;
  }
  p.ans->ansVer = TKB32_ANS_VER;
  *ans = (unsigned char*)p.ans;
  *ansSize = size;
  return 0;
}

/*! \fn void setParam()
\brief �p�����[�^�[�����擾����
*/
void setParam(){
  //char tmp[256] = {'\0'};

  if(confSetKey("WeathUse"))
    p.useWeath = atoi(confGetVal());
  if(confSetKey("TrkUse"))
    p.useTrk = atoi(confGetVal());
  if(confSetKey("ChopUse"))
    p.useChop = atoi(confGetVal());
  if(confSetKey("FeUse"))
    p.useFe = atoi(confGetVal());
  if(confSetKey("IfUse01"))
    p.useIf1 = atoi(confGetVal());
  if(confSetKey("IfUse02"))
    p.useIf2 = atoi(confGetVal());
  if(confSetKey("BeNum"))
    p.BeNum = atoi(confGetVal());
  
  if(confSetKey("EnvAnt")){
    strcpy(p.FileAnt, confGetVal());
    sprintf(p.EnvAnt, "ANTFILE=%s", confGetVal());
  }
  if(confSetKey("EnvEph"))
    sprintf(p.EnvEph, "EPHFILE=%s", confGetVal());
  if(confSetKey("EnvTime"))
    sprintf(p.EnvTime, "TIMEFILE=%s", confGetVal());


  if(confSetKey("ScanFlag"))
    p.ScanFlag = atoi(confGetVal());
  if(confSetKey("OnOffTime"))
    p.OnOffTime = atof(confGetVal());
  if(confSetKey("SeqNumber"))          //!< �݊����̂��ߓ��ʐݒ肵�Ă���
    p.SetNumber = atoi(confGetVal());
  if(confSetKey("SetNumber"))
    p.SetNumber = atoi(confGetVal());
  if(confSetKey("RSkyTime"))
    p.RSkyTime = atoi(confGetVal());
  if(confSetKey("LineTime"))
    p.LineTime = atof(confGetVal());
  if(confSetKey("TimeScan"))//080530 in
    p.TimeScan = atof(confGetVal());
  if(confSetKey("TimeApp"))//080530 in
    p.TimeApp = atof(confGetVal());
}

/*! \fn int checkParam0()
\brief init()�ł̃p�����[�^�[�̃`�F�b�N
\retval 0 ����
\retval -1 ���s
*/
int checkParam0(){
  if(p.useWeath < 0 || p.useWeath > 2){
    uM1("checkParam(); useWeath[%d] error!!", p.useWeath);
    return -1;
  }
  if(p.useTrk < 0 || p.useTrk > 2){
    uM1("checkParam(); useTrk[%d] error!!", p.useTrk);
    return -1;
  }
  if(p.useChop < 0 || p.useChop > 2){
    uM1("checkParam(); useChop[%d] error!!", p.useChop);
    return -1;
  }
  if(p.useFe < 0 || p.useFe > 2){
    uM1("checkParam(); useFe[%d] error!!", p.useFe);
    return -1;
  }
  if(p.useIf1 < 0 || p.useIf1 > 2){
    uM1("checkParam(); useIf1[%d] error!!", p.useIf1);
    return -1;
  }
  if(p.useIf2 < 0 || p.useIf2 > 2){
    uM1("checkParam(); useIf2[%d] error!!", p.useIf2);
    return -1;
  }

  if(!confSetKey("EnvAnt")){
    uM("checkParam(); EnvAnt error!!");
    return -1;
  }
  if(!confSetKey("EnvEph")){
    uM("checkParam(); EnvEph error!!");
    return -1;
  }
  if(!confSetKey("EnvTime")){
    uM("checkParam(); EnvTime error!!");
    return -1;
  }

  return 0;
}

/*! \fn int checkParam()
\brief obsInit()�ł̃p�����[�^�[�̃`�F�b�N
\retval 0 ����
\retval -1 ���s
*/
int checkParam(){
  if(p.useWeath < 0 || p.useWeath > 2){
    uM1("checkParam(); useWeath[%d] error!!", p.useWeath);
    return -1;
  }
  if(p.useTrk < 0 || p.useTrk > 2){
    uM1("checkParam(); useTrk[%d] error!!", p.useTrk);
    return -1;
  }
  if(p.useChop < 0 || p.useChop > 2){
    uM1("checkParam(); useChop[%d] error!!", p.useChop);
    return -1;
  }
  if(p.useFe < 0 || p.useFe > 2){
    uM1("checkParam(); useFe[%d] error!!", p.useFe);
    return -1;
  }
  if(p.useIf1 < 0 || p.useIf1 > 2){
    uM1("checkParam(); useIf1[%d] error!!", p.useIf1);
    return -1;
  }
  if(p.useIf2 < 0 || p.useIf2 > 2){
    uM1("checkParam(); useIf2[%d] error!!", p.useIf2);
    return -1;
  }

  if(p.BeNum < 0){
    uM1("checkParam(); BeNum[%d] < 0 error!!", p.BeNum);
    return -1;
  }

  if(!confSetKey("EnvAnt")){
    uM("checkParam(); EnvAnt error!!");
    return -1;
  }
  if(!confSetKey("EnvEph")){
    uM("checkParam(); EnvEph error!!");
    return -1;
  }
  if(!confSetKey("EnvTime")){
    uM("checkParam(); EnvTime error!!");
    return -1;
  }

  if(!((p.ScanFlag >= 1 && p.ScanFlag <= 6)
       || (p.ScanFlag >= 1001 && p.ScanFlag <= 1002))){
    uM1("ScanFlag(%d) error!!", p.ScanFlag);
      return -1;
  }
  if(p.OnOffTime < 0){
    uM1("checkParam(); OnOffTime[%d] error!!", p.OnOffTime);
    return -1;
  }
  if(p.SetNumber < 0){
    uM1("checkParam(); SetNumber[%d] error!!", p.SetNumber);
    return -1;
  }
  if(p.RSkyTime < 0){
    uM1("checkParam(); RSkyTime[%d] error!!", p.RSkyTime);
    return -1;
  }
  if(p.LineTime < 0){
    uM1("checkParam(); LineTime[%lf] error!!", p.LineTime);
    return -1;
  }
  if(p.TimeScan < 0){//080530 in
    uM1("checkParam(); TimeScan[%lf] error!!", p.TimeScan);
    return -1;
  }
  if(p.TimeApp < 0){//080530 in
    uM1("checkParam(); TimeApp[%lf] error!!", p.TimeApp);
    return -1;
  }
  return 0;
}

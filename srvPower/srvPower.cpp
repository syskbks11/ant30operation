/*!
\file srvPow.cpp
\date 2008.01.24
\author Y.Koide
\brief �A���g�ϑ��p�̃v���O�����B�p���[���[�^�𐧌䂷��B
* 2009.8.7
* Modified by NAGAI, for 30-cm telescope
*/
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include <time.h>
#include "../libtkb/export/libtkb.h"
#include "beProtocol.h"
#include "../powermeter/penguin_powermeter.h"
#include "srvPower.h"

const char programName[] = "srvPower";
static const char confFile[] = "../etc/srvPower.conf";
static char basePath[] = ".";
static const char logPath[] = "../log/srvPower";        //!< ���O��ۊǂ���f�B���N�g���̃p�X

typedef struct srvPow_s{
  //! �p�����[�^
  int SrvPowGpibDummy; //!< Gpib���[���g�p���邩 0:���Ȃ� 1:�[���g�p����
  int BeId;            //!< ���Ȏ���ID
/* 090811 out
  int port;            //!< Network Port
*/
  //int SrvPowCnMax;     //!< �ڑ��ő吔
  int gpibNo;          //!< GP-IB�̃A�h���X
  int devNo;           //!< PowerMeter��GP-IB�A�h���X
  const char* Group;   //!< GroupName
  const char* Project; //!< ProjectName
	const char* Table;		//!< ObsTable //***
  char* dataPath;      //!< �f�[�^��ۊǂ���f�B���N�g���̃p�X

  //! �ϑ��e�[�u��
  int ScanFlag;
  //double TimeApp;
  double TimeScan;
  double OnOffTime;
  double RSkyTime;
  double TimeOneInteg;
  double TimeDump;

  //! �ϐ�
  FILE* fp;
  void* conf;
  netServerClass_t* netsv0;
  //void** netsv;
	thrdClass_t* thrd;//090810 in
/* 090810 out
  void* thrd;
*/
  void* gpibNi;
  unsigned char* buf;   //!< ��M�o�b�t�@
  int bufSize;          //!< ��M�o�b�t�@�T�C�Y(�擾�\��T�C�Y)
  int readSize;         //!< ��M�ς݃f�[�^�T�C�Y
  int paramInit;        //!< 0:�p�����[�^���擾 1:�p�����[�^�擾�ς�
  int thrdRun;          //!< 0:�X���b�h�����s 1:�X���b�h���s��
  int integStart;       //!< 0:�ϕ����ĂȂ� 1:�ϕ��J�n�w��or�ϕ���
  double integTime;     //!< 1��̐ϕ����� sec
  int integNum;         //!< ����ϕ���A�����čs����
  double* integBuf;     //!< �ϕ��ۊǗ̈�
  beAnsStatus_t ans;      //!< �����f�[�^
  int cnNum;            //!< ���݂̐ڑ���
  time_t integStartTime;//!< �ϕ����J�n���鎞��time()�Ŏ擾���鎞���P�ʁB
  char* dataPathObs;    //!< "./[dataPath]/[Group]/[Project]"�̕�����
} srvPow_t;

static int act; //!< �ڑ����A�N�e�B�u���ǂ���

static int _reqans(srvPow_t* p, void* req, int reqSize, void** ans, int* ansSize);
static int _reqInit(srvPow_t* p, const char* paramStr);
static int _reqInteg(srvPow_t* p, void* req);
static void* _integMain(void* _p);
static int _setParam(const char** allKeyVal, srvPow_t* p);
static int _serverInit(srvPow_t* p);//090811 in

/*! \fn void* srvPowInit(const int _gpib, const int _devNo)
\brief srvPow������
\return �f�[�^�\���̂ւ̃|�C���^
*/
srvPower_Class* srvPowerInit(){
  srvPow_t* p = NULL;
  const char** allKeyVal;
  int ret;

  //! �f�[�^�\���̂̊m��
  p = new srvPow_t();
  if(!p){
    return NULL;
  }
  memset(p, 0, sizeof(*p));
  //p->SrvPowCnMax = 1;

  //! �p�����[�^�擾
  p->conf = conf2Init();
  ret = conf2AddFile(p->conf, confFile);
  conf2Print(p->conf);
  if(conf2SetKey(p->conf, "SrvPowGpibDummy")){
    p->SrvPowGpibDummy = atoi(conf2GetVal(p->conf));
  }
  //if(conf2SetKey(p->conf, "SrvPowCnMax")){
  //  p->SrvPowCnMax = atoi(conf2GetVal(p->conf));
  //}
/* 090811 moved to _serverInit()
  if(conf2SetKey(p->conf, "port")){
    p->port = atoi(conf2GetVal(p->conf));
  }
*/
  if(conf2SetKey(p->conf, "BeId")){
    p->BeId = atoi(conf2GetVal(p->conf));
  }
  if(conf2SetKey(p->conf, "gpibNo")){
    p->gpibNo = atoi(conf2GetVal(p->conf));
  }
  if(conf2SetKey(p->conf, "devNo")){
    p->devNo = atoi(conf2GetVal(p->conf));
  }

  if(conf2SetKey(p->conf, "dataPath")){
    int size = strlen(conf2GetVal(p->conf)) + 1;
    p->dataPath = new char[size];
    strcpy(p->dataPath, conf2GetVal(p->conf));
  }
  else{
    int size = strlen(basePath) + 1;
    p->dataPath = new char[size];
    strcpy(p->dataPath, basePath);
  }

  //! �p�����[�^�`�F�b�N
  int err = 0;
  //if(p->SrvPowCnMax <= 0){
  //  err = 1;
  //}
  if(p->gpibNo < 0){
    uM1("srvPowInit(); gpibNo [%d] error!!", p->gpibNo);
    err = 1;
  }
  if(p->devNo < 0){
    uM1("srvPowInit(); devNo [%d] error!!", p->devNo);
    err = 1;
  }
/* 090811 moved to _serverInit()
  if(p->port <= 0){
    uM1("srvPowInit(); port [%d] error!!", p->port);
    err = 1;
  }
*/
  if(err){
    srvPowerEnd(p);
    return NULL;
  }

  //! netsv������
	err = _serverInit(p);//090811 in
	if(err){
		srvPowerEnd(p);
		return NULL;
	}
/* 090811 out
  p->netsv0 = netsvInit(p->port);
  if(!p->netsv0){
    uM("srvPowInit(); netsvInitWithProcessMax(); error");
    srvPowerEnd(p);
    return NULL;
  }
*/
  //p->netsv = new void*[p->SrvPowCnMax + 1];
  //p->buf = new unsigned char*[p->SrvPowCnMax];
  //p->bufSize = new int[p->SrvPowCnMax];
  //p->readSize = new int[p->SrvPowCnMax];
  //memset(p->netsv, 0, sizeof(*p->netsv) * (p->SrvPowCnMax + 1));
  //memset(p->buf, 0, sizeof(*p->buf) * p->SrvPowCnMax);
  //memset(p->bufSize, 0, sizeof(*p->bufSize) * p->SrvPowCnMax);
  //memset(p->readSize, 0, sizeof(*p->readSize) * p->SrvPowCnMax);

  //! GP-IB������
  if(!p->SrvPowGpibDummy){//090828 in, to check communication
	ret = penguin_powermeter_init(p->devNo);
	if(ret){
		uM("penguin_powermeter_init() failed. ");
		return NULL;
	}
	penguin_CmdTxCommand("UNIT:POW DBM");
	penguin_CmdTxCommand("INIT:CONT OFF");
	penguin_CmdTxCommand("AVER:COUNT:AUTO OFF");
  }
  if(!p->SrvPowGpibDummy){//090828 in
	penguin_powermeter_end();
  }

  //! �ϕ��p�̃X���b�h�N��
  p->thrd = thrdInit(_integMain, (void*)p);
  if(!p->thrd){
    uM("srvPowInit(); thrdInit(); error");
    srvPowerEnd(p);
    return NULL;
  }
  return (srvPower_Class*)p;
}

/*! \fn int srvPowEnd(void* _p)
\brief �I�����ɕK���Ăяo��
\return 0:Success
*/
int srvPowerEnd(srvPower_Class* p){
//  srvPow_t* p = (srvPow_t*)_p;

  if(p){
    if(p->thrd){
      p->thrdRun = 0;
      thrdEnd(p->thrd);
      p->thrd = NULL;
    }
    //if(p->netsv){
    //  for(int i = 0; i < p->SrvPowCnMax; i++){
    //    if(p->netsv[i]){
    //      netsvDisconnectProcess(p->netsv[i]);
    //      p->netsv[i] = NULL;
    //    }
    //  }
    //  delete[] p->netsv;
    //  p->netsv = NULL;
    //}
    if(p->netsv0){
      netsvEnd(p->netsv0);
      p->netsv0 = NULL;
    }
    if(p->buf){
      //for(int i = 0; i < p->SrvPowCnMax; i++){
      //  if(p->buf[i]){
      //    delete[] p->buf[i];
      //  }
      //}
      delete[] p->buf;
      p->buf = NULL;
    }
    //if(p->bufSize){
    //  delete[] p->bufSize;
    //  p->bufSize = NULL;
    //}
    //if(p->readSize){
    //  delete[] p->readSize;
    //  p->readSize = NULL;
    //}
    if(p->dataPath){
      delete[] p->dataPath;
      p->dataPath = NULL;
    }
    if(p->dataPathObs){
      delete[] p->dataPathObs;
      p->dataPathObs = NULL;
    }
    delete p;
    p = NULL;
  }
  return 0;
}

/*! \fn int srvPowRepeat(void* _p)
\brief ��������Ƃ̒ʐM�҂������A�ʐM���s��
*/
int srvPowerRepeat(srvPower_Class* p){
//  srvPow_t* p = (srvPow_t*)_p;
  int ret;
  
  //netsvSetTimeOut(p->netsv0, 0);
	uM("### srvPowerRepeat() started. ###");
  while(1){
    ret = netsvWaiting(p->netsv0);
    if(ret < 0){
      //! error
      return 0;
    }
    if(ret == 0){
      //! connect
      break;
    }
    //! �ڑ��Ȃ�
    tmSleepMSec(1000);
    continue;
  }

  act = 1;
  while(act){
	p->bufSize = sizeof(long);
	if(p->buf){
		delete p->buf;
	}
	p->buf = new unsigned char[p->bufSize];
	p->readSize = 0;

	//! ���N�G�X�g���擾
	int i = 0;
	for(i = 0; i < 10; i++){
		ret = netsvRead(p->netsv0, p->buf + p->readSize, p->bufSize - p->readSize);
		if(ret < 0){
			uM1("srvPowRepeat(); netsvRead(); netsv[%d] error!!", i);
			act = 0;
			break;
		}
		p->readSize += ret;
		if(p->readSize == sizeof(long)){
			//! ���N�G�X�g�w�b�_�擾
			p->bufSize = *((int*)p->buf);
			delete[] p->buf;
			p->buf = new unsigned char[p->bufSize];
			*((int*)p->buf) = p->bufSize;
			continue;
		}
		if(p->readSize >= p->bufSize){
			//! ���N�G�X�g�f�[�^�擾����
			break;
		}
		tmSleepMSec(1);
	}
	if(i == 10){
		uM("srvPowRepeat(); netsvRead(); Timeout error!!");
		break;
	}
	if(act == 0){
		//! Network InActive
		break;
	}

	//! ���N�G�X�g�����s
	void* ans;
	int ansSize;
	int writeSize = 0;

	ret = _reqans(p, p->buf, p->bufSize, &ans, &ansSize);
	//if(ret){
	//  uM1("srvPowRepeat(); reqans(); [%d] error!!", ret);
	//}

	//! �����f�[�^�𑗐M�B�^�C���A�E�g1msec*10��
	p->bufSize = sizeof(long);
	writeSize = 0;
	for(int i = 0; i < 10; i++){
		ret = netsvWrite(p->netsv0, (const unsigned char*)ans + writeSize, ansSize - writeSize);
		if(ret < 0){
			uM("srvPowRepeat(); netsvWrite(); error!!");
			act = 0;
			break;
		}
		writeSize += ret;
		if(writeSize == ansSize){
			uM1("srvPowRepeat(); answer to the cliant successfully [endExeFlag=%d].", p->ans.endExeFlag)
			break;
		}
		if(i == 10 - 1){//090810 in
	/* 090810 out
		if(i = 10 - 1){
	*/
			uM("srvPowRepeat(); netsvWrite(); Timeout error!!");
			break;
		}
		tmSleepMSec(1);
	}
	if(act == 0){
	break;
	}
  }
	uM("srvPowRepeat(); netsvDisconnect() invoke.");
  netsvDisconnect(p->netsv0);
	uM("srvPowRepeat(); netsvDisconnect() returned.");
  p->thrdRun = 0;
	uM("srvPowRepeat(); thrdStop() invoke.");
  thrdStop(p->thrd);
	uM("srvPowRepeat(); thrdStop() returned.");
  p->paramInit = 0;

  return 0;
}

/*! \fn int reqans(srvPow_t* p, void* req, int reqSize, void** ans, int* ansSize)
\brief ���N�G�X�g����ǂ������f�[�^���Z�b�g����B
\param[in] p �f�[�^�\����
\param[in] req ���N�G�X�g�f�[�^
\param[in] reqSize ���N�G�X�g�f�[�^�T�C�Y
\param[out] ans �����f�[�^
\param[out] ansSize �����f�[�^�T�C�Y
\return 0:Success
*/
int _reqans(srvPow_t* p, void* req, int reqSize, void** ans, int* ansSize){
  int ret;
  char* paramStr;

  p->ans.size = sizeof(p->ans);
  p->ans.acqErr = 1; //!< 1:Normal
  if(!p->paramInit){
		//! �������v��
		uM("reqans(); Request Initialize");
		beReqInit_t* dat = (beReqInit_t*)req;
		if(dat->paramSize > 0 && dat->paramSize < dat->size){
			paramStr = new char[dat->paramSize + 1];
		}
		memcpy(paramStr, &dat->param, dat->paramSize);
		paramStr[dat->paramSize] = '\0';
		ret = _reqInit(p, paramStr);
		if(ret){
			uM("reqans(); Initialize fault");
			p->ans.endExeFlag = 5; //!< 5:Reject
		}
		else{
			uM("reqans(); Initialize success");
			p->paramInit = 1;
			p->ans.endExeFlag = 4; //!< 4:Accept
		}
		delete[] paramStr;
  }
  else{
		//! ��ʃ��N�G�X�g
		beReqInteg_t* dat = (beReqInteg_t*)req;
		if(dat->exeFlag == 0){
			//! stop integ
			uM("reqans(); Request stop integ");
			p->ans.endExeFlag = 4; //!< 4:Accept
			p->integStart = 0;
		}
		else if(dat->exeFlag == 1){
			//*** 2011/06/24 in
			fprintf(p->fp, "%s %lf %lf %lf %lf %lf %lf %lf %lf %lf",
				uGetTime(), dat->PAZ, dat->PEL, dat->DAZ, dat->DEL, dat->RA, dat->DEC, dat->DRA, dat->DDEC, dat->IPINT);
			switch(dat->OnOffRoad) {
				case 0://! ON
				fprintf(p->fp, " ON");
				break;
				case 1://! OFF
				fprintf(p->fp, " OFF");
				break;
				case 2://! LOAD
				fprintf(p->fp, " LOAD");
				break;
			}

			//! start integ
			uM("reqans(); Request start integ");
			ret = _reqInteg(p, (void*)req);
			if(ret){
				uM("reqans(); Reject start integ!!");
				p->ans.endExeFlag = 5; //!< 5:Reject �{���͐ϕ����ȂǏꍇ�킯���ׂ��B
			}
			else{
				uM("reqans(); Accept start integ");
				p->ans.endExeFlag = 4; //!< 4:Accept
			}
		}
		else if(dat->exeFlag == 2){
			//! status
			if(p->integStart){
				p->ans.endExeFlag = 3; //!< 3:Execution
			}
			else{
				p->ans.endExeFlag = 2; //!< 2:Standby �{���͐ϕ���������Success�����s����Failed��Ԃ��B
			}
		}else{
			uM1("reqans(); Invalid exeFlag (%d)!!", dat->exeFlag);
			p->ans.endExeFlag = 5; //!< 5:Reject
		}
  }
  *ans = (void*)&p->ans;
  *ansSize = sizeof(p->ans);
  return 0;
}

/*! \fn int reqInit(srvPow_t* p, const char* paramStr)
\brief �������B���N�G�X�g����
\param[in] p �f�[�^�\����
\param[in] paramStr �p�����[�^������
\return 0:Success 1:Error
*/
int _reqInit(srvPow_t* p, const char* paramStr){
  int ret;
	char obs[16];	//*** 2010/12/09 in
	char tmp[64];	//*** 2010/12/09 in
  const char** allKeyVal;

  //! �O��̃X���b�h�������Ă���ꍇ�͒�~����B
  if(p->thrdRun){
    p->thrdRun = 0;
    thrdStop(p->thrd);
  }

  conf2End(p->conf);
  p->conf = conf2Init();
  conf2AddStr(p->conf, paramStr);
  conf2Print(p->conf);

  allKeyVal = conf2GetAllKeyVal2(p->conf);
  ret = _setParam(allKeyVal, p);
  if(ret){
    uM("reqInit(); setParam(); error!!");
    return 1;
  }

	//*** 2010/12/09 in
	//! �V�K�Ƀ��O�t�@�C�����쐬
	uEnd();
	ret = strlen(p->Table);
	strncpy(obs, p->Table, ret-4);
	obs[ret-4] = '\0';
	sprintf(tmp, "%s/%s-%s", logPath, programName, obs);
	ret = uInit(tmp);
	if(ret) {
		return 1;
	}
	uM("new obs-logFile start success!!");
	conf2Print(p->conf);

  //! �f�[�^�ۊǃf�B���N�g���𐶐�����
  if(p->dataPathObs){
    delete[] p->dataPathObs;
  }
  int size = strlen(p->dataPath) + strlen(p->Group) + strlen(p->Project) + strlen("mkdir -p") + 1;
  p->dataPathObs = new char[size];
#ifdef WIN32
  sprintf(p->dataPathObs, "mkdir %s\\%s\\%s", p->dataPath, p->Group, p->Project);
#else
  sprintf(p->dataPathObs, "mkdir -p %s/%s/%s", p->dataPath, p->Group, p->Project);
#endif
  system(p->dataPathObs);
  sprintf(p->dataPathObs, "%s/%s/%s", p->dataPath, p->Group, p->Project);
	uM1("I checked directory %s. ", p->dataPathObs);

  int integNum;
  double num;
    if((0 < p->ScanFlag && p->ScanFlag <= 5) || (1000 < p->ScanFlag && p->ScanFlag <= 1002) ){//090812 in
/* 090812 out
    if(p->ScanFlag != 6){
*/
    //! Raster�ȊO
    integNum = 1;
  }
    else if(6 <= p->ScanFlag && p->ScanFlag <= 7){//090812 in; Raster & Cross; 090827 corrected.
/* 090812 out
    else{
*/
    //! Raster
    //num = p->TimeScan / p->TimeOneInteg;
    num = p->TimeScan / p->TimeOneInteg + 1.1;//*** 2010/12/08 in 
    integNum = (int)num;
    //uM2("debug!! num = %f, integNum = %d\n", num, integNum);//***
    }else{//090812 in
		uM1("Invalid ScanFlag (%d)!! I assume the requesst is normal. ", p->ScanFlag);
		integNum = 1;
  }
  p->integBuf = new double[integNum];

  ret = thrdStart(p->thrd);		// thred start(lib file�Q��)
  if(ret){
    uM("reqInit(); thrdStart(); error");
    srvPowerEnd(p);
    return NULL;
  }
  return 0;
}

/*! \fn int reqInteg(void* _p)
\brief �ϕ��B���N�G�X�g����
\param[in] p �f�[�^�\����
\param[in] req ���N�G�X�g�f�[�^
\return 0:Success 1:In Execution
*/
int _reqInteg(srvPow_t* p, void* req){
  beReqInteg_t* dat = (beReqInteg_t*)req;

  if(p->integStart){
    //! ���łɐϕ����ł���
    return 1;
  }

  p->integStartTime = dat->nowTime;
  if(dat->OnOffRoad == 0){
    //! On
    uM("## On-Point");
    if((0 < p->ScanFlag && p->ScanFlag <= 5) || (1000 < p->ScanFlag && p->ScanFlag <= 1002) ){//090811 in
/* 090811 out
    if(p->ScanFlag != 6){
*/
      p->integTime = p->OnOffTime; //! �ϕ����Ԃ�ݒ�
      p->integNum = 1;
    }
    else if(6 <= p->ScanFlag && p->ScanFlag <= 7){//090811 in; Raster & Cross; 090827 corrected. 
/* 090811 out
    else{
*/
      //! Raster
      p->integTime = p->TimeOneInteg;
      //p->integNum = p->TimeScan / p->TimeOneInteg;
      double test = p->TimeScan / p->TimeOneInteg + 1.1;//*** 2010/12/08 in
      p->integNum = (int)test;
      //uM2("debug!! test = %f, p->integnum = %d\n", test, p->integNum);//***
    }else{//090811 in
		uM1("Invalid ScanFlag (%d)!! I assume the requesst is normal. ", p->ScanFlag);
		p->integTime = p->OnOffTime; //! �ϕ����Ԃ�ݒ�
		p->integNum = 1;
	}
  }
  else if(dat->OnOffRoad == 1){
    //! Off
    uM("## Off-Point");
    p->integTime = p->OnOffTime;
    p->integNum = 1;
  }
  else if(dat->OnOffRoad == 2){
    //! Load
    uM("## Load");
    p->integTime = p->RSkyTime;
    p->integNum = 1;
  }else{//090810 in
		uM1("Invalid OnOffRoad (%d)!! Please report to the programmer. ", dat->OnOffRoad);
	}
  p->integStart = 1;

  return 0;
}

/*! \fn void* integMain(void* _p)
\brief �ϕ�����������B�X���b�h�ɂď����B
\param[in] _p �f�[�^�\����
\reutrn NULL
*/
void* _integMain(void* _p){		// �X���b�h�ŋN�������֐�
	uM("_integMain() invoked.");
  srvPow_t* p = (srvPow_t*)_p;
  tmClass_t* vtm;
  int cnt;
  const int tmpSize = 1024;
  char tmp[tmpSize] = {'\0'};
  char fname[1024];
  double lag;
  int ret;
  char timeStr[16];

  //! �����Ǘ�������
  vtm = tmInit();

  //! GP-IB������
  if(!p->SrvPowGpibDummy){
	ret = penguin_powermeter_init(p->devNo);
	if(ret){
		return NULL;
	}
	penguin_CmdTxCommand("UNIT:POW DBM");
	penguin_CmdTxCommand("INIT:CONT OFF");
	penguin_CmdTxCommand("AVER:COUNT:AUTO OFF");
/*
    p->gpibNi = gpibNiInit(p->gpibNo, p->devNo);
    if(!p->gpibNi){
      return NULL;
    }
    sprintf(tmp, "UNIT:POW DBM");
    gpibNiWrite(p->gpibNi, tmp, strlen(tmp));
    sprintf(tmp, "INIT:CONT OFF");
    gpibNiWrite(p->gpibNi, tmp, strlen(tmp));
    sprintf(tmp, "AVER:COUNT:AUTO OFF");
    gpibNiWrite(p->gpibNi, tmp, strlen(tmp));
*/

  }

  //! �L�^�t�@�C�����J��
  time_t t;
  struct tm* stm;
  time(&t);
  stm = localtime(&t);
	char obsfilename[32];//0909811 in
	memset(obsfilename, 0, sizeof(obsfilename));
	if(conf2SetKey(p->conf, "ObsTable")){
		const char* obsTable = conf2GetVal(p->conf);
		strncpy(obsfilename, obsTable, strlen(obsTable)-4);
	}
	sprintf(fname, "%s/srvPow-%s-%04d%02d%02d%02d%02d%02d.dat", 
		p->dataPathObs, obsfilename, stm->tm_year + 1900, stm->tm_mon + 1, stm->tm_mday, stm->tm_hour, stm->tm_min, stm->tm_sec);
/* 090811 out
  sprintf(fname, "%s/srvPow-%04d%02d%02d%02d%02d%02d.dat",
    p->dataPathObs, stm->tm_year + 1900, stm->tm_mon + 1, stm->tm_mday, stm->tm_hour, stm->tm_min, stm->tm_sec);
*/
  p->fp = fopen(fname, "wb");
  if(!p->fp){
    uM1("integMain(); fopen(); file(%s) open error!!", fname);
	return NULL;//090811 in
/* 090811 out
    p->thrdRun;
*/
  }  //*** 2011/06/24 in
  fprintf(p->fp, "# Time AZ EL dAZ dEL RA DEC dRA dDEC tLoad OnOffLoad Pow\n");

  p->thrdRun = 1;
  while(p->thrdRun){
    //! integStart��0�ȊO�ɂȂ�܂őҋ@����
    while(!p->integStart && p->thrdRun){
      tmSleepMSec(1);
    }
    //! ����J�n
    //uM("integMain(); Start integ");
    stm = localtime(&p->integStartTime);
    sprintf(timeStr, "%02d:%02d:%02d", stm->tm_hour, stm->tm_min, stm->tm_sec);
    uM3("## integStartTime=%s integTime=%lf sec integNum=%d", timeStr, p->integTime, p->integNum);

    //! �J�E���g���Ɋ��Z����B
    cnt = p->integTime * 20.835 - 3.4832;
    if(cnt <= 0){
      cnt = 1;
    }

	//! �ϕ����s��
	time_t startTime = p->integStartTime;
	memset(p->integBuf, 0, sizeof(*p->integBuf) * p->integNum);
	int i;
	for(i = 0; i < p->integNum && p->integStart && p->thrdRun; i++){
		//! ���ԂɂȂ�܂őҋ@
		double ms = i * p->TimeOneInteg * 1000.0;
		while(tmGetDiff(startTime, ms) > 0.001){
		}
		if(!p->SrvPowGpibDummy){
			sprintf(tmp, "AVER:COUNT %d", cnt);
			penguin_CmdTxCommand(tmp);
			/*
			gpibNiWrite(p->gpibNi, tmp, strlen(tmp));
			*/
		}
		uM1("# integ start No.[%03d]", i + 1);

	      //tmSleepMSec((p->integTime - 0.01) * 1000.0);
		if(!p->SrvPowGpibDummy){
			while(1){
				ret = penguin_CmdTxCommand("READ?");
				uM("# READ? send");
			/*
			  sprintf(tmp, "READ?");
			  ret = gpibNiWrite(p->gpibNi, tmp, strlen(tmp));
			*/
				if(ret < 0){
					break;
				}
				ret = penguin_CmdRxData();
				uM("# READ? received");
				if(ret > 0 ){
					sprintf(tmp, "%s", penguin_chopper_getMessage());
//					printf("mes: %s\n", tmp);
					break;
				}
			/*
			  ret = gpibNiRead(p->gpibNi, tmp, tmpSize);
			  if(ret){
			    break;
			  }
			*/
			  tmSleepMSec(1);
			}
		}
		else{
			sprintf(tmp, "0.0");
		}
		//lag = tmGetLag(vtm);
		//! �ϕ��f�[�^�擾
		p->integBuf[i] = atof(tmp);
		//! debug
		//printf("%s %.04lf dBm (%lf sec)\n", uGetTime(), p->integBuf[i], lag);
		if(p->thrdRun){
			uM2("# integ end   No.[%03d] %.04lf dBm", i + 1, p->integBuf[i]);
		}
		else{
			break;
		}
	}
	if(i == p->integNum){
		uM("integ success");
		for(i = 0; i < p->integNum; i++){//090811 in
			fprintf(p->fp, " %lf\n", p->integBuf[i]);
		}
/* 090811 out
		fwrite(p->integBuf, sizeof(*p->integBuf) * p->integNum, 1, p->fp);
*/
	}
	else{
		uM("integ fault!!");
	}
	fflush(p->fp);		// fflush():�W���֐�
	p->integStart = 0;
  }
  fclose(p->fp);
  if(!p->SrvPowGpibDummy){
	penguin_powermeter_end();
/*
    gpibNiEnd(p->gpibNi);
*/
  }
  tmEnd(vtm);
  return NULL;
}

int _setParam(const char** allKeyVal, srvPow_t* p){
  //! ScanFlag, TimeApp, LineTime, OnOffTime, RSkyTime, TimeOneInteg
  char tmp[256];

  if(conf2SetKey(p->conf, "Group")){
    p->Group = conf2GetVal(p->conf);
  }
  else{
    uM("setParam(); Group(NULL) error!!");
    return -1;
  }
  if(conf2SetKey(p->conf, "Project")){
    p->Project = conf2GetVal(p->conf);
  }
  else{
    uM("setParam(); Project(NULL) error!!");
    return -1;
  }

	//*** 2010/12/09 in
	if(conf2SetKey(p->conf, "ObsTable")) {
		p->Table = conf2GetVal(p->conf);
	}
	else {
		uM("setParam(); ObsTable(NULL) error!!");
		return -1;
	}

  if(conf2SetKey(p->conf, "ScanFlag")){
    p->ScanFlag = atoi(conf2GetVal(p->conf));
  }
  if(conf2SetKey(p->conf, "TimeScan")){
    p->TimeScan = atof(conf2GetVal(p->conf));
  }
  if(conf2SetKey(p->conf, "OnOffTime")){
    p->OnOffTime = atof(conf2GetVal(p->conf));
  }
  if(conf2SetKey(p->conf, "RSkyTime")){
    p->RSkyTime = atof(conf2GetVal(p->conf));
  }
  //*** 2010/12/08 in
  if(conf2SetKey(p->conf, "TimeDump")){
    p->TimeDump = atof(conf2GetVal(p->conf));
  }

  if(conf2SetKey(p->conf, "TimeOneInteg")){//090811 in
/* 090811 out
  sprintf(tmp, "TimeOneInteg", p->BeId);
  if(conf2SetKey(p->conf, tmp)){
*/
    p->TimeOneInteg = atof(conf2GetVal(p->conf));
    if(p->TimeDump > p->TimeOneInteg) {
      p->TimeOneInteg = p->TimeDump;
    }
    else {
      uM2("dumpTime < %f !! modified to %f\n", p->TimeOneInteg, p->TimeOneInteg);
    }
  }
  
  return 0;
}

int _serverInit(srvPow_t* p){
	int port = 0;
  if(conf2SetKey(p->conf, "port")){
    port = atoi(conf2GetVal(p->conf));
  }
  if(port <= 0){
    uM1("srvPowInit(); port [%d] error!!", port);
    return 1;
  }
  p->netsv0 = netsvInit(port);
  if(!p->netsv0){
    uM("srvPowInit(); netsvInitWithProcessMax(); error");
    return 2;
  }
	return 0;//normal end
}


/*!
\file srvPow.cpp
\date 2008.01.24
\author Y.Koide
\brief �A���g�ϑ��p�̃v���O�����B�p���[���[�^�𐧌䂷��B
*/
#ifdef _DEBUG
# pragma comment(lib, "import/libtkbD.lib")
#else
# pragma comment(lib, "import/libtkb.lib")
#endif

# pragma comment(lib, "import/gpibNi.lib")

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include <time.h>
#include "import/libtkb.h"
#include "import/beProtocol.h"
#include "import/gpibNi.h"
#include "srvPow.h"

static const char confFile[] = "srvPow.conf";
static char basePath[] = ".";

typedef struct srvPow_s{
  //! �p�����[�^
  int SrvPowGpibDummy; //!< Gpib���[���g�p���邩 0:���Ȃ� 1:�[���g�p����
  int BeId;            //!< ���Ȏ���ID
  int port;            //!< Network Port
  //int SrvPowCnMax;     //!< �ڑ��ő吔
  int gpibNo;          //!< GP-IB�̃A�h���X
  int devNo;           //!< PowerMeter��GP-IB�A�h���X
  const char* Group;   //!< GroupName
  const char* Project; //!< ProjectName
  char* dataPath;      //!< �f�[�^��ۊǂ���f�B���N�g���̃p�X

  //! �ϑ��e�[�u��
  int ScanFlag;
  //double TimeApp;
  double TimeScan;
  double OnOffTime;
  double RSkyTime;
  double TimeOneInteg;

  //! �ϐ�
  FILE* fp;
  void* conf;
  void* netsv0;
  //void** netsv;
  void* thrd;
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

int act; //!< �ڑ����A�N�e�B�u���ǂ���

static int reqans(srvPow_t* p, void* req, int reqSize, void** ans, int* ansSize);
static int reqInit(srvPow_t* p, const char* paramStr);
static int reqInteg(srvPow_t* p, void* req);
static void* integMain(void* _p);
static int setParam(const char** allKeyVal, srvPow_t* p);

/*! \fn void* srvPowInit(const int _gpib, const int _devNo)
\brief srvPow������
\return �f�[�^�\���̂ւ̃|�C���^
*/
void* srvPowInit(){
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
  if(conf2SetKey(p->conf, "port")){
    p->port = atoi(conf2GetVal(p->conf));
  }
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
  if(p->port <= 0){
    uM1("srvPowInit(); port [%d] error!!", p->port);
    err = 1;
  }
  if(err){
    srvPowEnd((void*)p);
    return NULL;
  }

  //! netsv������
  p->netsv0 = netsvInit(p->port);
  if(!p->netsv0){
    uM("srvPowInit(); netsvInitWithProcessMax(); error");
    srvPowEnd((void*)p);
    return NULL;
  }
  //p->netsv = new void*[p->SrvPowCnMax + 1];
  //p->buf = new unsigned char*[p->SrvPowCnMax];
  //p->bufSize = new int[p->SrvPowCnMax];
  //p->readSize = new int[p->SrvPowCnMax];
  //memset(p->netsv, 0, sizeof(*p->netsv) * (p->SrvPowCnMax + 1));
  //memset(p->buf, 0, sizeof(*p->buf) * p->SrvPowCnMax);
  //memset(p->bufSize, 0, sizeof(*p->bufSize) * p->SrvPowCnMax);
  //memset(p->readSize, 0, sizeof(*p->readSize) * p->SrvPowCnMax);

  //! �ϕ��p�̃X���b�h�N��
  p->thrd = thrdInit(integMain, (void*)p);
  if(!p->thrd){
    uM("srvPowInit(); thrdInit(); error");
    srvPowEnd((void*)p);
    return NULL;
  }
  return (void*)p;
}

/*! \fn int srvPowEnd(void* _p)
\brief �I�����ɕK���Ăяo��
\return 0:Success
*/
int srvPowEnd(void* _p){
  srvPow_t* p = (srvPow_t*)_p;

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
int srvPowRepeat(void* _p){
  srvPow_t* p = (srvPow_t*)_p;
  int ret;
  
  //netsvSetTimeOut(p->netsv0, 0);
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

    ret = reqans(p, p->buf, p->bufSize, &ans, &ansSize);
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
        break;
      }
      if(i = 10 - 1){
        uM("srvPowRepeat(); netsvWrite(); Timeout error!!");
        break;
      }
      tmSleepMSec(1);
    }
    if(act == 0){
      break;
    }
  }
  netsvDisconnect(p->netsv0);
  p->thrdRun = 0;
  thrdStop(p->thrd);
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
int reqans(srvPow_t* p, void* req, int reqSize, void** ans, int* ansSize){
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
    ret = reqInit(p, paramStr);
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
      //! start integ
      uM("reqans(); Request start integ");
      ret = reqInteg(p, (void*)req);
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
int reqInit(srvPow_t* p, const char* paramStr){
  int ret;
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
  ret = setParam(allKeyVal, p);
  if(ret){
    uM("reqInit(); setParam(); error!!");
    return 1;
  }

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

  int integNum;
  if(p->ScanFlag != 6){
    //! Raster�ȊO
    integNum = 1;
  }
  else{
    //! Raster
    integNum = p->TimeScan / p->TimeOneInteg;
  }
  p->integBuf = new double[integNum];

  ret = thrdStart(p->thrd);		// thred start(lib file�Q��)
  if(ret){
    uM("reqInit(); thrdStart(); error");
    srvPowEnd((void*)p);
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
int reqInteg(srvPow_t* p, void* req){
  beReqInteg_t* dat = (beReqInteg_t*)req;

  if(p->integStart){
    //! ���łɐϕ����ł���
    return 1;
  }

  p->integStartTime = dat->nowTime;
  if(dat->OnOffRoad == 0){
    //! On
    uM("## On-Point");
    if(p->ScanFlag != 6){
      p->integTime = p->OnOffTime; //! �ϕ����Ԃ�ݒ�
      p->integNum = 1;
    }
    else{
      //! Raster
      p->integTime = p->TimeOneInteg;
      p->integNum = p->TimeScan / p->TimeOneInteg;
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
  }
  p->integStart = 1;

  return 0;
}

/*! \fn void* integMain(void* _p)
\brief �ϕ�����������B�X���b�h�ɂď����B
\param[in] _p �f�[�^�\����
\reutrn NULL
*/
void* integMain(void* _p){		// �X���b�h�ŋN�������֐�
  srvPow_t* p = (srvPow_t*)_p;
  void* vtm;
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
  }


  //! �L�^�t�@�C�����J��
  time_t t;
  struct tm* stm;
  time(&t);
  stm = localtime(&t);
  sprintf(fname, "%s/srvPow-%04d%02d%02d%02d%02d%02d.dat",
    p->dataPathObs, stm->tm_year + 1900, stm->tm_mon + 1, stm->tm_mday, stm->tm_hour, stm->tm_min, stm->tm_sec);
  p->fp = fopen(fname, "wb");
  if(!p->fp){
    uM("integMain(); fopen(); file(%s) open error!!", fname);
    p->thrdRun;
  }

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
        gpibNiWrite(p->gpibNi, tmp, strlen(tmp));
      }
      uM1("# integ start No.[%03d]", i + 1);

      //tmSleepMSec((p->integTime - 0.01) * 1000.0);
      if(!p->SrvPowGpibDummy){
        while(1){
          sprintf(tmp, "READ?");
          ret = gpibNiWrite(p->gpibNi, tmp, strlen(tmp));
          if(ret < 0){
            break;
          }
          ret = gpibNiRead(p->gpibNi, tmp, tmpSize);
          if(ret){
            break;
          }
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
      fwrite(p->integBuf, sizeof(*p->integBuf) * p->integNum, 1, p->fp);
    }
    else{
      uM("integ fault!!");
    }
    fflush(p->fp);		// fflush():�W���֐�
    p->integStart = 0;
  }
  fclose(p->fp);
  if(!p->SrvPowGpibDummy){
    gpibNiEnd(p->gpibNi);
  }
  tmEnd(vtm);
  return NULL;
}

int setParam(const char** allKeyVal, srvPow_t* p){
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
  sprintf(tmp, "TimeOneInteg", p->BeId);
  if(conf2SetKey(p->conf, tmp)){
    p->TimeOneInteg = atof(conf2GetVal(p->conf));
  }

  return 0;
}
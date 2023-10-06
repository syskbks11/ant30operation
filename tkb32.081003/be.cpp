/*!
\file be.cpp
\author Y.Koide
\date 2006.12.20
\brief �o�b�N�G���h�Ƃ���肷��
*/
#include <stdlib.h>
#include <memory.h>
#include <time.h>

#include "../libtkb/export/libtkb.h"
#include "beProtocol.h"
#include "errno.h"
#include "be.h"

//! �o�b�N�G���h�ւ̃��N�G�X�g�\����
typedef struct sReqBeTmp{
  long size;
  int exeFlag;
}tReqBeTmp;

typedef struct sParamBe{
  //! �p�����[�^
  int BeNum;                 //!< �o�b�N�G���h�̑��䐔
  int* BeUse;                //!< �ǂ̃o�b�N�G���h���g�p���邩 0:���g�p 1:�g�p���� 2:�[���g�p(�l�b�g���[�N�ڑ������Ȃ�)
  const char** BeIp;         //!< �e�o�b�N�G���h��IP
  int* BePort;               //!< �e�o�b�N�G���h��Port�ԍ�
  double BeInterval;         //!< be�ɃA�N�Z�X����ŏ����� [sec]

  //! �ϐ�
  void* vtm;                 //!< ����tm�p
  void** net;                //!< netcl�p
  int* netAct;               //!< netWork�̏�� 1:Online 0:Offline
  beReqInteg_t* req;               //!< �ʐM�f�[�^�B���N�G�X�g�\����
  beAnsStatus_t* ans;               //!< �ʐM�f�[�^�B�A���T�[�\����
}tParamBe;

static int init = 0;         //!< beInit���������Ă��邩 0:�܂� 1:����
static tParamBe p;

static int myRead(int beId, unsigned char* buf, long bufSize);
static int myWrite(int beId, const unsigned char* buf, long bufSize);
static void setParam();
static int checkParam();

/*! \fn int beInit()
\brief ������
\retval 0 ����
\retval 0�ȊO �G���[�R�[�h
*/
int beInit(){
  int ret;

  //! ���łɏ��������Ă�����I��������B
  //if(init){
  //  beEnd();
  //}

  //! �p�����[�^�擾
  memset(&p, 0, sizeof(p));
  setParam();
  if(checkParam()){
    //uM("beInit(); checkParam(); error");
    return BE_PARAM_ERR;
  }

  //! ������
  init = 1;
  
  p.net = new void*[p.BeNum];
  p.netAct = new int[p.BeNum];
  p.req = new beReqInteg_t[p.BeNum];
  p.ans = new beAnsStatus_t[p.BeNum];
  memset(p.net, 0, sizeof(*p.net) * p.BeNum);
  memset(p.netAct, 0, sizeof(*p.netAct) * p.BeNum);
  memset(p.req, 0, sizeof(*p.req) * p.BeNum);
  memset(p.ans, 0, sizeof(*p.ans) * p.BeNum);

  p.vtm = tmInit(); //!< �O�̂���tm�͏��������Ă���
  for(int i = 0; i < p.BeNum; i++){
    if(p.BeUse[i] != 1){
      continue;
    }
    p.net[i] = netclInit(p.BeIp[i], p.BePort[i]);
    if(p.net[i] == NULL){
      uM2("beInit(); netclInit(BeIp%02d, BePort%02d); error!!", i + 1, i + 1);
      beEnd();
      return BE_NET_ERR;
    }
  }
  
  ret = beReqInit();
  if(ret){
    uM1("beInit(); beReqInit(); return %d error!!", ret);
    return ret;
  }
  return 0;
}

/*! \fn int beEnd()
\brief �I�����ɌĂяo��
\retval 0 ����
*/
int beEnd(){
  if(init){
    if(p.BeUse){
      delete[] p.BeUse;
      p.BeUse = NULL;
    }
    if(p.BeIp){
      delete[] p.BeIp;
      p.BeIp = NULL;
    }
    if(p.BePort){
      delete[] p.BePort;
      p.BePort = NULL;
    }

    if(p.vtm){
      tmEnd(p.vtm);
      p.vtm = NULL;
    }
    if(p.net){
      for(int i = 0; i < p.BeNum; i++){
	if(p.net[i]){
	  netclEnd(p.net[i]);
	  p.net[i] = NULL;
	}
      }
      delete[] p.net;
      p.net = NULL;
    }
    if(p.netAct){
      delete[] p.netAct;
      p.netAct = NULL;
    }
    if(p.req){
      delete[] p.req;
      p.req = NULL;
    }
    if(p.ans){
      delete[] p.ans;
      p.ans = NULL;
    }
  }
  init = 0;
  return 0;
}

/*! \fn int beReqInit()
\brief �����v������������B�p�����[�^�[�𑗐M����B
\retval 0 ����
\retval 0�ȊO �G���[�R�[�h
*/
int beReqInit(){
  const char* allParam;
  beReqInit_t* initDat;
  long size;
  int ret;
  int e = 0;

  if(init == 0){
    return BE_NOT_INIT_ERR;
  }

  allParam = confGetAllKeyVal();
  size = sizeof(initDat->size) + sizeof(initDat->paramSize) + strlen(allParam);
  initDat = (beReqInit_t*)(new unsigned char[size]);
  initDat->size = size;
  initDat->paramSize = strlen(allParam); //! �I�[����'\0'�͊܂܂Ȃ�
  uM2("size=%d, paramSize=%d", size, initDat->paramSize);
  memcpy(&initDat->param, allParam, initDat->paramSize);
  for(int i = 0; i < p.BeNum; i++){
    if(p.BeUse[i] != 1 || p.netAct[i] == 1){
      continue;
    }
    //! �������w�߂𑗐M
    ret = netclWrite(p.net[i], (const unsigned char*)initDat, initDat->size);
    //printf("saaacqInit(); size=%d paramSize=%d\n", size, initDat->paramSize);
    if(ret <= 0){
      uM2("beReqInit(); netclWrite(BeIp%02d); return %d error!!", i + 1, ret);    
      if(ret < 0){
	e = BE_NET_ERR;
      }
      else{
	e = BE_NET_DAT_ERR;
      }
      continue;
    }
    else if(ret != initDat->size){
      uM3("beReqInit(); netclWrite(BeIp%02d); ret(%d) != initDat->size(%d) error!!", i + 1, ret, initDat->size);
      e = BE_NET_DAT_ERR;
      continue;
    }

    //! �X�e�[�^�X���Ԃ��Ă���̂œǂݍ��܂Ȃ��Ă͂Ȃ�Ȃ��B
    memset(&p.ans[i], 0, sizeof(*p.ans));
    unsigned char* buf = (unsigned char*)&p.ans[i];
    long size = 0;                                         //!< ���݂̎擾�T�C�Y
    long getSize = sizeof(long);
    int err = 1;

    for(int j = 0; j < 5; j++){
      ret = netclRead(p.net[i], buf + size, getSize - size);
      if(ret <= 0){
	//uM2("beReqInit(); netclRead(BeIp%02d); ret=%d error!!", i + 1, ret);
	err = 2;
	break;
      }
      if(j == 0){
	getSize = p.ans[i].size;
	//printf("p.ans.size=%d\n",p.ans[i].size);
      }
      size += ret;
      printf("debug size=%d\n", size);
      if(size >= getSize){
	err = 0;
	break;
      }
    }
    //uM1("debug beReqInit(); netclRead(); getSize=%d", getSize);
    if(err == 2){
      uM1("beReqInit(); netclRead(BeIp%02d); Network error!!", i + 1);
      e = BE_NET_ERR;
      continue;
    }
    else if(err == 1){
      uM2("beReqInit(); netclRead(BeIp%02d); timeout size=%ld error!!", i + 1, size);
      e = BE_NET_TIMEOUT_ERR;
      continue;
    }
    if(p.ans[i].endExeFlag != 4){
      uM2("beReqInit(); netclRead(BeIp%02d); endExeFlag = %d", i + 1, p.ans[i].endExeFlag);
      e = BE_REJECT;
      continue;
    }
    p.netAct[i] = 1;
  }
  delete[] initDat;
  return e;
}

/*! \fn int beReqStart(int OnOffRoad, int returnFlag, unsigned long startTime,
		  double* RADEC, double* LB, double* AZEL,
		  double* DRADEC, double* DLB, double* DAZEL,
		  double* PAZEL, double* RAZEL,
		  double PA, double FQIF1, double VRAD, double IFATT,
		  double* weath, double* wind, double TAU,
		  double BATM, double IPINT)
\brief �����v�ɐϕ��̊J�n�w���𑗂�B
\brief �p�x�̒P�ʂ͑S��[rad]
\param[in] OnOffRoad �ϑ��_�̎�ނ�ݒ肷��B 0:OnPoint 1:OffPoint 2:RSky
\param[in] returnFlag �\��
\param[in] startTime �ϑ��J�n����
\param[in] RADEC �}�b�v�Z���^�[��RADEC�ʒu�z��B[RA][DEC]�̏��Ɋi�[
\param[in] LB �}�b�v�Z���^�[��LB�ʒu�z��B[L][B]�̏��Ɋi�[
\param[in] AZEL �}�b�v�Z���^�[��AZEL�ʒu�z��B[AZ][EL]�̏��Ɋi�[
\param[in] DRADEC �}�b�v�Z���^�[����̃I�t�Z�b�g�l�B[RA][DEC]�̏��Ɋi�[
\param[in] DLB �}�b�v�Z���^�[����̃I�t�Z�b�g�l�B[L][B]�̏��Ɋi�[
\param[in] DAZEL �}�b�v�Z���^�[����̃I�t�Z�b�g�l�B[AZ][EL]�̏��Ɋi�[
\param[in] PAZEL �A���e�i�w���p�B[AZ][EL]�̏��Ɋi�[
\param[in] RAZEL �A���e�i���ۊp�B[AZ][EL]�̏��Ɋi�[
\param[in] PA �|�W�V�����A���O��(�ۗ�)
\param[in] FQIF1 1stIF�̃��[�J�����g��[GHz]
\param[in] VRAD �ϑ��n����̎����x[km/s]
\param[in] IFATT IF�̃X�e�b�v�A�b�e�l�[�^�[�̒l[dB]
\param[in] weath �C�ۃf�[�^�B[�C��][�C��][����C��]�̏�
\param[in] wind �C�ۃf�[�^(��)�B[����][����][�ő�u�ԕ���]�̏�
\param[in] TAU ��C�̌���(�ۗ�)
\param[in] BATM ��C�̉��x[K]
\param[in] IPINT ���x�r�����u�̉��x[K]
\retval 0 ����
\retval 0�ȊO �G���[�R�[�h
*/
int beReqStart(int OnOffRoad, int returnFlag, unsigned long startTime,
		  double* RADEC, double* LB, double* AZEL,
		  double* DRADEC, double* DLB, double* DAZEL,
		  double* PAZEL, double* RAZEL,
		  double PA, double FQIF1, double VRAD, double IFATT,
		  double* weath, double* wind, double TAU,
		  double BATM, double IPINT){

  int ret;
  //time_t now;
  int e = 0;

  if(init == 0){
    return BE_NOT_INIT_ERR;
  }

  for(int i = 0; i < p.BeNum; i++){
    if(p.BeUse[i] != 1){
      continue;
    }
    memset(&p.req[i], 0, sizeof(*p.req));
    p.req[i].size = sizeof(*p.req);
    p.req[i].exeFlag = 1;                   //!< 1:Start
    p.req[i].OnOffRoad = OnOffRoad;
    //p.req[i].returnFlag = returnFlag;       //!< (�\��)
    //time(&now);
    p.req[i].nowTime = (unsigned long)startTime;
    p.req[i].DRA = DRADEC[0];
    p.req[i].DDEC = DRADEC[1];
    p.req[i].DGL = DLB[0];
    p.req[i].DGB = DLB[1];
    p.req[i].DAZ = DAZEL[0];
    p.req[i].DEL = DAZEL[1];
    //if(p.iaoflg == 1){
    p.req[i].RA = RADEC[0];
    p.req[i].DEC = RADEC[1];
    //}
    //else if(p.iaoflg == 2){
    p.req[i].GL = LB[0];
    p.req[i].GB = LB[1];
    //}
    //else if(p.iaoflg == 3){
    p.req[i].AZ = AZEL[0];
    p.req[i].EL = AZEL[1];
    //}
    p.req[i].PAZ = PAZEL[0];
    p.req[i].PEL = PAZEL[1];
    p.req[i].RAZ = RAZEL[0];
    p.req[i].REL = RAZEL[1];
    p.req[i].PA = PA;                        //!< �|�W�V�����A���O��(�ۗ�)
    p.req[i].FQIF1 = FQIF1;
    p.req[i].VRAD = VRAD;
    p.req[i].IFATT = (int)IFATT;
    p.req[i].TEMP = weath[0];                //!< ���x [K]
    p.req[i].PATM = weath[1];                //!< ��C�� [hPa]
    p.req[i].PH2O = weath[2];                //!< �����C�� [hPa]
    p.req[i].VWIND = wind[1];                //!< ����
    p.req[i].DWIND = wind[0];                //!< ����
    p.req[i].TAU = TAU;                      //!< (�ۗ�)
    p.req[i].BATM = BATM;
    p.req[i].IPINT = IPINT;
    
    ret = myWrite(i, (const unsigned char*)&p.req[i], p.req[i].size);
    //uM1("beReqStart(); netclWrite(); ret=%d", ret);
    if(ret <= 0 || ret != p.req[i].size){
      uM3("beReqStart(); myWrite(BeIp%02d); [%d(%s)] error!!", i + 1, ret, errnoStr(ret));
      if(ret <= 0){
	e = BE_NET_ERR;
      }
      else{
	e = BE_NET_DAT_ERR;
      }
      continue;
      //return ret;
    }

    //! �X�e�[�^�X���Ԃ��Ă���̂œǂݍ��܂Ȃ��Ă͂Ȃ�Ȃ��B
    memset(&p.ans[i], 0, sizeof(*p.ans));
    unsigned char* buf = (unsigned char*)&p.ans[i];
    long size = 0;                      //!< ���݂̎擾�T�C�Y
    long getSize = sizeof(long);
    int err = 1;
    for(int j = 0; j < 5; j++){
      ret = myRead(i, buf + size, getSize - size);
      if(ret <= 0){
	uM3("beReqStart(); myRead(BeIp%02d); [%d(%s)]", i + 1, ret, errnoStr(ret));
	err = 2;
	break;
	//continue;
	//return ret;
      }
      if(j == 0){
	getSize = p.ans[i].size;
	//printf("p.ans.size=%d\n",p.ans[i].size);
      }
      size += ret;
      //printf("debug size=%d\n", size);
      if(size >= getSize){
	err = 0;
	break;
      }
    }
    //uM1("debug beReqState(); netclRead(); getSize=%d", getSize);
    if(err == 2){
      continue;
    }
    else if(err == 1){
      uM2("beReqStart(); BeIp%02d timeout size=%ld", i + 1, size);
      e = BE_NET_TIMEOUT_ERR;
      continue;
      //return BE_NET_TIMEOUT_ERR;
    }
    if(p.ans[i].endExeFlag != 4){
      uM2("beReqStart(); BeIp%02d endExeFlag = %d", i + 1, p.ans[i].endExeFlag);
      e = BE_REJECT;
      continue;
      //return BE_REJECT;
    }
  }
  return e;
}

/*! \fn int beReqStop()
\brief �����v�̏����𐳏�I��������B
\brief �ϕ��r���̏ꍇ�ł��I��������B
\retval 0 ����
\retval 0�ȊO �G���[�R�[�h
*/
int beReqStop(){
  int ret;
  tReqBeTmp r = {0};
  int e = 0;

  if(init == 0){
    return BE_NOT_INIT_ERR;
  }

  for(int i = 0; i < p.BeNum; i++){
    if(p.BeUse[i] != 1){
      continue;
    }
    r.size = sizeof(r);
    r.exeFlag = 0; //!< request stop
    ret = myWrite(i, (const unsigned char*)&r, r.size);
    if(ret <= 0 || ret != r.size){
      uM2("beReqStop(); netcl(BeIp%02d); return %d error!!", i + 1, ret);
      if(ret <= 0){
	e = BE_NET_ERR;
      }
      else{
	e = BE_NET_DAT_ERR;
      }
      //return ret;
      continue;
    }

    //! �X�e�[�^�X���Ԃ��Ă���̂œǂݍ��܂Ȃ��Ă͂Ȃ�Ȃ��B
    memset(&p.ans[i], 0, sizeof(*p.ans));
    unsigned char* buf = (unsigned char*)&p.ans[i];
    long size = 0;                         //!< ���݂̎擾�T�C�Y
    long getSize = sizeof(long);
    int err = 1;
    for(int j = 0; j < 5; j++){
      ret = myRead(i, buf + size, getSize - size);
      if(ret <= 0){
	uM2("beReqStop(); netclRead(BeIp%02d); return %d error!!", i + 1, ret);
	e = BE_NET_DAT_ERR;
	err = 2;
	break;
	//return ret;
      }
      if(j == 0){
	getSize = p.ans[i].size;
	//printf("p.ans.size=%d\n",p.ans.size);
      }
      size += ret;
      //printf("debug size=%d\n", size);
      if(size >= getSize){
	err = 0;
	break;
      }
    }
    //uM1("debug beReqStop(); netclRead(); getSize=%d", getSize);
    if(err == 2){
      continue;
    }
    else if(err == 1){
      uM2("beReqStop(); myRead(BeIp%02d); timeout size=%ld", i + 1, size);
      e = BE_NET_TIMEOUT_ERR;
      continue;
      //return BE_NET_TIMEOUT_ERR;
    }
    if(p.ans[i].endExeFlag != 4){
      uM2("beReqStop(); myRead(BeIp%02d); endExeFlag = %d", i + 1, p.ans[i].endExeFlag);
      e = BE_REJECT;
      continue;
      //return BE_REJECT;
    }
  }
  return e;
}

/*! \fn int beReqState()
\breif �����v�փX�e�[�^�X���N�G�X�g�𑗂�B
\retval 0 ����
\retval 0�ȊO �G���[�R�[�h
*/
int beReqState(){
  int ret;
  tReqBeTmp r;
  int e = 0;

  if(init == 0){
    return BE_NOT_INIT_ERR;
  }

  if(tmGetLag(p.vtm) < p.BeInterval)
    return 0;
  tmReset(p.vtm);

  r.size = sizeof(r);
  r.exeFlag = 2;

  for(int i = 0; i < p.BeNum; i++){
    if(p.BeUse[i] != 1){
      continue;
    }
    ret = myWrite(i, (const unsigned char*)&r, r.size);
    //uM1("debug beReqState(); netclWrite(); ret=%d", ret);
    if(ret <= 0 || ret != r.size){
      uM2("beReqState(); myWrite(BeIp%02d); return %d error!!", i + 1, ret);
      if(ret <= 0){
	e = BE_NET_ERR;
      }
      else{
	e = BE_NET_DAT_ERR;
      }
      continue;
      //return ret;
    }

    //! �X�e�[�^�X���Ԃ��Ă���̂œǂݍ��܂Ȃ��Ă͂Ȃ�Ȃ��B
    memset(&p.ans[i], 0, sizeof(*p.ans));
    unsigned char* buf = (unsigned char*)&p.ans[i];
    long size = 0;                             //!< ���݂̎擾�T�C�Y
    long getSize = sizeof(long);
    int err = 1;
    for(int j = 0; j < 5; j++){
      ret = myRead(i, buf + size, getSize - size);
      if(ret <= 0){
	uM2("beReqState(); netclRead(BeIp%02d); ret=%d error!!", i + 1, ret);
	memset(&p.ans[i], 0, sizeof(*p.ans));
	e = BE_NET_ERR;
	err = 2;
	break;
	//return ret;
      }
      if(j == 0){
	getSize = p.ans[i].size;
	//printf("p.ans.size=%d\n",p.ans.size);
      }
      size += ret;
      //printf("debug size=%d\n", size);
      if(size >= getSize){
	err = 0;
	break;
      }
    }
    //uM1("debug beReqState(); netclRead(); getSize=%d", getSize);
    if(err == 2){
      continue;
    }
    else if(err == 1){
      uM2("beReqState(); netclRead(BeIp%02d); timeout size=%ld", i + 1, size);
      memset(&p.ans[i], 0, sizeof(*p.ans));
      e = BE_NET_TIMEOUT_ERR;
      continue;
      //return BE_NET_TIMEOUT_ERR;
    }
  }
  return e;
}

/*! \fn int beGetAns(int* endExeFlag, int* acqErr, double* tsys, double* totPow)
\brief �����v�̃X�e�[�^�X�����擾����
\param[in] beId �o�b�N�G���h�̔ԍ�
\param[out] endExeFlag ���s��� -1:FAILED 1:SUCCESS 2:STANDBY 3:EXECUTION 4:ACCEPT 5:REJECT
\param[out] acqErr �G���[�l 1:NORMAL 2:ADCovfl 3:ACCovfl 4:ADACCovfl
\param[out] tsys RSky�œ���ꂽ�V�X�e���G�����x[K]
\param[out] totPow On�_�ϑ��œ���ꂽ�g�[�^���p���[[W/Hz]
\retval 0 ����
*/
int beGetAns(int beId, int* endExeFlag, int* acqErr, double* tsys, double* totPow){

  if(init == 0){
    return BE_NOT_INIT_ERR;
  }
  if(p.BeUse[beId] != 1){
    *endExeFlag = 0;
    *acqErr = 0;
    *tsys = 0;
    *totPow = 0;
    return BE_NOT_USE;
  }
  *endExeFlag = p.ans[beId].endExeFlag;
  *acqErr = p.ans[beId].acqErr;
  *tsys = p.ans[beId].tsys;
  *totPow = p.ans[beId].totalPower;
  return 0;
}

/*! \fn int myRead(void* net, unsigned char* buf, long bufSize)
\brief �l�b�g���[�N�o�R�ł̃f�[�^�̎�M���s��
\param[in] beId �ڑ���̔ԍ�
\param[in] buf �f�[�^�̊i�[�̈�̐擪�|�C���^�[
\param[in] bufSize buf�̍ő�T�C�Y[Byte]
\retval 1�ȏ� ��M�o�C�g��
\retval 0�@�G���[
\retval -1�ȉ� �G���[�R�[�h
*/
int myRead(int beId,  unsigned char* buf, long bufSize){
  int ret;

  if(p.netAct[beId] == 0){
    ret = beReqInit();
    if(ret){
      //!error
      return ret;
    }
    //else{
    //  p.netAct[beId] = 1;
    //}
  }
  ret = netclRead(p.net[beId], buf, bufSize);
  if(ret <= 0){
    p.netAct[beId] = 0;
    return BE_NET_ERR;
  }
  //else if(ret == 0){
  //  return 0;
  //}
  return ret;
}

/*! \fn int myWrite(void* net, const unsigned char* buf, long bufSize)
\brief �l�b�g���[�N�o�R�ł̃f�[�^�̑��M���s��
\brief �ʐM���ؒf���Ă����ꍇ�͕����v�̏��������s���āA�ʐM���p������B
\param[in] beId �ڑ���̔ԍ�
\param[in] buf ���M����f�[�^�̊i�[�̈�̐擪�|�C���^�[
\param[in] bufSize ���M�f�[�^�̃T�C�Y
\retval 1�ȏ� ���M�o�C�g��
\retval 0�@�G���[
\retval -1�ȉ� �G���[�R�[�h
*/
int myWrite(int beId, const unsigned char* buf, long bufSize){
  int ret;

  if(p.netAct[beId] == 0){
    ret = beReqInit();
    if(ret){
      //! error
      return ret;
    }
    //else{
    //  p.netAct[beId] = 1;
    //}
  }
  ret = netclWrite(p.net[beId], buf, bufSize);
  if(ret <= 0){
    p.netAct[beId] = 0;
    return BE_NET_ERR;
  }
  //else if(ret == 0){
  //  return 0;
  //}
  return ret;
}

/*! \fn void setParam()
\brief �p�����[�^�[�����擾����
*/
void setParam(){
  char tmp[256];

  if(confSetKey("BeNum"))
    p.BeNum = atoi(confGetVal());
  if(p.BeNum > 0){
    int n = p.BeNum;
    p.BeUse = new int[n];
    p.BeIp = new const char*[n];
    p.BePort = new int[n];
    memset(p.BeUse, 0, sizeof(*p.BeUse) * n);
    memset(p.BeIp, 0, sizeof(*p.BeIp) * n);
    memset(p.BePort, 0, sizeof(*p.BePort) * n);
    for(int i = 0; i < n; i++){
      sprintf(tmp, "BeUse%02d", i + 1);
      if(confSetKey(tmp)){
	p.BeUse[i] = atoi(confGetVal());
      }
      sprintf(tmp, "BeIp%02d", i + 1);
      if(confSetKey(tmp)){
	p.BeIp[i] = confGetVal();
      }
      sprintf(tmp, "BePort%02d", i + 1);
      if(confSetKey(tmp)){
	p.BePort[i] = atoi(confGetVal());
      }
    }
  }
  //if(confSetKey("useBe"))
  //  p.use = atoi(confGetVal());
  //if(confSetKey("Hardware"))
  //  p.use = atoi(confGetVal());
  //if(confSetKey("beip"))
  //  strcpy(p.beip, confGetVal());
  //if(confSetKey("beport"))
  //  p.beport=atoi(confGetVal());
  if(confSetKey("BeInterval"))
    p.BeInterval=atof(confGetVal());
  //if(confSetKey("Coordinate"))
  //  p.iaoflg=atoi(confGetVal());
  //if(confSetKey("X_Rad"))
  //  p.dsourc[0] = atof(confGetVal());
  //if(confSetKey("Y_Rad"))
  //p.dsourc[1] = atof(confGetVal());
}

/*! \fn int checkParam()
\brief �p�����[�^�[�̃`�F�b�N
\retval 0 ����
\retval -1 ���s
*/
int checkParam(){
  char tmp[256];

  if(p.BeNum < 0){
    uM1("BeNum(%d) error!!", p.BeNum);
    return -1;
  }
  for(int i = 0; i < p.BeNum; i++){
    if(p.BeUse[i] < 0 || p.BeUse[i] > 2){
      uM2("BeUse%02d(%d) error!!", i + 1, p.BeUse[i]);
      return -1;
    }
    if(p.BeUse[i] == 1){
      sprintf(tmp, "BeIp%02d", i + 1);
      if(!confSetKey(tmp)){
	uM1("BeIp%02d(NULL) error!!", i + 1);
	return -1;
      }
      if(p.BePort[i] <= 0){
	uM2("BePort%02d(%d) error!!", i + 1, p.BePort[i]);
	return -1;
      }
    }
    if(p.BeInterval < 0){
      uM1("BeInterval(%lf) error!!", p.BeInterval);
      return -1;
    }
  }

  return 0;
}

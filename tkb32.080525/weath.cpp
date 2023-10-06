/*!
\file weath.cpp
\author Y.Koide
\date 2006.12.12
\brief �C�ۃf�[�^���擾����
*/
#include <stdlib.h>
#include <memory.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <sys/types.h>
#include <pthread.h>

#include "../libtkb/export/libtkb.h"
#include "errno.h"
#include "weath.h"

const double ABS_ZERO = 273.15;

typedef struct sWeathDat{
  int    direct;        //!< ���� 16
  double windVel;       //!< ���� m/s
  double windVelMax;    //!< �ő�u�ԕ��� m/s
  double temp;          //!< �C�� K
  double water;         //!< �����C�� hPa
  double press;         //!< �C�� hPa
}tWeathDat;

typedef struct sParamWeath{
  int WeathUse;
  int WeathCom;         //!<  �C�ۊϑ����u�ɐڑ�����RS-232C�|�[�g�ԍ�
  double WeathInterval; //!<  �C�ێ擾���s���Ԋu sec
  tWeathDat dat;        //!<  �C�ۃf�[�^�i�[�p

  void* vtm;
  int thrdRet;          //!<  �X���b�h�̖߂�l
  int thrdRun;          //!<  �X���b�h�̏�� 1:���s 0:��~
  pthread_t thrdID;     //!<  �X���b�h��ID
}tParamWeath;

static int init=0;
static tParamWeath p;
static int comAct=0;    //!< RS-232C�̏�� 0:���ڑ� 1:�ڑ�

static void* weathThrdMain(void* _p);
static double weathCalWater(const double T, const double moisture);
static void setParam();
static int checkParam();

/*! \fn int weathInit()
\brief ������
\retval 0 ����
\retval 0�ȊO �G���[�R�[�h
*/
int weathInit(){
  
  if(init){
    weathEnd();
    init = 0;
  }
  memset(&p, 0, sizeof(p));
  setParam();
  if(checkParam()){
    uM("weathInit(); checkParam(); error");
    return WEATH_PARAM_ERR;
  }
  //p.dat.temp = 273;
  //p.dat.water = 5;
  //p.dat.press = 1024;

  init = 1;
  if(p.WeathUse == 0)
    return WEATH_NOT_USE;

  //! ������
  comAct = 0;
  p.thrdRun = 0;
  p.vtm = tmInit();
  if(p.WeathUse == 1){
    if(rs232cInit(p.WeathCom, 1200, 8, 1, 2) == 0){
      uM1("weathInit(); RS-232C port%d open", p.WeathCom);
      comAct = 1;
    }
    else{
      uM1("weathInit(); RS-232C port %d open error", p.WeathCom);
      return WEATH_COM_ERR;
    }
  }
  else{
    p.dat.temp =  280;
    p.dat.water = 10;
    p.dat.press = 1000;
    p.dat.direct = 10;
    p.dat.windVel = 1;
    p.dat.windVelMax = 5;
  }
  return 0;
}

/*! \fn int weathEnd()
\brief �I�����ɌĂ�
\retval 0 ����
\retval 0�ȊO �G���[�R�[�h
*/
int weathEnd(){
  init = 0;
  if(p.thrdRun){
    p.thrdRun = 0;
    //tmSleepMSec(p.WeathInterval * 1000.0);
    pthread_join(p.thrdID, NULL);
    p.thrdID = NULL;
  }
  if(comAct == 1){
    rs232cEnd(p.WeathCom);
    comAct = 0;
  }
  if(p.vtm){
    tmEnd(p.vtm);
    p.vtm = NULL;
  }
  
  return 0;
}

/*! \fn int weathRepeat()
\brief �C�ۃf�[�^�̎擾����������ɍs���Ă��邩�m�F����B
\brief �X���b�h�����s����Ă��邩���m�F����B
\retval 0 ����
\retval 0�ȊO �G���[�R�[�h
*/
int weathRepeat(){
  if(p.WeathUse == 0)
    return WEATH_NOT_USE;
  if(p.thrdRun == 0){
    return WEATH_THRD_ERR;
  }
  return 0;
}

/*! \fn int weathStart()
\brief �C�ۃf�[�^�擾���J�n����B�X���b�h�𐶐�����
\retval 0 ����
\retval 0�ȊO �G���[�R�[�h
*/
int weathStart(){
  //void* ret;
  int state;

  if(p.WeathUse == 0){
    return WEATH_NOT_USE;
  }

  if(p.thrdRun){
    uM("weathStart(); Weath already started");
    //return WEATH_THRD_RAN_ERR;
    return 0;
  }
  state = pthread_create(&p.thrdID, NULL, weathThrdMain, (void*)NULL);
  if(state != 0){
    uM1("weathStart(); pthread_create(); state=%d error", state);
    return WEATH_THRD_ERR;
  }
  return 0;
}

/*! \fn void* weathThrdMain(void* _p)
\brief �C�ۃf�[�^�擾�����̃��C��������
\param[in] _p weath�̃p�����[�^�[�ւ̃|�C���^�B�X���b�h�������ɓn�����B
\retval NULL ����I��
*/
void* weathThrdMain(void* _p){
  int ret;

  if(p.WeathUse == 0)
    return NULL;
  
  p.thrdRun = 1;
  while(p.thrdRun){
    if(p.WeathUse == 1){
      if(!comAct){
	//! �C�ۊϑ����u
	//! bps:1200, start:1bit, data:8bit, parity:odd, stop:2bit
	if(rs232cInit(p.WeathCom, 1200, 8, 2, 2) == 0){
	  uM1("weathThrdMain(); RS-232C port%d open", p.WeathCom);
	  comAct = 1;
	}
      }
      if(comAct){
	ret = weathGetDat();
	if(ret){
	  printf("weathThrdMain(); weathGetDat(); return %d", ret);
	}
	if(!comAct){
	  rs232cEnd(p.WeathCom);
	}
      }
    }
    tmSleepMSec(p.WeathInterval * 1000.0);
  }
  p.thrdRun = 0;
  p.thrdRet = 0;
  return NULL;
}

//! �ϑ��f�[�^�̍\���́B�C�ەϊ��@���炱�̃t�H�[�}�b�g�Ńf�[�^������B
typedef struct sWeathDatTmp{
  unsigned char sl;       //!< '/'
  unsigned char d[2];     //!< ���� 16���� 00:(�J�[��)���� 16:N 04:E 08:S 12:W 15:NNW
  unsigned char v[4];     //!< ���� m/s
  unsigned char vmax[4];  //!< �ő�u�ԕ��� m/s
  unsigned char t[6];     //!< �C�� Degree/12 4.411.9  8.97 49 985.8
  unsigned char m[3];     //!< ���x %
  unsigned char p[6];     //!< �C�� hPa
  unsigned char cr;       //!< CR=0x0D
  unsigned char lf;       //!< LF=0x0A
}tWeathDatTmp;

/*! \fn int weathGetDat()
\brief �C�ەϊ��@����C�ۃf�[�^���擾����
\retval 0 ����I��
\retval 0�ȊO �G���[�R�[�h
*/
int weathGetDat(){
  const unsigned int bufSize = 128;
  unsigned char buf[128];
  tWeathDatTmp datTmp;
  int ret;
  int size = 0;
  int cnt = 0;

  memset(buf, 0, bufSize);
  while(size < sizeof(datTmp)){
    //! �C�ۊϑ��f�[�^�Ǎ���
    ret = rs232cRead(p.WeathCom, buf + size, bufSize - size);
    if(ret == 0){
      //! �X�V�f�[�^����
      cnt++;
      if(cnt > 2){
	return 0;
      }
    }
    else if(ret < 0){
      comAct=0;
      return WEATH_COM_ERR;
    }
    //else if(ret != sizeof(datTmp)){
    //  return WEATH_COM_DAT_ERR;
    //}
    size += ret;
    tmSleepMSec(100);
    //! debug
    memcpy(&datTmp, buf, sizeof(datTmp));
    //printf("weathGetDat();1 %s", (char*)buf);
  }
  memcpy(&datTmp, buf, sizeof(datTmp));
  //printf("weathGetDat();2 %s", (char*)buf);
  //if(datTmp.sl != '/' || datTmp.cr != 0x0D || datTmp.lf != 0x0A){
  if(datTmp.sl != '/'){
    //! �ُ�f�[�^
    //tmSleepMSec(10);
    ret = rs232cRead(p.WeathCom, buf, bufSize); //!< �c��̃f�[�^���o�b�t�@�������
    return WEATH_COM_DAT_ERR;
  }
  char tmp[128];
  double moi;
  memset(&p.dat, 0, sizeof(tWeathDat));
  memcpy(tmp, &datTmp.d, 2);
  tmp[2] = '\0';
  p.dat.direct = atoi(tmp);                       //!< ���� 16����
  memcpy(tmp, &datTmp.v, 4);
  tmp[4] = '\0';
  p.dat.windVel = atof(tmp);                      //!< ���� [m/s]
  memcpy(tmp, &datTmp.vmax, 4);
  tmp[4] = '\0';
  p.dat.windVelMax = atof(tmp);                   //!< �ő�u�ԕ��� [m/s]
  memcpy(tmp, &datTmp.t, 6);
  tmp[6] = '\0';
  p.dat.temp = atof(tmp);                         //!< ���x [��]
  memcpy(tmp, &datTmp.m, 3);
  tmp[3] = '\0';
  moi = atoi(tmp);                                //!< ���x [%]
  memcpy(tmp, &datTmp.p, 6);
  tmp[6] = '\0';
  p.dat.press = atof(tmp);                        //!< �C�� [hPa]

  p.dat.direct *= 360.0 / 16.0;                   //!< 360:N 90:E 180:S 270:W �Ƃ�����ʊp(�E���n) 0:�J�[��(�É���)
  p.dat.temp += ABS_ZERO;                         //!< ���x K
  p.dat.water = weathCalWater(p.dat.temp, moi);   //!< �����C�� hPa

  //sprintf(tmp, "Tamb=%lf Pamb=%lf Pwater=%lf WindD=%d WindAvg=%lf WindMax=%lf",
  //	  p.dat.temp, p.dat.press, p.dat.water, p.dat.direct, p.dat.windVel, p.dat.windVelMax);
  //uM1("%s", tmp);

  FILE* fp;
  time_t t;
  struct tm* tmt;

  time(&t);
  tmt = localtime(&t);
  sprintf(tmp, "../log/weath-%04d%02d%02d.log", tmt->tm_year+1900, tmt->tm_mon+1, tmt->tm_mday);
  fp = fopen(tmp, "a");
  if(fp){
    int len = sizeof(tWeathDatTmp);
    strncpy(tmp, (const char*)buf, len);
    tmp[len] = '\0';
    fprintf(fp, "\"%04d/%02d/%02d-%02d:%02d:%02d\", %s",
	    tmt->tm_year+1900, tmt->tm_mon+1, tmt->tm_mday, tmt->tm_hour, tmt->tm_min, tmt->tm_sec, tmp);
    fclose(fp);
  }
  return 0;
}

/*! \fn int weathGetWeath(double* weath)
\brief �C�ۃf�[�^�C���E�C���E����C�����擾����
\param[out] weath �f�[�^���i�[���邽�߂̔z��B[�C��][�C��][����C��]�̏�
\retval 0 ����
*/
int weathGetWeath(double* weath){
  weath[0] = p.dat.temp;     //!< �C�� [K]
  weath[1] = p.dat.press;    //!< �C�� [hPa]
  weath[2] = p.dat.water;    //!< �����C�� [hPa]
  return 0;
}
/*! int weathGetWind(double* wind)
\brief �C�ۃf�[�^�̕����E�����E�ő�u�ԕ������擾����
\param[out] wind �f�[�^���i�[���邽�߂̔z��B[����][����][�ő�u�ԕ���]�̏�
\retval 0 ����
*/
int weathGetWind(double* wind){
  wind[0] = p.dat.direct;     //!< ���� 16����
  wind[1] = p.dat.windVel;    //!< ���� [m/s]
  wind[2] = p.dat.windVelMax; //!< �ő�u�ԕ��� [m/s]
  return 0;
}

/*! \fn double weathCalWater(double T, double moisture)
\brief ���x�Ɖ��x���琅���C�����Z�o����B
\brief �v�Z�ɂ̓��O�i�[�̎���p���Ă���B
\brief �����ł̊��Z��273�`313K�܂ł����T�|�[�g���Ă��Ȃ��̂Œቷ���Ŏg�p����ꍇ�͒��ӂ��K�v�B
\param[in] T �C�� [K]
\param[in] moisture ���x [%]
\return �����C�� [hPa]
*/
double weathCalWater(const double T, const double moisture){
  const double A = -7.76451;
  const double B = 1.45838;
  const double C = -2.7758;
  const double D = -1.23303;
  const double Tc = 647.3; //!< K
  const double Pc = 22120; //!< kPa(�g�p�͈�275�`647.3 K�A273�`313 K�ɂ�����0.1hPa�ȉ��̌덷)
  double tau = 1.0-T/Tc;
  double Pw;
  double tmp;

  tmp = (A * tau + B * pow(tau, 1.5) + C * pow(tau, 3) + D * pow(tau, 6)) / (T / Tc);
  Pw = pow(M_E, tmp) * Pc; //!< [kPa]
  Pw *= 10;                //!< [hPa]
  Pw *= moisture / 100.0;  //!< Pwater [hPa]
  return Pw;
}

/*! \fn void setParam()
\brief �p�����[�^�[�����擾����
*/
void setParam(){
  if(confSetKey("WeathUse"))
    p.WeathUse = atoi(confGetVal());
  if(confSetKey("WeathInterval"))
    p.WeathInterval = atof(confGetVal());
  if(confSetKey("WeathCom"))
    p.WeathCom = atoi(confGetVal());
}

/*! \fn int checkParam()
\brief �p�����[�^�[�̃`�F�b�N
\retval 0 ����
\retval -1 ���s
*/
int checkParam(){
  if(p.WeathUse < 0 || p.WeathUse > 2){
    uM1("WeathUse(%d) error!!", p.WeathUse);
    return -1;
  }
  if(p.WeathUse == 1 || p.WeathUse == 2){
    if(p.WeathInterval <= 0){
      uM2("WeathUse(%d) WeathInterval(%d) error!!", p.WeathUse, p.WeathInterval);
      return -1;
    }
    if(p.WeathUse == 1){
      if(p.WeathCom < 0){
	uM2("WeathUse(%d) WeathCom(%d) error!!", p.WeathUse, p.WeathCom);
	return -1;
      }
    }
  }
  return 0;
}

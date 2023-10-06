/*!
\file trk.cpp
\author Y.Koide
\date 2006.11.20
\brief Tracking Program
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>
#include <sys/types.h>
#include <pthread.h>

//#include<unistd.h>//update080529

#include"thread.h"

#include "../libtkb/export/libtkb.h"
#include "trk45Sub.h"
#include "errno.h"
#include "trk.h"

//#define PI  3.14159265358979323846
#define PI M_PI

#define STX 0x02
#define ETX 0x03
#define EL_DRIVE_DISABLE 0x38
#define AZ_DRIVE_DISABLE 0x34
#define EL_DRIVE_ENABLE 0x32
#define AZ_DRIVE_ENABLE 0x31
#define TRACK_MODE_FAULT 0x38
#define STANDBY 0x34
#define PROGRAM_TRACK_MODE 0x32
#define REMOTE_CONTROL_MODE 0x38
#define LOCAL_CONTROL_MODE 0x34
#define STOW 0x32
#define TOTAL_ALARM 0x34
#define ANGLE_FAULT 0x34
#define AZ_DCPA_FAULT 0x32
#define EL_DCPA_FAULT 0x31

const double ABS_ZERO = 273.15;
//const int mapNumMax = 20;
const double sec2rad = PI / (180.0 * 3600.0);
const double rad2sec = 180.0 * 3600.0 / PI;

/*enum eAcuCmd {STX=0x02, ETX=0x03};
enum eAcuS1 {EL_DRIVE_DISABLE=0x38, AZ_DRIVE_DISABLE=0x34, EL_DRIVE_ENABLE=0x32, AZ_DRIVE_ENABLE=0x31};
enum eAcuS2 {TRACK_MODE_FAULT=0x38, STANDBY=0x34, PROGRAM_TRACK_MODE=0x32};
enum eAcuS3 {REMOTE_CONTROL_MODE=0x38, LOCAL_CONTROL_MODE=0x34, STOW=0x32};
enum eAcuS4 {TOTAL_ALARM=0x34};
enum eAcuS5 {ANGLE_FAULT=0x34, AZ_DCPA_FAULT=0x32, EL_DCPA_FAULT=0x31};
*/

//! ACU�̏�Ԃ��i�[���Ă����\����
typedef struct sAcu{
  int zone;             //!< �A���e�i�݂̍�]�[�� 1:CW 2:CCW
  double dazel[2];      //!< �A���e�i��AZEL�̌��ݒl [deg]
  int s[5];
}tAcu;

//! trk�̃p�����[�^�[�\����
typedef struct sParamTrk{
  //! �ڑ��p�����[�^
  int TrkUse;           //!< trk���g�p���邩 0:�g�p���Ȃ�, 1:�g�p, 2:�ʐM�ȊO�g�p
  int TrkCom;           //!< RS-232C�|�[�g
  //! �ϐ�
  void* vtm;            //!< �X�V���ԊǗ��p

  //! �ϑ��p�����[�^
  double TrkInterval;   //!< �ǔ��������������Ԋu sec
  int OnCoord;          //!< On�_�̍��W��` 1:RADEC 2:LB 3:AZEL
  int OffCoord;         //!< Off�_�̍��W��` 1:RADEC 2:LB 3:AZEL
  int OffNumber;     //!< Off�_��2�_�g���� 1:NoUse 2:Use
  int ScanFlag;         //!< �X�L�����̒�` 1:OnOff 2:5Points 3:Grid 4:Random 5:9Point 6:Raster 1001:SourceTracking 1002:Lee Tracking
  int OffInterval;      //!< One-Point�A�O���b�h�A�����_���}�b�s���O��On�_����Ɉ��Off�_�����邩
  int OffMode;          //!< 1:OffsetFromCenter 2:AbsoluteValue
  char* SetPattern;     //!< �}�b�s���O�̊ϑ�����
  int OnNumber;         //!< �}�b�s���O�̗v�f�̐�
  double PosAngle_Rad;  //!< �|�W�V�����A���O�� [rad]
  double LineTime;      //!< ���X�^�[�̃A�v���[�`���Ԃ��l������1Line�̎��� [sec]
  //double LinePathX_Rad;  //!< ���X�^�[�̃A�v���[�`���Ԃ��l������1Line�̋���X [rad]
  //double LinePathY_Rad; //!< ���X�^�[�̃A�v���[�`���Ԃ��l������1Line�̋���Y [rad]
  double AntAzElMarg[2];//!< �A���e�i�̋��e�덷[deg]
  int AntAzElMargThrsd; //!< ���̉񐔁AAZEL�����e�덷���ɘA���Ŏ��܂�ƒǔ��Ɣ��肷��
  const char* AntInst[7];//!< �A���e�i�퍷�␳�p�����[�^������
  double TrkTimeOffset; //!< �ǔ��v�Z���s���Ƃ��ɉ��b��̌v�Z���s�����Z�b�g���� �P��sec
  
  //! �ǔ����C�u�����p�p�����[�^
  int ioflg;            //!< �V�̃t���O 0:���z�n�O 1-10:�f�� 11:���z 12:COMET 99:AZEL
  int iaoflg;           //!< �ʒu���W�n 0:�f�� 1:RADEC 2:LB 3:AZEL
  int ibjflg;           //!< ���_�t���O 1:B1950 2:J2000
  double drdlv;         //!< �V�̂̎������x [m/s]
  double dsourc[2];     //!< �V�̈ʒu [rad]
  double dazelmin[2];   //!< �A���e�i�̍ŏ��쓮���E [deg]
  double dazelmax[2];   //!< �A���e�i�̍ő�쓮���E [deg]

  //! �e��ϐ�
  XTRKI xin;            //!< 45m�ǔ����C�u�������̓p�����[�^�[�\����
  double sof[2];        //!< ���݂̃X�L�����I�t�Z�b�g�l(�|�W�V�����A���O���ɂ��␳�����̒l) [rad]
  int sofCoord;         //!< ���݂̃X�L�����I�t�Z�b�g�̍��W�n 1:RADEC 2:LB 3:AZEL
  double* off;          //!< Off�_�̈ʒu offX1 offY1 offX2 offY2 ... [sec]
  double* on;           //!< �}�b�s���O�̍ۂ�On�_ onX1 onY1 onX2 onY2 ... [sec]
  double* linePath;     //!< ���X�^�[�̍ۂ�on����̃p�X
  int* mapPattern;      //!< �}�b�s���O�̊ϑ����� 0:R -:Off +:On 
  int mapNum;           //!< �}�b�s���O�̓_��
  char cstart[24];      //!< �ϑ������̐ݒ� YYYYMMDDhhmmss.0 (JST)
  double dweath[3];     //!< �C�� [��], �C�� [hPa] �����C�� [hPa]
  //int pofFlg;           //!< Position offset ���W��` 0:���p 1:���p
  int pofCoord;         //!< Position offset coord �ꎞ�i�[�p�B5�_�@�A9�_�@�Ŏg�p
  double pof[2];        //!< Position offset �ꎞ�i�[�p�B5�_�@�A9�_�@�Ŏg�p
  int rastStart;        //!< ���X�^�[�J�n�t���O 0:none 1:RasterStart
  time_t rastStartTime;//!< ���X�^�[�J�n����
  double* rastSof;      //!< ���X�^�[��1�X�L������TrkInterval���Ƃ̃X�L�����I�t�Z�b�g�i�[�p [rad]
  int rastSofSize;      //!< trkSof�ɓo�^����Ă���X�L�����ʒu�̐�
  int rastSofNo;        //!< �����Ԗڂ̃��X�^�[�X�L�����ʒu�����Ă��邩
  double P[30];         //!< �A���e�i�퍷�␳�p�����[�^

  XTRKO xout;           //!< 45m�ǔ����C�u�����v�Z���ʏo�͗p�\����
  double doazel[2];     //!< �v�Z����AZEL [rad]
  double doazel2[2];    //!< �v�Z���ʂɊ퍷�␳���������l [deg]
  double doazelC[2];    //!< ACU�֏o�͗p��AZEL [deg]
  double deazel[2];     //!< dazel-doazel2 [deg]
  double dgtdat[8][2];  //!< MapCenter(RADEC)(LB) Obs(RADEC)(LB) [rad] 
  tAcu acuStat;         //!< ACU���݂̃X�e�[�^�X
  int acuStatSBefore[5]; //!< ��O�Ɏ擾����ACU�̃X�e�[�^�X�R�[�h

  pthread_t thrdStatusID;//!< �X�e�[�^�X�擾�X���b�hID
  pthread_t thrdID;     //!< �X���b�hID
  int thrdRet;          //!< �X���b�h�̖߂�l
  int thrdRun;          //!< �X���b�h���s�� 1:���s 0:��~
  int thrdStatusRun;
  int antZoneType;      //!< �A���e�i��Zone�̃^�C�v1~5�܂Œ�`
  int antZone;          //!< Zone(CW,CCW�̂ǂ���̗̈�ő��点�邩) 0:�ړ��������Z���� 1:CW 2:CCW
  int trackStatCnt;     //!< �A���e�i�ǔ���Ԃ̃J�E���g�p
  int trackStat;        //!< �A���e�i�ǔ���ԁB�r�b�g�t���O�ɂ���ĕ\�� 0:Tracking -1:az�p��Swing -2:el�p��Swing -4:Zone�ړ����܂�Swing
  FILE* fp;             //!< �퍷�␳�f�[�^���W�p
  int dayFlg;           //!< trk_00()���Ăяo���^�C�~���O�p�BUT�ɂ�������t���ς�����Ƃ��ĂԁB
  int sofNo;            //!< scan offset�̉��Ԗڂ��������Ă��邩
  int offNo;            //!< ���Ԃ�Off�_���������Ă��邩 0 or 1
  int offFlg;           //!< ���O��Off�_�ϑ������Ă��邩 0:No 1:Yes
  unsigned int lst;     //!< LST�����b�݂̂�b���Z��������
}tParamTrk;

static int init=0;
static tParamTrk p;
static int comAct=0;    //!< RS-232C�̏��. 1:�g�p��, 0:�g�p�s��
//static double sps;     //!< Second Per Step 1Step��������̂ɂ����鎞�� [sec]

static void* trkThrdStatus(void* _p);
static void* trkThrdMain(void* _p);
static int trkCal();
static void trkCalInst(double X, double Y, double* dX, double* dY);
static void trkCalTrackStatus(const int reset);
static int trkSetACU();
static int trkSafetyCheck();
static void trkCalZoneType(double az);
static void trkCalZoneTypeInit();
static void setParam();
static int checkParam();

/*! \fn int trkInit()
\brief �������v���Z�X�ł��̃��W���[�����g�p����ۂɕK���ŏ��ɌĂԁB
\brief �X�e�[�^�X�̎擾�݂̂̏ꍇ�͍Œ���̃p�����[�^������΂悢�B
\brief �܂��A�p�����[�^�[��ݒ肵�Ȃ��������Ƃ��͈�xtrkEnd()���s���Ă���trkInit()����
\return 0 ����
*/
int trkInit(){
  int ret;

  if(init){
    trkEnd();
    init=0;
  }
  memset(&p, 0, sizeof(p));
  comAct = 0;
  p.pofCoord = 3; //!< �|�C���e�B���O�I�t�Z�b�g���W�n�̏����lAZEL

  //! �ڑ��p�����[�^�擾  
  if(confSetKey("TrkUse"))
    p.TrkUse = atoi(confGetVal());
  if(confSetKey("TrkCom"))
    p.TrkCom = atoi(confGetVal());

  //! �ڑ��p�����[�^�`�F�b�N
  if(p.TrkUse < 0 || p.TrkUse > 2){
    uM1("TrkUse(%d) error!!\n", p.TrkUse);
    return -1;
  }
  if(p.TrkUse == 1){
    if(p.TrkCom == 0){ //!< �v���X�l��ttyUSB���g�p���邱�Ƃ��Ӗ�����B�}�C�i�X�l��ttyS���g�p
      uM2("TrkUse(%d) TrkCom(%d) error!!\n", p.TrkUse, p.TrkCom);
      return -1;
    }
  }

  init = 1;
  if(p.TrkUse==0)
    return TRK_NOT_USE;

  //! Initialize timer module
  p.vtm=tmInit();

  //! Initialize ACU
  if(p.TrkUse==1){
    ret = trkGetACU();
    if(ret){
      uM1("trkInit(); trkGetACU(); ret = %d error!!",ret);
      return ret;
    }
    ret = trkSafetyCheck();
    if(ret){
      uM("trkInit(); trkSafetyCheck(); error");
      //return ret;//!< �g�[�^���A���[���Ɉ������邽�߈ꎞ�I�ɃR�����g�A�E�g
    }
  }
  //if((p.fp=fopen("kisa.cvs","a")) == NULL){
  //  uM("fileopen error");
  //}
  //fprintf(p.fp, "PAZ,PEL,PAZ+,PEL+,RAZ,REL");

  //! �X�e�[�^�X�擾�J�n
  int status = pthread_create(&p.thrdStatusID, NULL, trkThrdStatus, NULL);
  if(status != 0){
    uM1("trkInit(); pthread_create(trkTrhdStatus); status=%d error", status);
    return TRK_THRD_ERR;
  }

  return 0;
}

/*! \fn int trkInitParam()
\brief �ϑ��p�����[�^���Z�b�g���A�V�̂̏����ʒu���Z�o����B
\return 0:Success other:error
*/
int trkInitParam(){
  char cerr[256];
  int ret;

  if(p.TrkUse==0)
    return TRK_NOT_USE;

  if(p.thrdRun){
    uM("trkInitParam(); Tracking already started");
    return 0;
  }

  //! �ϑ��p�����[�^������
  p.TrkInterval = 0;
  //memset(p.antPosLaLo, 0, sizeof(p.antPosLaLo[0]) * 2);
  p.OnCoord = 0;
  p.OffCoord = 0;
  p.OffNumber = 0;
  p.ScanFlag = 0;
  p.OffInterval = 0;
  p.OffMode = 0;
  if(p.SetPattern){
    free(p.SetPattern);
    p.SetPattern = NULL;
  }
  if(p.mapPattern){
    free(p.mapPattern);
    p.mapPattern = NULL;
  }
  p.OnNumber = 0;
  p.PosAngle_Rad = 0;
  p.LineTime = 0;
  //p.LinePathX_Rad = 0;
  //p.LinePathY_Rad = 0;
  memset(p.AntAzElMarg, 0, sizeof(p.AntAzElMarg[0]) * 2);
  p.AntAzElMargThrsd = 0;
  memset(p.AntInst, 0, sizeof(p.AntInst[0]) * 7);
  p.TrkTimeOffset = 0;
  //p.RSkyInterval = 0;

  p.ioflg = 0;
  p.iaoflg = 0;
  p.ibjflg = 0;
  p.drdlv = 0;
  memset(p.dsourc, 0, sizeof(p.dsourc[0]) * 2);
  memset(p.dazelmin, 0, sizeof(p.dazelmin[0]) * 2);
  memset(p.dazelmax, 0, sizeof(p.dazelmax[0]) * 2);
  
  memset(&p.xin, 0, sizeof(p.xin));
  memset(p.sof, 0, sizeof(p.sof[0]) * 2);
  p.sofCoord = 0;
  if(p.off){
    free(p.off);
    p.off = NULL;
  }
  if(p.on){
    free(p.on);
    p.on = NULL;
  }
  if(p.linePath){
    free(p.linePath);
    p.linePath = NULL;
  }
  memset(p.cstart, 0, sizeof(p.cstart[0]) * 24);
  //memset(p.dweath, 0, sizeof(p.dweath[0]) * 3);
  //! �|�C���e�B���O�I�t�Z�b�g�͏��������Ȃ��B
  //p.pofFlg = 0;
  //p.pofCoord = 0;
  //memset(p.pof, 0, sizeof(p.pof[0]) * 2);
  p.rastStart = 0;
  p.rastStartTime = 0;
  if(p.rastSof){
    free(p.rastSof);
    p.rastSof = NULL;
  }
  p.rastSofSize = 0;
  p.rastSofNo = 0;
  //  uM("trkInitParam(); ***2");
  memset(p.P, 0, sizeof(p.P[0]) * 30);

  memset(&p.xout, 0, sizeof(p.xout));
  memset(p.doazel, 0, sizeof(p.doazel[0]) * 2);
  memset(p.doazel2, 0, sizeof(p.doazel2[0]) * 2);
  memset(p.doazelC, 0, sizeof(p.doazelC[0]) * 2);
  memset(p.deazel, 0, sizeof(p.deazel[0]) * 2);
  for(int i = 0; i < 8; i++){
    memset(p.dgtdat[i], 0, sizeof(p.dgtdat[0][0]) * 2);
  }
  memset(&p.acuStat, 0, sizeof(p.acuStat));
  memset(p.acuStatSBefore, 0, sizeof(p.acuStatSBefore[0]) * 5);
  if(p.fp){
    fclose(p.fp);
    p.fp = NULL;
  }
  p.thrdRet = 0;
  p.dayFlg = 0;
  p.sofNo = 0;
  p.offNo = 0;
  p.offFlg = 0;
  p.lst = 0;

  //! �O��̃|�C���e�B���OOffSet��ݒ�
  double pofSec[2];
  pofSec[0] = p.pof[0] * rad2sec;
  pofSec[1] = p.pof[1] * rad2sec;
  trkSetPof(pofSec, p.pofCoord);

  //! �ϑ��p�����[�^�擾
  setParam();

  if(p.ScanFlag == 1002){
    //! Lee Tracking
    p.ioflg = 99;                  //!< 99:AZEL
    p.iaoflg = 3;                  //!< 3:AZEL
    p.ibjflg = 2;                  //!< J2000.0
    p.dsourc[0] = 0 * PI / 180.0;  //!< �K���Ȓl AZ
    p.dsourc[1] = 10 * PI / 180.0; //!< �K���Ȓl EL
  }

  if(checkParam()){
    uM("trkInitParam(); checkParam(); error");
    p.thrdRet = TRK_PARAM_ERR;
    return p.thrdRet;
  }

  //! Initialize map pattern
  if(p.ScanFlag >= 0 && p.ScanFlag <= 6){
    //! On,5Point,Grid,Random,9Point,Raster
    int len = strlen(p.SetPattern);
    int i, j, k;
    
    uM1("trkInitParam(); debug SetPattern %s", p.SetPattern);
    for(i = 0, j = 0, k = 0; i < len; i++){
      if(p.SetPattern[i] == ' ' || p.SetPattern[i] == '\t'){
	continue;
      }
      else if((p.SetPattern[i] >= 'A' && p.SetPattern[i] <= 'Z')
	      || (p.SetPattern[i] >= '0' && p.SetPattern[i] <= '9')){
	if(j == 0){
	  k++;
	}
	j++;
      }
      else if(p.SetPattern[i] == ','){
	j = 0;
      }
    }

    //! �}�b�v�p�^�[���̗̈���m�ۂ���B
    p.mapNum = k;
    uM1("trkInitParam(); debug p.mapNum=%d", p.mapNum);
    //if(k != p.OnNumber){
    //  uM3("trkStart(); SetPattern Num[%d] != OnNumber[%d] and use first [%d] pieces", k, p.OnNumber, p.mapNum);
    //}
    p.mapPattern = (int*) malloc(sizeof(p.mapPattern[0]) * p.mapNum);
    memset(p.mapPattern, 0, sizeof(p.mapPattern[0]) * p.mapNum);
    //! �}�b�v�p�^�[����ID�L�^ 0:R +:On -:Off
    for(i = 0, k = 0; k < p.mapNum;){
      while((p.SetPattern[i] == ' ' || p.SetPattern[i] == '\t') && p.SetPattern[i] != '\0'){
	i++;
      }
      
      if(p.SetPattern[i] == 'R'){
	//! R
	p.mapPattern[k] = 0;
	k++;
      }
      else if(p.SetPattern[i] >= 'A' && p.SetPattern[0] <= 'Z'){
	//! Off-Point
	p.mapPattern[k] = -(p.SetPattern[i] - 'A' + 1);
	//! �s���f�[�^�̏ꍇ�͖���
	if(p.mapPattern[k] < - p.OffNumber){
	  char tmp = - p.mapPattern[k] - 1 + 'A';
	  //uM2("trkInitParam(); invalid SetPattern[%d(%c)]", i, tmp);
	  p.mapPattern[k] = 0;
	}
	else{
	  k++;
	}
      }
      else if(p.SetPattern[i] >= '0' && p.SetPattern[i] <= '9'){
	//! On-Point
	while(p.SetPattern[i] >= '0' && p.SetPattern[i] <= '9' && p.SetPattern[i] != '\0'){
	  p.mapPattern[k] = (p.SetPattern[i] - '0') + p.mapPattern[k] * 10;
	  i++;
	}
	//! �s���f�[�^�̏ꍇ�͖���
	if(p.mapPattern[k] > p.OnNumber){
	  //uM1("trkInitParam(); invalid SetPattern[%d]", p.SetPattern[k]);//080627 out
	  p.mapPattern[k] = 0;
	}
	else{
	  k++;
	}
      }
      else if(p.SetPattern[i] != ','){
	//uM1("trkInitParam(); invalid SetPattern data [%c]", p.SetPattern[i]);//080627 out
      }

      while(p.SetPattern[i] != ',' && p.SetPattern[i] != '\0'){
	i++;
      }
      i++;
    }
    p.mapNum = k; //!< �s���f�[�^��������̊ϑ��_��

    //! �m�F
    //    for(k = 0; k < p.mapNum; k++){
    // if(p.mapPattern[k] < - p.OffNumber){
    //char tmp = - p.mapPattern[k] - 1 + 'A';
    //uM1("trkInitParam(); invalid SetPattern[%c] ans set 'A'", tmp);
    //p.mapPattern[k] = -1;
    // }
    // else if(p.mapPattern[k] > p.OnNumber){
    //uM1("trkInitParam(); invalid SetPattern[%d] ans set '1'", p.SetPattern[k]);
    //p.mapPattern[k] = 1;
    // }
    //}
    //! debug
    //printf("trkInitParam(); debug mapPattern = [");
    for(k = 0; k < p.mapNum; k++){
      uM1("map pattern: %d,", p.mapPattern[k]);
      //      printf("%d,", p.mapPattern[k]);
    }
    //printf("]\n");
  }

  if(p.ScanFlag == 6){
    //! Raster
    p.rastSofSize = p.LineTime / p.TrkInterval + 2; //!< �Ō��1�񕪂ƒ[�����o���ꍇ�̕��𑫂��Ă���
    //uM1("trkInitParam(); p.rastSofSize = %d", p.rastSofSize);
    p.rastSof = (double*)malloc(sizeof(p.rastSof[0]) * p.rastSofSize * 2);
    memset(p.rastSof, 0, sizeof(p.rastSof[0]) * p.rastSofSize * 2);
  }

  //! Initialize trk45 library
  strcpy(p.cstart, tmGetTimeStr(p.vtm, p.TrkTimeOffset));
  if(trk_00(p.cstart, "", cerr) != 0) {
    uM2("trkInitParam(); trk_00(%s) %s", p.cstart, cerr);
    p.thrdRet =  TRK_TRK00_ERR;
    return p.thrdRet;
  }
  p.dayFlg = 1;

  //! Get Zone Type
  uLock();
  ret = trkCal();
  uUnLock();
  if(ret){
    uM2("trkInitParam(); trkCal(); [%d(%s)] error!!", ret, errnoStr(ret));
    p.thrdRet = ret;
    return p.thrdRet;
  }
  trkCalZoneTypeInit();

  return 0;
}

/*! \fn int trkStart()
\brief �ϑ��e�[�u���Ɋ�Â����ǔ��̎��s����B
\return 0:Success other:error
*/
int trkStart(){
  int status;

  //! Thread start
  status = pthread_create(&p.thrdID, NULL, trkThrdMain, NULL);
  if(status != 0){
    uM1("trkStart(); pthread_create(); status=%d error!!", status);
    p.thrdRet = TRK_THRD_ERR;
    return p.thrdRet;
  }
  p.thrdRet = 0;
  return 0;
}

/*! \fn int trkStop()
\brief �ǔ��̒�~
\retval 0 ����
*/
int trkStop(){
  if(p.TrkUse==0){
    return TRK_NOT_USE;
  }
  if(p.thrdRun){
    p.thrdRun=0;
    pthread_join(p.thrdID, NULL);
  }
  else{
    return TRK_THRD_STOPPED_ERR;
  }
  return 0;
}

/*! \fn int trkRepeat
\brief �ǔ����s��Ԃ̎擾
*/
int trkRepeat(){
  if(p.TrkUse==0)
    return TRK_NOT_USE;

  if(p.thrdRun == 0 && p.thrdStatusRun == 0){
    return TRK_THRD_STOPPED_ERR;
  }
  return p.thrdRet;
}

/*! \fn void* trkThrdMain(void* _p)
\brief �����ǔ��X���b�h
\param[in] _p �p�����[�^�z��(���g�p)
\return NULL(0):����
*/
void* trkThrdMain(void* _p){
  char tmp[4096];
  int ret;
  int err;

  //! �X�e�[�^�X�擾�X���b�h�������Ă������~����B
  //if(p.thrdStatusRun && p.ScanFlag != 6){//080630 in
  if(p.thrdStatusRun){
    p.thrdStatusRun=0;
    pthread_join(p.thrdStatusID, NULL);
    p.thrdStatusID = NULL;
  }

  //! Main Loop
  p.thrdRun = 1;
  while(p.thrdRun){
    //uM("while() start");// debug 080621 in

    //! �^�C�~���O����
    if(!p.rastStart){
      //! Not Raster
      if(tmGetLag(p.vtm) < p.TrkInterval){
	tmSleepMSec(p.TrkInterval * 10.0);
	continue;
      }
    }

    //! RastStart�t���O�������Ă�����Raster�������s���B
    //double a = tmGetDiff(p.rastStartTime, 0);
    //printf("trkThrdMain(); debug tmGetDiff(); diff=%lf sec\n", a);
    int rastFlg = 0;
    //uM("if #2"); // debug 080621 in
    if(p.rastStart){
      if(p.rastSofNo > 0){
	rastFlg = 1;
      }
      else if(p.rastSofNo == 0 && tmGetDiff(p.rastStartTime, 0) < p.TrkInterval){
	rastFlg = 1;
      }
      else{
	//! �X�^�[�g�����ɋ߂Â��܂Œʏ폈��
	if(tmGetLag(p.vtm) < p.TrkInterval){
	  tmSleepMSec(p.TrkInterval * 10.0);
	  continue;
	}
      }
    }
    //uM1("trkThrdMain(); rastFlg(if_before) = %d", rastFlg);// 080625 out
    if(rastFlg){
      //uM1("trkThrdMain(); rastFlg(if_after) = %d", rastFlg);
      //! ���X�^�[��
      g_rastEndFlag = RUN_RASTENDF;
      double sof[2];
      if(p.rastStart == 1){//update080529*1
	if(p.rastSofNo < p.rastSofSize){
	  p.xin.iasflg = p.OnCoord;
	  sof[0] = p.rastSof[p.rastSofNo*2];
	  sof[1] = p.rastSof[p.rastSofNo*2+1];
	  p.xin.dscnst[0] = sof[0];
	  p.xin.dscnst[1] = sof[1];
	  p.xin.dscned[0] = sof[0];
	  p.xin.dscned[1] = sof[1];
	  //! 1msec�ȓ��ɂȂ�܂ő҂B
	  double offsetMSec = p.rastSofNo * p.TrkInterval * 1000.0;
	  //uM("raster while() before");// debug 080621 in
	  while(tmGetDiff(p.rastStartTime, offsetMSec) > 0.001){}
	  p.rastSofNo++;
	  uM3("trkThrdMain(); rastSofNo[%03d] sof(%.15lf,%.15lf)\"",
	      p.rastSofNo, sof[0] * rad2sec, sof[1] * rad2sec);
	}
	else{
	  //! ���X�^�[�I��
	  p.rastStart = 0;
	  p.rastSofNo = 0;
	  //	uM("trkThrdMain(); p.rastStart = 0 done");
	  g_rastEndFlag = END_RASTENDF;
	}
      }//*1
    }
    //uM("after if #3");// debug 080621 in
    //   printf("trkThrdMain(); debug sof(%.15lf,%.15lf)\" rastStart=%d rastSofNo=%d rastSofSize=%d\n",
    //p.xin.dscnst[0], p.xin.dscned[1], p.rastStart, p.rastSofNo, p.rastSofSize);

    tmReset(p.vtm);
    strcpy(p.cstart, tmGetTimeStr(p.vtm, p.TrkTimeOffset));
    
    //uM("ACU() start"); // debug 080621 in?
    ret = 0;
    if(p.TrkUse == 1 && p.rastStart == 0){//080627 in
	  //        if(p.TrkUse == 1){
          ret = trkGetACU();
         if(ret){
    	uM1("trkThrdMain(); trkGetACU(); ret = %d", ret);
    	p.thrdRet = ret;
          }
        }
    
    //! �ǔ���Ԃ̎Z�o
    //uM("Status() start"); // debug 080621 in?
    trkCalTrackStatus(ret);    

    //! �ǔ��v�Z
    //uM("calc start"); // debug 080621 in
    uLock();
    ret = trkCal();
    uUnLock();
    //uM("calc end"); // debug 080621 in
    //printf("debug dazel  [rad]    : %.9lf,%.9lf,\n", p.xout.dazel[0], p.xout.dazel[1]);
    //printf("debug %s doazel2 : %.9lf,%.9lf,\n", p.cstart, p.doazel2[0], p.doazel2[1]);
    //printf("debug OBS    [rad]    : %+10.9lf %+10.9lf (RADEC)\n", p.dgtdat[4][0], p.dgtdat[4][1]);
    if(ret < 0){
      uM("if #4 true");
      p.thrdRet = ret;
      continue;
    }

    //! �A���e�i�̋쓮���E�𒴂���ꍇ�͒ǔ����Ȃ�
    err=0;
    p.doazelC[0] = p.doazel2[0];
    p.doazelC[1] = p.doazel2[1];
    if(p.antZone == 2 && p.doazel2[0] <= p.dazelmin[0]){
      //uM3("Antenna Az(%lf) <= %lf (zone=%d)",
      //p.doazel2[0], p.dazelmin[0], p.antZone);
      err = TRK_LIMIT_AZ_ERR;
    } 
    else if(p.antZone == 1 && p.doazel2[0] >= p.dazelmax[0]){
      //uM3("Antenna Az(%lf) >= %lf (zone=%d)",
      //p.doazel2[0], p.dazelmax[0], p.antZone);
      err = TRK_LIMIT_AZ_ERR;
    }
    if(p.doazel2[1] < p.dazelmin[1] || p.doazel2[1] > p.dazelmax[1]){
      //uM3("Antenna El(%lf) over limit(%lf - %lf)",
      //p.doazel2[1], p.dazelmin[1], p.dazelmax[1]);
      if(err){
	err = TRK_LIMIT_AZ_EL_ERR;
      }
      else{
	//! El������~�b�g�݂̂̏ꍇ��El��ՊE�p�����50[sec]���ꂽ���܂ł����čs���AAz�͓�����
	double marg = 50.0 / 3600.0;
	if(p.doazel2[1] > p.dazelmax[1]){
	  p.doazelC[1] = p.dazelmax[1] - marg;
	}
	err = TRK_LIMIT_EL_ERR;
      }
    }
    //uM1("err calculated: %d.", err);
    if(err){
      p.thrdRet = err;
      //tmSleepMSec(p.TrkInterval * 1000.0);
      //! El������~�b�g�݂̂̏ꍇ��El��ՊE�p�����50[sec]���ꂽ���܂ł����čs���AAz�͓�����
      if(!(p.doazel2[1] > p.dazelmax[1]  && err == TRK_LIMIT_EL_ERR)){
	continue;
      }
    }

    //uM("before output position");
    if(1){
      //! ���ʏo��
      //sprintf(tmp,"START => %s\n", p.cstart);
      sprintf(tmp + strlen(tmp),"AZEL [deg]   : %+10.9lf %+10.9lf\n",p.doazel2[0], p.doazel2[1]);
      //sprintf(tmp + strlen(tmp),"DAZ [rad/sec]: %+10.9lf\n", p.xout.ddazel[0]);
      //sprintf(tmp + strlen(tmp),"DEL [rad/sec]: %+10.9lf\n", p.xout.ddazel[1]);
      //! �}�b�v�Z���^�[���W
      //sprintf(tmp + strlen(tmp),"MAP [rad]    : %+10.9lf %+10.9lf (RADEC)\n", p.dgtdat[1][0], p.dgtdat[1][1]);
      //sprintf(tmp + strlen(tmp),"MAP [rad]    : %+10.9lf %+10.9lf (LB)\n",    p.dgtdat[2][0], p.dgtdat[2][1]);
      //! �ϑ��ʒu���W
      //sprintf(tmp + strlen(tmp),"OBS [rad]    : %+10.9lf %+10.9lf (RADEC)\n", p.dgtdat[4][0], p.dgtdat[4][1]);
      //sprintf(tmp + strlen(tmp),"OBS [rad]    : %+10.9lf %+10.9lf (LB)\n",    p.dgtdat[5][0], p.dgtdat[5][1]);
      //uM1("%s",tmp);// habolim
    }

    //uM("before SafetyCheck()");
    if(p.TrkUse == 1){
      //uM("InvokeSafetyCheck()");
      ret = trkSafetyCheck();
      //! debug
      //! ACU�֑��M
      if(ret){
	p.thrdRet = ret;
      }
      else{
	//uM("Invoke trkSetACU()");
	ret = trkSetACU();
	if(ret){
	  //uM1("trkThrdMain(); trkSetACU(); return %d", ret);
	  p.thrdRet = ret;
	}
      }
    }//!< if(p.TrkUse==1)

    if(err == 0){
      p.thrdRet = 0;
    }
    //uM("while() end");// debug 080621 in
    //tmSleepMSec(p.TrkInterval * 1000.0);
  }//!< while(1)
  p.thrdRun = 0;
  //p.thrdRet = 0;

  //! ���݂�RealAZEL�Ŏw�߂�������B��~������B
  if(p.TrkUse == 1){
    ret = trkSafetyCheck();
    if(!ret){
      p.antZone = 0; //! 0:Auto
      p.doazelC[0] = p.acuStat.dazel[0];
      p.doazelC[1] = p.acuStat.dazel[1];
      ret = trkSetACU();
      if(ret){
	uM1("trkThrdMain(); trkSetACU(); return %d", ret);
	p.thrdRet = ret;
      }
    }
    else{
      p.thrdRet = ret;
    }
  }

  //if(p.ScanFlag != 6){//080630 in
    int status = pthread_create(&p.thrdStatusID, NULL, trkThrdStatus, NULL);
    if(status != 0){
      uM1("trkThrdMain(); pthread_create(trkThrdStatus); status=%d error", status);
      return NULL;
    }

    //}

  return NULL;
}

/*! \fn void* trkThrdStatus(void* _p)
\brief �X�P�W���[���ϑ����Ă��Ȃ��Ƃ��̃X�e�[�^�X�擾�X���b�h
\param[in] _p �p�����[�^�z��(���g�p)
\return NULL(0):����
*/
void* trkThrdStatus(void* _p){
  int ret;

  p.thrdStatusRun = 1;
  while(p.thrdStatusRun){
    if(tmGetLag(p.vtm) < p.TrkInterval){
      tmSleepMSec(p.TrkInterval * 10.0);
      continue;
    }
    tmReset(p.vtm);
    //p.thrdRet = 0;
    if(p.TrkUse == 1){
      ret = trkGetACU();
      if(ret){
	uM2("trkThrdStatus(); trkGetACU(); [%d(%s)]", ret, errnoStr(ret));
	p.thrdRet = ret;
      }
      ret = trkSafetyCheck();
    }
  }
  p.thrdStatusRun = 0;
  return NULL;
}


/*! \fn int trkEnd()
\brief �I�����ɌĂ�
\retval 0 ����
*/
int trkEnd(){
  void* ret;

  init = 0;
  if(p.thrdRun){
    p.thrdRun=0;
    pthread_join(p.thrdID, &ret);
    p.thrdID = NULL;
  }
  if(p.thrdStatusRun){
    p.thrdStatusRun=0;
    pthread_join(p.thrdStatusID, &ret);
    p.thrdStatusID = NULL;
  }
  if(comAct){
    rs232cEnd(p.TrkCom);
    comAct = 0;
  }
  if(p.vtm){
    tmEnd(p.vtm);
    p.vtm = NULL;
  }
  if(p.fp){
    fclose(p.fp);
    p.fp = NULL;
  }
  if(p.SetPattern){
    free(p.SetPattern);
    p.SetPattern = NULL;
  }
  if(p.mapPattern){
    free(p.mapPattern);
    p.mapPattern = NULL;
  }
  if(p.on){
    free(p.on);
    p.on = NULL;
  }
  if(p.linePath){
    free(p.linePath);
    p.linePath = NULL;
  }
  if(p.off){
    free(p.off);
    p.off = NULL;
  }
  if(p.rastSof){
    free(p.rastSof);
    p.rastSof = 0;
  }
  p.rastSofSize = 0;

  return 0;
}

/*! \fn int trkCal()
\brief �ǔ��v�Z��
\retval 0 ����
\retval �}�C�i�X�l ���s
*/
int trkCal(){
  char cerr[256];

  //! trk_20�p�ϐ�
  double dtime;

  //! trk_30�p�ϐ�
  double diazel[2], dorade[2];
  static double dgtloc;        //!< �u�Ԃ̎��P���� [rad]

  //! ��Ɨp�ϐ�
  int ival;
  double daz, del;

  //! UT�ɂ����ē��ɂ����ς���Ă�����trk_00�����s����
  tmReset(p.vtm);
  strcpy(p.cstart, tmGetTimeStr(p.vtm, p.TrkTimeOffset));
  if(p.cstart[8] == '0' && p.cstart[9] == '9'){         //!< JST�����Ƃ̔�r
    if(p.dayFlg == 0){
      uM1("trkCal(); trk_00(%s)", p.cstart);
      if(trk_00(p.cstart, "", cerr) != 0) {
	uM1("trkCal(); %s",cerr);
	return TRK_TRK00_ERR;
      }
      p.dayFlg = 1;
    }
  }
  else{
    p.dayFlg = 0;
  }

  //! trk_10
  //! ��Q�����͏����t���O 0:AZEL
  //! uM2("debug trkCal(); sourc=(%lf,%lf)", p.dsourc[0], p.dsourc[1]);
  if(p.OffMode == 2 && p.offFlg == 1){
    //! ��΍��W�w���OFF�_���ϑ����̏ꍇ�B0:���z�n�O 0:AZEL :OffCoord ibjflg:On�_�Ɠ������� :���W 0:���x 
    ival = trk_10(0, 0, p.OffCoord, p.ibjflg, &p.off[p.offNo*2], 0);
  }
  else{
    //! �ʏ�̓V�̂��ϑ����̏ꍇ
    ival = trk_10(p.ioflg, 0, p.iaoflg, p.ibjflg, p.dsourc, p.drdlv);
  }

  if (ival == 1) {
    //uM("trk_10: Argument check error");
    return TRK_TRK10_ARG_ERR;
  }
  else if (ival == 2){
    //uM("trk_10: Common area initial error");
    return TRK_TRK10_COMAREA_ERR;
  }
  
  //! trk_20 (�ϑ��Y�������̐ݒ� YYYYMMDDhhmmss.0 (JST))
  if(tjcd2m(p.cstart, &dtime) == 1){  //!< JST���n�̗���t����MJD�ւ̕ϊ�
    //uM("tjcd2m: Error");
    return TRK_TJCD2M_ERR;
  }
  ival = trk_20(dtime, p.xin, &p.xout);
  trk_ant_(p.dgtdat[0], p.dgtdat[1], p.dgtdat[2], p.dgtdat[3], p.dgtdat[4],
	   p.dgtdat[5], p.dgtdat[6], p.dgtdat[7], &dgtloc);
  //! �V�̍��W�ALST���擾 �����b��b���Z��������[sec]
  p.lst = dgtloc * (24.0 * 60.0 * 60.0 / (2.0 * PI));

  if(ival == -1){
    //uM("trk_20: Argument check error");
    p.doazel2[0] = p.xout.dazel[0] * 180.0 / PI;
    p.doazel2[1] = p.xout.dazel[1] * 180.0 / PI;
    return TRK_TRK20_ARG_ERR;
  }
  else if (ival == -2) {
    //uM("trk_20: Local error end");
    p.doazel2[0] = p.xout.dazel[0] * 180.0 / PI;
    p.doazel2[1] = p.xout.dazel[1] * 180.0 / PI;
    return TRK_TRK20_LOCAL_ERR;
  }
  //! Elevation���n�����ȉ��ɂȂ��Ă��܂��Ă���ꍇ�͒ǔ��s�\ 
  //uM1("trkCal(); xout.dazel[1] (%lf deg)", p.xout.dazel[1]*180.0/PI);
  if(p.xout.dazel[1]<=0){
    p.doazel2[0] = p.xout.dazel[0] * 180.0 / PI;
    p.doazel2[1] = p.xout.dazel[1] * 180.0 / PI;
    return TRK_TRK20_EL_ERR;
  }
    
  diazel[0] = p.xout.dazel[0];
  diazel[1] = p.xout.dazel[1];

  //! trk_30 (��C���␳)
  ival = trk_30(diazel, p.dweath, p.dgtdat[0], dgtloc, p.doazel, dorade);
  if(ival != 0){
    //! uM("trk_30: Local error end");
    p.doazel2[0] = p.xout.dazel[0] * 180.0 / PI;
    p.doazel2[1] = p.xout.dazel[1] * 180.0 / PI;
    return TRK_TRK30_LOCAL_ERR;
  }
  //uM1("trkCal(); doazel[1] %lf", p.doazel[1] * 180.0 / PI);

  //! rad(p.doazel) -> deg(p.doazel2)
  p.doazel2[0] = p.doazel[0] * 180.0/PI;
  p.doazel2[1] = p.doazel[1] * 180.0/PI;

  if(p.doazel2[1] < 0){
    return TRK_LIMIT_EL_ERR;
  }

  //! �퍷�␳
  trkCalInst(p.doazel2[0], p.doazel2[1], &daz, &del);
  p.doazel2[0] += daz;
  p.doazel2[1] += del;

  while(p.doazel2[0]>=360.0){
    p.doazel2[0]-=360.0;
  }
  while(p.doazel2[0]<0.0){
    p.doazel2[0]+=360.0;
  }
  
  trkCalZoneType(p.doazel2[0]);
  return 0;
}

/*! \fn void trkCalInst(double X, double Y, double* dx, double* dy)
\brief �퍷�␳���s�����߂̊֐��B
\brief �ꎞ�I�ɂ���32m����S/X�тł̃p�����[�^�[���g�p
\param[in] X  raw Az deg
\param[in] Y  raw El deg
\param[out] dX (add to raw Az) deg
\param[out] dY (add to raw El) deg
*/
void trkCalInst(double X, double Y, double* dX, double* dY){
  double Phi = 90.0 * PI / 180.0;
  //double P[31] = {0};

  //memset(P, 0, 31*sizeof(P[0]));
  //! deg -> rad
  X *= PI / 180.0;
  Y *= PI / 180.0;

  //*dX=P[1] + P[3]*tan(Y) - P[4]/cos(Y) + P[5]*sin(X)*tan(Y) - P[6]*cos(X)*tan(Y);
  *dX=p.P[0] - p.P[1]*cos(Phi)*tan(Y) + p.P[2]*tan(Y) - p.P[3]/cos(Y)
    + p.P[4]*sin(X)*tan(Y) - p.P[5]*cos(X)*tan(Y)
    + p.P[11]*X + p.P[12]*cos(X) + p.P[13]*sin(X) + p.P[16]*cos(2*X) + p.P[17]*sin(2*X);
  
  //*dY=P[5]*cos(X) + P[6]*sin(X) + P[7] - P[8]*cos(Y);
  *dY=  p.P[4]*cos(X) + p.P[5]*sin(X)
    + p.P[6] - p.P[7]*(cos(Phi)*sin(Y)*cos(X)-sin(Phi)*cos(Y)) + p.P[8]*Y
    + p.P[9]*cos(Y) + p.P[10]*sin(Y) + p.P[14]*cos(2*X) + p.P[15]*sin(2*X)
    + p.P[18]*cos(8*Y) + p.P[19]*sin(8*Y) + p.P[20]*cos(X) + p.P[21]*sin(X);

  //! debug
  //double dx = *dX * 3600.0;
  //double dy = *dY * 3600.0;
  //printf("debug trkCalInst(); dAz dEl = %lf %lf\n", *dX, *dY);
  return;
}

/*! \fn void trkCalTrackStatus()
\brief �ǔ���Ԃ��Z�o����
\brief trackStat��ݒ肷�� 0:Tracking 1:Counting -:Swing(�r�b�g�t���O�ɂ���ĕ\�� 1:az�p��Swing 2:el�p��Swing 4:Zone�ړ����܂�Swing 8:���Z�b�g)
\param[in] reset 0:None other:Reset Tracking Status and return
*/
void trkCalTrackStatus(const int reset){
  double dAz;
  double dEl;
  int stat;   //!< 0:Tracking 1:Counting -:�r�b�g�t���O(1:Az 2:El 4:Zone 8:StateCalFault)

  //! Reset Tracking Status
  //! etc)RealAzEl������Ɏ擾����Ă��Ȃ��ꍇ�AUpdate Sof
  if(reset){
    p.trackStatCnt = 0;
    p.trackStat = -8;
    return;
  }

  //! ACU�ƒʐM���Ė����ꍇ�͏��Tracking
  if(p.TrkUse != 1){
    p.trackStatCnt = p.AntAzElMargThrsd;
    p.trackStat = 0;
    return;
  }

  stat = 0;
  if(p.antZone != 0 && p.antZone != p.acuStat.zone){
    stat = -4;
  }
  p.deazel[0] = p.acuStat.dazel[0] - p.doazel2[0];
  p.deazel[1] = p.acuStat.dazel[1] - p.doazel2[1];
  
  //! ��������ErrAz���炻�̂Ƃ���El�ɂ�����ErrAz�֕ϊ�����B
  //p.deazel[0] = 2.0 * asin(cos( p.doazel2[1] * PI / 180.0) * sin(p.deazel[0] * PI / 180.0)); 
  //p.deazel[0] *= 180.0 / PI;
  p.deazel[0] = p.deazel[0] * cos(p.doazel2[1] * PI / 180.0);

  dAz = fabs(p.deazel[0]);
  dEl = fabs(p.deazel[1]);
  if(dAz > p.AntAzElMarg[0]){
    stat += -1;
  }
  if(dEl > p.AntAzElMarg[1]){
    stat += -2;
  }

  if(stat == 0){
    //! Now tracking
    if(p.trackStatCnt > p.AntAzElMargThrsd){
      //! Tracking (count over threshold)
      p.trackStat = 0;
    }
    else{
      //! Counting
      p.trackStatCnt++;
      p.trackStat = 1;
    }
  }
  else{
    //! Swing
    p.trackStatCnt = 0;
    p.trackStat = stat;
  }
  return;
}

//! �X�e�[�^�X�v���R�}���h
unsigned char acuGStat[]={STX, 'S', ETX};

//! AZEL�ݒ�R�}���h
typedef struct sAcuAzel{
  unsigned char stx;
  unsigned char c;
  unsigned char z;       //!< �A���e�i�݂̍�]�[�� '1':CW, '2':CCW
  unsigned char x100;    //!< AZ������100�̈ʂ̒lASCII���� [deg]
  unsigned char x10;
  unsigned char x1;
  unsigned char x0_1;
  unsigned char x0_01;
  unsigned char x0_001;
  unsigned char y10;     //!< EL������10�̈ʂ̒lASCII���� [deg]
  unsigned char y1;
  unsigned char y0_1;
  unsigned char y0_01;
  unsigned char y0_001;
  unsigned char etx;     //!< ETX(0x03)
}tAcuAzel;

//! �쓮���[�h����
typedef struct sAcuMood{
  unsigned char stx;
  unsigned char m;
  unsigned char s1;      //!< 0x30:�X�^���o�C, 0x32:�v���O�����ǔ�, 0x33:STOW(�i�[)
  unsigned char etx;
}tAcuMood;

//! �쓮�֎~����
typedef struct sAcuBan{
  unsigned char stx;
  unsigned char d;
  unsigned char s1;
  unsigned char etx;
}tAcuBan;

//! STOW����
unsigned char acuStow[]={STX, 'E', ETX};


//! AZEL�X�e�[�^�X
typedef struct sAcuStat{
  unsigned char stx;      //! STX(0x02)
  unsigned char s;        //! ASCII��S(0x53)
  unsigned char z;        //! �A���e�i�݂̍�]�[�� '1':CW, '2':CCW
  unsigned char x100;     //! AZ������100�̈ʂ̒lASCII���� [deg]
  unsigned char x10;
  unsigned char x1;
  unsigned char x0_1;
  unsigned char x0_01;
  unsigned char x0_001;
  unsigned char y10;      //! EL������10�̈ʂ̒lASCII���� [deg]
  unsigned char y1;
  unsigned char y0_1;
  unsigned char y0_01;
  unsigned char y0_001;
  unsigned char s1;
  unsigned char s2;
  unsigned char s3;
  unsigned char s4;
  unsigned char s5;
  unsigned char etx;      //! ETX(0x03)
}tAcuStat;


/*! \fn int trkGetACU()
\brief ACU����f�[�^���擾����
\retval 0 ����
\retval 0�ȊO �G���[�R�[�h�Q��
*/
int trkGetACU(){
  const unsigned int bufSize=1024;
  unsigned char buf[1024] = {0};

  tAcuStat stat;
  int ret;
  int i,size;

  //uM("debug trkGetACU()");

  if(!comAct){
    uM("trkGetACU() if #1 true");
    if(rs232cInit(p.TrkCom, 4800, 7, 1, 1) == 0){
      uM1("trkGetACU(); RS-232C port%d open", p.TrkCom);
      comAct = 1;
    }
    else{
      return TRK_COM_ERR;
    }
  }
  
  uM("invoke rs232cWriter()");
  //! �X�e�[�^�X�v��
  ret=rs232cWrite(p.TrkCom, acuGStat, sizeof(acuGStat));
  uM("rs232cWriter() returned");
  if(ret<0){
    comAct=0;
    rs232cEnd(p.TrkCom);
    return TRK_COM_ERR;
  }
  else if(ret!=sizeof(acuGStat)){
    return TRK_COM_ERR;
  }

  uM("for of rs232cRead()");
  //! �X�e�[�^�X�Ǎ���
  size=0;
  for(i=0; i<5; i++){
    //ret=rs232cRead(p.TrkCom, buf+size, sizeof(tAcuStat)-size);
    ret=rs232cRead(p.TrkCom, buf+size, bufSize-size);
    if(ret<0){
      comAct=0;
      rs232cEnd(p.TrkCom);
      return TRK_COM_ERR;
    }
    else if(ret<sizeof(tAcuStat)-size){
      size+=ret;
    }
    else{
      break;
    }
    tmSleepMSec(10);
  }
  uM("for of rs232cRead() end");
  if(i==5){
    //buf[size] = '\0';
    //uM1("%s", buf);
    uM1("trkGetACU(); time out. current size=%d", size);
    return TRK_COM_DAT_ERR;
  }
  uM1("trkGetACU(); obtained size=%d successfully", size);
  //! debug
  //printf("trkGetACU(); debug ");
  //for(i=0; i<sizeof(tAcuStat); i++){
  //  printf("%02X ",((unsigned char*)&buf)[i]);
  //}
  //puts("");

  memcpy(&stat, buf, sizeof(tAcuStat));
  if(stat.stx!=STX || stat.s!='S' || stat.etx!=ETX){
    uM("trkGetACU(); invalid status data");//080702 in
    //    uM("trkGetACU(); invalid status date");//080702 out
    return TRK_COM_DAT_ERR;
  }

  memcpy(p.acuStatSBefore, p.acuStat.s, sizeof(p.acuStatSBefore[0]) * 5);
  //! �f�[�^���
  p.acuStat.zone = stat.z - 0x30;
  p.acuStat.dazel[0]  =
    100.0*(stat.x100-0x30)
    +10.0*(stat.x10-0x30)+(stat.x1-0x30)+0.1*(double)(stat.x0_1-0x30)
    +0.01*(double)(stat.x0_01-0x30)+0.001*(double)(stat.x0_001-0x30);

  p.acuStat.dazel[1]  =
    10.0*(stat.y10-0x30)+(stat.y1-0x30)+0.1*(double)(stat.y0_1-0x30)
    +0.01*(double)(stat.y0_01-0x30)+0.001*(double)(stat.y0_001-0x30);

  //uM2("RAZEL %lf %lf",p.acuStat.dazel[0], p.acuStat.dazel[1]);
  p.acuStat.s[0] = (int)stat.s1;
  p.acuStat.s[1] = (int)stat.s2;
  p.acuStat.s[2] = (int)stat.s3;
  p.acuStat.s[3] = (int)stat.s4;
  p.acuStat.s[4] = (int)stat.s5;

  return 0;
}

int trkSetACU(){
  const unsigned int bufSize=1024;
  unsigned char buf[1024];
  tAcuAzel azel;
  double az,el;
  int ret;

  memset(buf, 0, bufSize);

  az = p.doazelC[0]+0.0005;   //!< �l�̌ܓ�
  el = p.doazelC[1]+0.0005;
  
  azel.stx   = STX;
  azel.c     = 'C';
  if(p.antZone == 1){
    azel.z   = '1';
  }
  else if(p.antZone == 2){
    azel.z   = '2';
  }
  else{
    azel.z   = '0';           //! �ŏ��쓮����
  }
  azel.x100  = 0x30+((int)(az/100)%10);
  azel.x10   = 0x30+((int)(az/10)%10);
  azel.x1    = 0x30+((int)(az)%10);
  azel.x0_1  = 0x30+((int)(az/0.1)%10);
  azel.x0_01 = 0x30+((int)(az/0.01)%10);
  azel.x0_001= 0x30+((int)(az/0.001)%10);
  azel.y10   = 0x30+((int)(el/10)%10);
  azel.y1    = 0x30+((int)(el)%10);
  azel.y0_1  = 0x30+((int)(el/0.1)%10);
  azel.y0_01 = 0x30+((int)(el/0.01)%10);
  azel.y0_001= 0x30+((int)(el/0.001)%10);
  azel.etx   = ETX;

  //! debug
  //printf("trkSetACU(); debug ");
  //for(i=1; i<sizeof(azel)-1; i++){
  //  printf("%c",((char*)&azel)[i]);
  //}
  //puts("");
  //! �A���e�i�ւ̐���R�}���h�o��
  if(!comAct){
    if(rs232cInit(p.TrkCom, 4800, 7, 2, 1) == 0){
      uM1("trkSetACU(); RS-232C port%d open", p.TrkCom);
      comAct = 1;
    }
    else{
      return TRK_COM_ERR;
    }
  }

  ret=rs232cWrite(p.TrkCom, (unsigned char*)&azel, sizeof(azel));
  if(ret<0){
    comAct=0;
    rs232cEnd(p.TrkCom);
    return TRK_COM_ERR;
  }
  return 0;
}

/*! \fn int trkSetAcuMood(int mood)
\brief ACU�̃��[�h����
\param[in] mood 0:�X�^���o�C 2:�v���O�����ǔ� 3:StowLock
\return 0:���� -:���s
*/
int trkSetAcuMood(int mood){
  tAcuMood acuMood;
  int ret;

  if(mood != 0 && mood != 2 && mood != 3){
    return TRK_SET_ERR;
  }
  acuMood.stx = STX;
  acuMood.m = 'M';
  acuMood.s1 = 0x30 + ((unsigned char)mood);
  acuMood.etx = ETX;

  if(p.TrkUse == 1){
    ret = rs232cWrite(p.TrkCom, (unsigned char*)&acuMood, sizeof(acuMood));
    if(ret < 0){
      comAct = 0;
      return TRK_COM_ERR;
    }
  }
  return 0;
}

/*! \fn int trkSetAcuBan(int ban)
\brief �쓮�֎~����
\param[in] ban 0:�쓮�� 1:�쓮�֎~
\return 0:���� -:���s
*/
int trkSetAcuBan(int ban){
  tAcuBan acuBan;
  int ret;
  
  if(ban!=0 && ban!=1){
    return TRK_SET_ERR;
  }
  acuBan.stx=STX;
  acuBan.d='D';
  acuBan.s1=0x30 + ((unsigned char)ban);
  acuBan.etx=ETX;

  if(p.TrkUse == 1){
    ret=rs232cWrite(p.TrkCom, (unsigned char*)&acuBan, sizeof(acuBan));
    if(ret<0){
      comAct=0;
      return TRK_COM_ERR;
    }
  }
  return 0;
}

/*! \fn int trkSetAcuUnStow()
\brief StowUnlock����
\retval 0 ����
\retval 0�ȊO �G���[�R�[�h
*/
int trkSetAcuUnStow(){
  int ret;

  if(p.TrkUse == 1){
    ret = rs232cWrite(p.TrkCom, (unsigned char*)acuStow, sizeof(acuStow));
    if(ret < 0){
      comAct = 0;
      return TRK_COM_ERR;
    }
  }
  return 0;
}

/*! \fn int trkSafetyCheck()
\brief ACU�̃X�e�[�^�X�ŃG���[���������`�F�b�N����
\return 0:����, -1:���s
*/
int trkSafetyCheck(){
  int err;
  char tmp[1024];

  err=0;
  sprintf(tmp, "trkSafetyCheck();\n");
  //! AZEL�h���C�u�m�F
  if((p.acuStat.s[0] & EL_DRIVE_DISABLE) == EL_DRIVE_DISABLE){
    sprintf(tmp+strlen(tmp), "S1 = EL_DRIVE_DISABLE\n");
    //err=-1;
  }
  if((p.acuStat.s[0] & AZ_DRIVE_DISABLE) == AZ_DRIVE_DISABLE){
    sprintf(tmp+strlen(tmp), "S1 = AZ_DRIVE_DISABLE\n");
    //err=-1;
  }
  if((p.acuStat.s[0] & EL_DRIVE_ENABLE) == EL_DRIVE_ENABLE){
    sprintf(tmp+strlen(tmp), "S1 = EL_DRIVE_ENABLE\n");
  }
  if((p.acuStat.s[0] & AZ_DRIVE_ENABLE) == AZ_DRIVE_ENABLE){
    sprintf(tmp+strlen(tmp), "S1 = AZ_DRIVE_ENABLE\n");
  }
  //! �ǔ����[�h�m�F
  if((p.acuStat.s[1] & TRACK_MODE_FAULT) == TRACK_MODE_FAULT){
    sprintf(tmp+strlen(tmp), "S2 = TRACK_MODE_FAULT\n");
    //err=-1;
  }
  if((p.acuStat.s[1] & STANDBY) == STANDBY){
    sprintf(tmp+strlen(tmp), "S2 = STANDBY\n");
  }
  if((p.acuStat.s[1] & PROGRAM_TRACK_MODE) == PROGRAM_TRACK_MODE){
    sprintf(tmp+strlen(tmp), "S2 = PROGRAM_TRACK_MODE\n");
  }
  //! ���䃂�[�h�m�F
  if((p.acuStat.s[2] & REMOTE_CONTROL_MODE) == REMOTE_CONTROL_MODE){
    sprintf(tmp+strlen(tmp), "S3 = REMOTE_CONTROL_MODE\n");
  }
  if((p.acuStat.s[2] & LOCAL_CONTROL_MODE) == LOCAL_CONTROL_MODE){
    sprintf(tmp+strlen(tmp), "S3 = LOCAL_CONTROL_MODE\n");
    //err=-1;
  }
  if((p.acuStat.s[2] & STOW) == STOW){
    sprintf(tmp+strlen(tmp), "S3 = STOW\n");
  }
  //! �����A���[���m�F
  if((p.acuStat.s[3] & TOTAL_ALARM) == TOTAL_ALARM){
    sprintf(tmp+strlen(tmp),"S4 = TOTAL_ALARM error\n");
    //err=-1;
  }
  //! �p�x���m��̊m�F
  if((p.acuStat.s[4] & ANGLE_FAULT) == ANGLE_FAULT){
    sprintf(tmp+strlen(tmp), "S5 = ANGLE_FAULT\n");      
    err=-1;
  }
  if((p.acuStat.s[4] & AZ_DCPA_FAULT) == AZ_DCPA_FAULT){
    sprintf(tmp+strlen(tmp), "S5 = AZ_DCPA_FAULT\n");      
    err=-1;
  }
  if((p.acuStat.s[4] & EL_DCPA_FAULT) == EL_DCPA_FAULT){
    sprintf(tmp+strlen(tmp), "S5 = EL_DCPA_FAULT\n");      
    err=-1;
  }
  if(p.acuStat.s[0] != p.acuStatSBefore[0]
     || p.acuStat.s[1] != p.acuStatSBefore[1]
     || p.acuStat.s[2] != p.acuStatSBefore[2]
     || p.acuStat.s[3] != p.acuStatSBefore[3]
     || p.acuStat.s[4] != p.acuStatSBefore[4]){
    uM1("%s",tmp);
    //memcpy(p.acuStatSBefore, p.acuStat.s, sizeof(p.acuStat.s[0]) * 5);
  }
  if(err){
    return TRK_SAFETY_ERR;
  }
  return 0;
}

/*! \fn void trkSetPof(double* pof, int coord)
\brief �|�C���e�B���O�I�t�Z�b�g�̏C��
\param[in] pof �|�C���e�B���O�I�t�Z�b�gX,Y 2�����z�� [sec]
\param[in] coord �|�C���e�B���O�I�t�Z�b�g���W�̒�` 1:RADEC 2:LB 3:AZEL
*/
void trkSetPof(double* pof, int coord){
  double azel[2];
  //uLock();

  //!< �I�t�Z�b�g��`��3(AzEl)���V�̍��W�n��3(AzEl)�ȊO�Ȃ�|�C���e�B���O�I�t�Z�b�g�̍��W��`�͎��p
  //p.pofFlg = 1;

  p.pofCoord = coord;
  p.pof[0] = pof[0] * sec2rad;
  p.pof[1] = pof[1] * sec2rad;
  //p.xin.iapflg = p.pofCoord;
  
  if(p.thrdStatusRun || (p.ScanFlag != 2 && p.ScanFlag != 5)){
    //!< �X�e�[�^�X�擾���[�h�������́A5-Point,9-Point �ȊO�̏ꍇ�͑��I�t�Z�b�g�����s
    trkGetPAZEL(azel);
    uM2("trkSetPof(); PAZEL = %lf %lf [deg]", azel[0], azel[1]);
    uM4("trkSetPof(); Set Pointing Offset=(%.2lf\",%.2lf\") coord=%d(%s)", pof[0], pof[1], coord, errnoCoord(coord));
    p.xin.iapflg = coord;
    if(coord == 1 || coord == 2){
      p.xin.dpofst[0] = p.pof[0];
      p.xin.dpofst[1] = p.pof[1];
      memset(p.xin.dpofaz, 0, sizeof(double) * 2);
    }
    else if(coord == 3){
      p.xin.dpofaz[0] = p.pof[0];
      p.xin.dpofaz[1] = p.pof[1];
      memset(p.xin.dpofst, 0, sizeof(double) * 2);
    }
  }
  else{
    uM4("trkSetPof(); Next SetPattern Set Pointing Offset=(%.2lf\",%.2lf\") coord=%d(%s)",
	pof[0], pof[1], coord, errnoCoord(coord));
  }
  //uM("debug trkSetPof(); Complete");
  //uUnLock();
  return;
}

/*! \fn void trkSetSof(double* sof, int coord)
\brief �X�L�����I�t�Z�b�g�̏C��
\param[in] sof �X�L�����I�t�Z�b�gX,Y 2�����z�� [sec]
\param[in] coord �X�L�����I�t�Z�b�g���W�̒�` 1:RADEC 2:LB 3:AZEL
*/
void trkSetSof(double* sof, int coord){
  double x, y;

  uM4("trkSetSof(); Set scan offset(%.2lf\",%.2lf\") coord=%d(%s)",
      sof[0], sof[1], coord, errnoCoord(coord));
  //uLock();
  if(1 <= coord && coord <= 3){
    p.sofCoord = coord;
    p.xin.iasflg = coord;
  }
  sof[0] *= sec2rad;
  sof[1] *= sec2rad;
  memcpy(p.sof, sof, sizeof(p.sof[0]) * 2);  

  x =   sof[0] * cos(p.PosAngle_Rad) + sof[1] * sin(p.PosAngle_Rad);
  y = - sof[0] * sin(p.PosAngle_Rad) + sof[1] * cos(p.PosAngle_Rad);

  p.xin.dscnst[0] = x;
  p.xin.dscnst[1] = y;
  p.xin.dscned[0] = x;
  p.xin.dscned[1] = y;
  //uUnLock();
  return;
}

/*! \fn void trkSetOff()
\brief ���̃I�t�Z�b�g�ʒu��ݒ肷��
\brief �I�t�Z�b�g��2�_�ȏ゠��ꍇ�ɕK�v
*/
void trkSetOff(){
  if(p.offNo < 0 || p.offNo >= p.OffNumber){
    p.offNo = 0;
  }
  trkSetSof(&p.off[p.offNo*2], p.OffCoord);
  p.offNo++;
  return;
}

/*! \fn void trkSetSofNext()
\brief �ϑ��̎�ނ𔻕ʂ��Ď��̊ϑ��_��ݒ肵�A�ϑ��_�̎�ނ�Ԃ��B
\retval 1 End scan set
\retval 0 Set Next Sof
*/
int trkSetSofNext(int* id){
  double sof[2];
  int mapNo;

  //! OnPoint,5Point,Grid,Random,9Point,raster�͎��̃X�L�����ʒu�̊Ǘ���Random�ōs���B

  if(p.sofNo >= p.mapNum){
    //! 1Set����
    p.sofNo = 0;
    p.offFlg = 0;
    *id = 0;
    return 1;
  }
  else{
    mapNo = p.mapPattern[p.sofNo];
    if(mapNo == 0){
      //! R
      p.sofNo++;
      p.offFlg = 0;
    }
    else if(mapNo > 0){
      //! On-Point
      int onNo = mapNo - 1;
      memcpy(sof, &p.on[onNo*2], sizeof(double) * 2);
      trkSetSof(sof, p.OnCoord);
      //uM("trkSetSofNext(); ***1");
      p.sofNo++;
      p.offFlg = 0;

      if(p.ScanFlag == 6){
	//! Raster��On�_�Ȃ�J�n�_(Approach�_�̊J�n�_)�ƏI���_���擾��
	//! �X�L�����I�t�Z�b�g��TrkInterval�Ԋu�Ōv�Z���S�ă������Ɋi�[
	const double a = cos(p.PosAngle_Rad);
	const double b = sin(p.PosAngle_Rad);
	double sx, sy; //!< rad
	double dx, dy; //!< rad

	//! �X�^�[�g���W�A�ϑ���Ԃ�PosAngle���l�����Ď擾����
	sx =(  p.on[onNo*2] * a + p.on[onNo*2+1] * b) * sec2rad;
	sy =(- p.on[onNo*2] * b + p.on[onNo*2+1] * a) * sec2rad;
	dx =   p.linePath[onNo*2] * a + p.linePath[onNo*2+1] * b;
	dy = - p.linePath[onNo*2] * b + p.linePath[onNo*2+1] * a;

	for(int i = 0; i < p.rastSofSize; i++){
	  p.rastSof[i*2]   = sx + dx * (double)i / (double)(p.rastSofSize-1);
	  p.rastSof[i*2+1] = sy + dy * (double)i / (double)(p.rastSofSize-1);
	  //uM3("trkSetSofNext(); rastSof[%d](%.15lf,%.15lf)\"",
	  // i, p.rastSof[i*2], p.rastSof[i*2+1]);
	}
	/* edited by hagiwara */	
// 	if(onNo > 0 && p.rastSofNo <= p.rastSofSize )//update070529
// 	  while(1){
// 	    uM("a");
// 	    if(p.rastSofNo - p.rastSofSize == 0) break;
// 	    usleep(10000);
// 	  }
// 	g_rastEndFlag = END_RASTENDF;
// 	uM("trkSetSofNext(); ***2");
	
 	if(p.rastStart == 0){//080530 in
	  p.rastSofNo = 0;//080530 out
	}//080530 in
      }
    }
    else if(mapNo < 0){
      //! OffPoint -1:Off1 -2:Off2 ....
      if(p.ScanFlag == 6){
	p.rastStart = 0; //!< �������X�^�[���ł��I��������
      }
      p.offNo = - mapNo - 1;
      if(p.OffMode != 2){
	//! ���΍��W��`��Off�_��������X�L�����I�t�Z�b�g�ɐݒ�
	memcpy(sof, &p.off[p.offNo*2], sizeof(double) * 2);
	trkSetSof(sof, p.OffCoord);
	//	uM("trkSetSofNext(); ***1");
      }
      else{
	memset(sof, 0, sizeof(double) * 2);
	trkSetSof(sof, p.OffCoord);
	//	uM("trkSetSofNext(); ***1");
      }
      p.offFlg = 1;
      p.sofNo++;
    }
    //uM3("debug trkSetSofNext(); sofNo[%d] mapNum[%d] mapNo[%d]", p.sofNo, p.mapNum, mapNo);
  }

  //! Reset Tracking Status
  trkCalTrackStatus(1); //!< Reset Tracking Status

  *id = mapNo;
  return 0;
}

/*! \fn int trkSetRastStart(time_t startTime)
\breif Raster�X�L�������J�n����B
\param[in] startTime �J�n������1970�N����̌o��UT1�b(time(&t)�Ŏ擾�ł���l)�Ŏw�肷��B
\return 0;Success othre:Error
*/
int trkSetRastStart(time_t startTime){
  time_t t;

  //! Tracking��ԂɂȂ��Ă��Ȃ��ꍇ�̓G���[
  if(p.trackStat != 0){
    p.thrdRet = TRK_STANDBY_ERR;
    return p.thrdRet;
  }

  if(p.ScanFlag != 6){
    p.thrdRet = TRK_SET_ERR;
    return TRK_SET_ERR;
  }

  if(p.rastStart == 1){
    uM("trkSetRastStart(); Already set");
    p.thrdRet = TRK_SET_ERR;
    return p.thrdRet;
  }

  time(&t);
  if(t > startTime){
    uM("trkSetRastStart(); Already passed startTime error!!");
    p.thrdRet = TRK_SET_ERR;
    return p.thrdRet;
  }

  //! RastStart�̐ݒ�
  p.rastStartTime = startTime;
  p.rastStart = 1;

  return 0;
}

/*! \fn void trkSetWeath(double* weath)
\brief �V����̐ݒ�
*/
void trkSetWeath(double* weath){ 
  p.dweath[0] = weath[0];    //!< �C�� [K]
  p.dweath[1] = weath[1];    //!< �C�� [hPa]
  p.dweath[2] = weath[2];    //!< �����C�� [hPa]

  p.dweath[0] -= ABS_ZERO;   //!< �C��[��]
  return;
}

/*! \fn int trkSetXY(int coord, double* xy)
\brief Lee Tracking�̎��ȂǂɃA���e�i�̍��W�𐏎��ݒ肷��B
\param[in] coord 1:RADEC 2:LB 3:AZEL
\param[in] xy ���W�ʒu(X,Y) rad
\reutrn 0:Success other:Error
*/
int trkSetXY(int coord, double* xy){
  uM3("trkSetXY(); coord[%d] XY[%lf, %lf] rad", coord, xy[0], xy[1]);
  if(p.ScanFlag != 1002){
    uM1("trkSetXY(); ScanFlag[%d] error!!", p.ScanFlag);
    return TRK_SET_ERR;
  }

  if(coord < 1 || coord > 3){
    uM("trkSetXY(); invalid error!!");
    return TRK_SET_ERR;
  }
  else if(xy[0] < 0.0 || xy[0] > 2.0 * PI || xy[1] < - PI / 2.0 || xy[1] > PI / 2.0){
    uM("trkSetXY(); invalid xy error!!");
    return TRK_SET_ERR;
  }

  p.iaoflg = coord;
  memcpy(p.dsourc, xy, sizeof(*p.dsourc) * 2);

  return 0;
}

/*! \fn void trkGetXY(int* coord, double* xy)
\brief ���݂̊ϑ��ʒu��p.iaoflg(�V�̒�`����Coordinate)�ɏ]���ĕԂ��B
\brief �f����AZEL��Ԃ����ƂƂ���B
\param[out] coord ��`���W�n�B 1:RADEC 2:LB 3:AZEL 0:Error
\param[out] xy ���Wdouble[2] [deg]
*/
void trkGetXY(int* coord, double* xy){

  if(p.iaoflg == 0){
    //! �f�� AZEL
    trkGetAZEL(xy);
    *coord = 3;
  }
  else if(p.iaoflg == 1){
    //! RADEC
    trkGetRADEC(xy);
    *coord = 1;
  }
  else if(p.iaoflg == 2){
    //! LB
    trkGetLB(xy);
    *coord = 2;
  }
  else if(p.iaoflg == 3){
    //! AZEL
    trkGetAZEL(xy);
    *coord = 3;
  }
  else{
    //! Error
    memset(xy, 0, sizeof(double) * 2);
    *coord = 0;
  }
  xy[0] *= 180.0 / PI;
  xy[1] *= 180.0 / PI;
  return;
}

/*! \fn void trkGetRADEC(double* RADEC)
\brief ���݂̊ϑ��ʒuRADEC�̎擾
\param[out] RADEC [rad]
*/
void trkGetRADEC(double* RADEC){
  memcpy(RADEC, p.dgtdat[4], sizeof(double) * 2);
  return;
}

/*! \fn void trkGetLB(double* LB)
\brief ���݂̊ϑ��ʒuLB�̎擾
\param[out] LB [rad]
*/
void trkGetLB(double* LB){
  memcpy(LB, p.dgtdat[5] , sizeof(double) * 2);
  return;
}

/*! \fn void trkGetAZEL(double* AZEL)
\brief ���݂̊ϑ��ʒuAZEL(�퍷�␳�����ʒu)�̎擾
\param[out] AZEL [rad]
*/
void trkGetAZEL(double* AZEL){
  memcpy(AZEL, p.doazel, sizeof(double) * 2);
  return;
}

/*! \fn void trkGetDRADEC(double* DRADEC)
\brief �X�L�����I�t�Z�b�gDRADEC�̎擾
\param[out] DRADEC [rad]
*/
void trkGetDRADEC(double* DRADEC){
  if(p.xin.iasflg==1){
    memcpy(DRADEC, p.xin.dscnst, sizeof(double)*2);
  }
  else{
    memset(DRADEC, 0, sizeof(double)*2);
  }
  return;
}

/*! \fn void trkGetDLB(double* DLB)
\brief �X�L�����I�t�Z�b�gDLB�̎擾
\param[out] DLB [rad]
*/
void trkGetDLB(double* DLB){
  if(p.xin.iasflg==2){
    memcpy(DLB, p.xin.dscnst, sizeof(double)*2);
  }
  else{
    memset(DLB, 0, sizeof(double)*2);
  }
  return;
}

/*! \fn void trkGetDAZEL(double* DAZEL)
\brief �X�L�����I�t�Z�b�gDAZEL�̎擾
\param[out] DAZEL [rad]
*/
void trkGetDAZEL(double* DAZEL){
  if(p.xin.iasflg==3){
    memcpy(DAZEL, p.xin.dscnst, sizeof(double) * 2);
  }
  else{
    memset(DAZEL, 0, sizeof(double)*2);
  }
  return;
}

/*! \fn void trkGetPZONE(int* PZONE)
\brief �A���e�iZone�̎w���p
\param[out] PZONE 0:�ŏ��쓮�p���� 1:CW 2:CCW
*/
void trkGetPZONE(int* PZONE){
  *PZONE = p.antZone;
  return;
}


/*! \fn void trkGetRZONE(int* RZONE)
\brief �A���e�iZone�̎��ےl
\param[out] RZONE 0:�ŏ��쓮�p���� 1:CW 2:CCW
*/
void trkGetRZONE(int* RZONE){
  *RZONE = p.acuStat.zone;
  return;
}

/*! \fn void trkGetPAZEL(double* PAZEL)
\brief �A���e�i�w���pPAZEL�̎擾
\param[out] PAZEL [rad]
*/
void trkGetPAZEL(double* PAZEL){
  memcpy(PAZEL, p.doazel2, sizeof(double)*2);
  return;
}

/*! \fn void trkGetRAZEL(double* RAZEL)
\brief �A���e�i���ۊpRAZEL�̎擾
\param[out] RAZEL [rad]
*/
void trkGetRAZEL(double* RAZEL){
  memcpy(RAZEL, p.acuStat.dazel, sizeof(double)*2);
  return;
}

/*! \fn void trkGetEAZEL(double* EAZEL)
\brief �A���e�i�w���p�Ǝ��ۊp�̌덷EAZEL�̎擾
\param[out] EAZEL (=RAZEL-PAZEL) [rad]
*/
void trkGetEAZEL(double* EAZEL){
  memcpy(EAZEL, p.deazel, sizeof(double)*2);
  return;
}

/*! \fn void trkGetACUStatus(int* status)
\brief ACU�̃X�e�[�^�X���擾����
\param[out] status int[5]�ƂȂ��Ă��肻�ꂼ��ɃX�e�[�^�X��񂪃r�b�g�t���O�Ŋi�[����Ă���B
*/
void trkGetACUStatus(int* status){
  memcpy(status, p.acuStat.s, sizeof(int) * 5);
  return;
}

/*! \fn void trkGetSof(int* coord, double* sof)
\brief �X�L�����I�t�Z�b�g�̌��ݒl�擾
\param[out] coord 1:RADEC 2:LB 3:AZEL
\param[out] sof �X�L�����I�t�Z�b�g�̒l [sec]
*/
void trkGetSof(int* coord, double* sof){
  *coord = p.sofCoord;
  memcpy(sof, p.xin.dscnst, sizeof(double) * 2);
  //memcpy(sof, p.sof, sizeof(double) * 2);
  sof[0] *= rad2sec;
  sof[1] *= rad2sec;
  return;
}

/*! \fn void trkGetPof(int* coord, double* pof)
\brief �|�C���e�B���O�I�t�Z�b�g�̌��ݒl�擾
\param[out] coord 1:RADEC 2:LB 3:AZEL
\param[out] pof �|�C���e�B���O�I�t�Z�b�g�̒l [sec]
*/
void trkGetPof(int* coord, double* pof){
  *coord = p.xin.iapflg;
  if(p.xin.iapflg == 1 || p.xin.iapflg == 2){
    memcpy(pof, p.xin.dpofst, sizeof(double) * 2);
  }
  else{
    memcpy(pof, p.xin.dpofaz, sizeof(double) * 2);
  }
  pof[0] *= rad2sec;
  pof[1] *= rad2sec;
  return;
}

/*! \fn void trkGetTrackStatus(int* status)
\brief �ǔ���Ԃ̎擾
\param[out] status �r�b�g�t���O�ɂ���ĕ\�� 0:Tracking -1:az�p��Swing -2:el�p��Swing -4:Zone�ړ����܂�Swing
*/
void trkGetTrackStatus(int* status){
  *status = p.trackStat;
  return;
}

/*! \fn void trkGetLST(unsigned int* lst)
\brief LST�̎擾
\param[out] lst �����b�݂̂�b���Z����LST [sec]
*/
void trkGetLST(unsigned int* lst){
  *lst = p.lst;
  return;
}

/*! \fn void trkCalZoneType(double az)
\brief �A���e�i�ǂ̖k�܈ʒu����A���e�i�̋쓮Zone���Z�o����B
\param[in] az �V�̂����邽�߂̃A���e�iAz�p [deg]
*/
void trkCalZoneType(double az){
  switch(p.antZoneType){
  case 0:
    p.antZone = 0; //!< Auto �ŒZ����
  case 1:
  case 2:
    p.antZone = 2; //!< CCW
    break;
  case 4:
  case 5:
    p.antZone = 1; //!< CW
    break;
  case 3:
    //if(az >= 270.0 + p.antPosLaLo[0] && az < 360.0){
    if(az >= 270.0 && az < 360.0){
      p.antZone = 2;//!< CCW
    }
    //else if(az >= 0 && az <= 90.0 - p.antPosLaLo[0]){
    else if(az >= 0 && az <= 90.0){
      p.antZone = 1;//!< CW
    }
    else{
      //! �͈͊O�̏ꍇ��antZoneType=0�ŏ���
      p.antZoneType = 0;
      p.antZone = 0;
    }
    break;
  default:
    p.antZone = 0;
    break;
  }
  return;
}

/*! \fn void trkCalZoneTypeInit()
\brief �A���e�i��ZoneType��ݒ肷��BZoneType��0�`5�����L�̂悤�ɒ�`����
\brief Map Center Dec���K�v�Ȃ��ߕK��trkCal()���s������ɌĂԂ��ƁB
\brief [0] �A���e�i�ړ��p���ŒZ�����ɂȂ�悤��Zone���g�p
\brief [1] 0<=antAz<=90-(antLatitude) (Zone = CCW)
\brief [2] 90-(antLatitude)<antAz<270+(antLatitude) (Zone = CCW)
\brief [3] 270+(antLatitude)<=antAz<360 (Zone = CCW)
\brief     or 0<=antAz<=90-(antLatitude) (Zone = CW)
\brief [4] 90-(antLatitude)<antAz<270+(Antenna Latitude) (Zone = CW)
\brief [5] 270+(antLatitude)<=antAz<360 (Zone = CW)
*/
void trkCalZoneTypeInit(){
  double dec;
  double la;
  //double az;

  p.antZoneType=0;
  p.antZone=0;
  if(p.iaoflg==1 || p.iaoflg == 2){
    //! �V�̂̍��W�n��RADEC,LB���g�p���Ă����ꍇ�̂ݓK��
    dec=p.dgtdat[1][1]; //! MapCenter DEC rad
    //la=p.antPosLaLo[0];
    la=p.dgtdat[0][1]; //! Antenna Latitude rad
    if(la<=dec){
      p.antZoneType=3;
    }
    else{
      p.antZoneType = 0;
      //if(p.acuStat.zone == 2){ //!< CCW
      //p.antZoneType=2;
      //}
      //else{
      //p.antZoneType=4;
      //}
    }
  }
  uM1("trkCalZoneTypeInit(); Set antZoneType=%d", p.antZoneType);
  return;
}

/*! \fn void setParam()
\brief �p�����[�^�[�����擾����
*/
void setParam(){
  int i;
  char tmp[16];

  if(confSetKey("TrkInterval"))
    p.TrkInterval = atof(confGetVal());
  if(confSetKey("AntAzMinimum"))
    p.dazelmin[0] = atof(confGetVal());
  if(confSetKey("AntElMinimum"))
    p.dazelmin[1] = atof(confGetVal());
  if(confSetKey("AntAzMaximum"))
    p.dazelmax[0] = atof(confGetVal());
  if(confSetKey("AntElMaximum"))
    p.dazelmax[1] = atof(confGetVal());
  if(confSetKey("AntAzMarg"))
    p.AntAzElMarg[0] = atof(confGetVal());
  if(confSetKey("AntElMarg"))
    p.AntAzElMarg[1] = atof(confGetVal());
  if(1)
    p.AntAzElMargThrsd = 3; //!< �A���e�i���g���b�L���O���Ă���Ɣ��f���鋖�e�덷�ɓ������񐔂�臒l
  for(int i = 0; i < 7; i++){
    sprintf(tmp, "AntInst%d", i);
    if(confSetKey(tmp))
      p.AntInst[i] = confGetVal();
  }
  if(confSetKey("TrkTimeOffset"))
    p.TrkTimeOffset = atof(confGetVal());
  
  //if(confSetKey("RSkyInterval"))
  //  p.RSkyInterval = atoi(confGetVal());
  //if(confSetKey("Interval"))
  //  p.RSkyInterval = atoi(confGetVal());//!< �݊����̂��ߓ��ʐݒ肵�Ă����B
  
  //! On-Point
  if(confSetKey("ScanFlag"))
    p.ScanFlag = atoi(confGetVal());
  if(confSetKey("OnCoord"))
    p.OnCoord = atoi(confGetVal());
  if(confSetKey("PosAngle_Rad"))
    p.PosAngle_Rad = atof(confGetVal());
  if(confSetKey("LineTime"))
    p.LineTime = atof(confGetVal());
  //if(confSetKey("LinePathX_Rad"))
  //  p.LinePathX_Rad = atof(confGetVal());
  //if(confSetKey("LinePathY_Rad"))
  //  p.LinePathY_Rad = atof(confGetVal());

  if(confSetKey("SeqPattern")){ //!< �݊����̂��ߎc���Ă���
    if(p.SetPattern){
      free(p.SetPattern);
    }
    p.SetPattern = (char*) malloc(strlen(confGetVal()) + 1);
    strcpy(p.SetPattern, confGetVal());
  }
  if(confSetKey("SetPattern")){
    if(p.SetPattern){
      free(p.SetPattern);
    }
    p.SetPattern = (char*) malloc(strlen(confGetVal()) + 1);
    strcpy(p.SetPattern, confGetVal());
  }
  if(confSetKey("OnNumber")){
    p.OnNumber = atoi(confGetVal());
    if(p.OnNumber < 0){
      uM1("trk setParam(); invalid  p.OnNumber[%d] < 0 ans set 0", p.OnNumber);
      p.OnNumber = 0;
    }
    else{
      if(p.on){
	free(p.on);
      }
      if(p.linePath){
	free(p.linePath);
      }
      p.on = (double*)malloc(sizeof(p.on[0]) * p.OnNumber * 2);
      p.linePath = (double*)malloc(sizeof(p.linePath[0]) * p.OnNumber * 2);
      memset(p.on, 0, sizeof(p.on[0]) * p.OnNumber * 2);
      memset(p.linePath, 0, sizeof(p.linePath[0]) * p.OnNumber * 2);
      for(i = 0; i < p.OnNumber; i++){
	sprintf(tmp, "OnX%d_Rad", i+1);
	if(confSetKey(tmp)){
	  p.on[i*2] = atof(confGetVal()) * rad2sec;
	}
	sprintf(tmp, "OnY%d_Rad", i+1);
	if(confSetKey(tmp)){
	  p.on[i*2+1] = atof(confGetVal()) * rad2sec;
	}
	sprintf(tmp, "LinePathX%d_Rad", i+1);
	if(confSetKey(tmp)){
	  p.linePath[i*2] = atof(confGetVal());
	}
	sprintf(tmp, "LinePathY%d_Rad", i+1);
	if(confSetKey(tmp)){
	  p.linePath[i*2+1] = atof(confGetVal());
	}
      }
    }
  }

  //! Off-Point
  if(confSetKey("OffCoord"))
    p.OffCoord = atoi(confGetVal());
  if(confSetKey("OffNumber")){
    p.OffNumber = atoi(confGetVal());
    if(p.OffNumber < 0){
      uM1("trk setParam(); invalid p.OffNumber[%d] < 0 ans set 0", p.OffNumber);
      p.OffNumber = 0;
    }
    else{
      if(p.off){
	free(p.off);
      }
      p.off = (double*)malloc(sizeof(p.off[0]) * p.OffNumber * 2);
      memset(p.off, 0, sizeof(p.off[0]) * p.OffNumber * 2);
      for(i = 0; i < p.OffNumber; i++){
	sprintf(tmp, "OffX%d_Rad", i+1);
	if(confSetKey(tmp)){
	  p.off[i*2] = atof(confGetVal()) * rad2sec;
	}
	sprintf(tmp, "OffY%d_Rad", i+1);
	if(confSetKey(tmp)){
	  p.off[i*2+1] = atof(confGetVal()) * rad2sec;
	}
      }
    }
  }
  if(confSetKey("OffInterval"))
    p.OffInterval = atoi(confGetVal());
  if(confSetKey("OffMode"))
    p.OffMode = atoi(confGetVal());

  //! Source
  if(confSetKey("SourceFlag"))
    p.ioflg = atoi(confGetVal());  //!< �V�̃t���O 0:���z�n�O 1-9:�f�� 10:�� 11:���z 12:COMET 99:AZEL
  if(confSetKey("Coordinate"))
    p.iaoflg = atoi(confGetVal()); //!< �ʒu���W�n 0:�f�� 1:RADEC 2:LB 3:AZEL
  if(confSetKey("X_Rad"))
    p.dsourc[0] = atof(confGetVal());
  if(confSetKey("Y_Rad"))
    p.dsourc[1] = atof(confGetVal());
  if(confSetKey("Epoch"))
    p.ibjflg = atoi(confGetVal()); //!< ���_�t���O 1:B1950 2:J2000
  if(confSetKey("Velocity"))
    p.drdlv = atof(confGetVal());

  if(1)
    p.xin.irpflg = 1;              //!< �|�C���e�B���O 0:���p 1:���p
  if(1)
    p.xin.iapflg = 3;              //!< �|�C���e�B���O���W��`�����l 1:RADEC 2:LB 3:AZEL
  if(1)
    p.xin.irsflg = 1;              //!< �X�L�����j���O 0:���p 1:���p
  if(1)
    p.xin.iasflg = 1;              //!< �X�L�����j���O���W��`�����l 1:RADEC 2:LB 3:AZEL
  
  if(1)
    p.xin.itnscn = 1;              //!< �X�L�����j���O�_��
  if(1)
    p.xin.inoscn = 1;              //!< �X�L�����j���O�_

  //if(p.ioflg != 0 || p.ioflg != 99){                 //!< �ʒu���W�n 0:�f�� 1:RADEC 2:LB 3:AZEL
  //  p.iaoflg = 0;
  //}

  for(int i = 0; i < 6; i++){
    int j = i * 5;
    if(sscanf(p.AntInst[i+1], "%lf %lf %lf %lf %lf",
	      &p.P[j], &p.P[j+1], &p.P[j+2], &p.P[j+3], &p.P[j+4]) != 5){
      uM2("setParam(); AntInst[%d] (%s) error!!", i+1, p.AntInst[i+1]);
    }
  }
}

/*! \fn int checkParam()
\brief �p�����[�^�[�̃`�F�b�N
\retval 0 ����
\retval -1 ���s
*/
int checkParam(){
  if(p.TrkUse != 0){
    //! ACU�֘A
    if(p.TrkInterval < 0){
      uM1("TrkInterval(%lf) error!!", p.TrkInterval);
      return -1;
    }
    if(p.dazelmin[0] < 0 || p.dazelmin[0] > 360){
      uM1("AntAzMinimum(%lf) error!!", p.dazelmin[0]);
      return -1;
    }
    if(p.dazelmin[1] < 0 || p.dazelmin[1] > 90){
      uM1("AntElMinimum(%lf) error!!", p.dazelmin[1]);
      return -1;
    }
    if(p.dazelmax[0] < p.dazelmin[0] || p.dazelmax[0] > 360){
      uM1("AntAzMaximum(%lf) error!!", p.dazelmax[0]);
      return -1;
    }
    if(p.dazelmax[1] < p.dazelmin[1] || p.dazelmax[1] > 90){
      uM1("AntElMaximum(%lf) error!!", p.dazelmax[1]);
      return -1;
    }
    if(p.AntAzElMarg[0] < 0){
      uM1("AntAzMarg(%lf) error!!", p.AntAzElMarg[0]);
      return -1;
    }
    if(p.AntAzElMarg[1] < 0){
      uM1("AntElMarg(%lf) error!!", p.AntAzElMarg[1]);
      return -1;
    }

    //! �ϑ��֘A
    if(p.ioflg < 0 || (p.ioflg > 12 && p.ioflg != 99)){
      uM1("SourceFlag(%d) error!!", p.ioflg);
      return -1;
    }
    if(p.iaoflg < 0 || p.iaoflg > 3){
      uM1("Coorinate(%d) error!!", p.iaoflg);
      return -1;
    }
    if(p.ibjflg < 1 || p.ibjflg > 2){
      uM1("Epoch(%d) error!!", p.ibjflg);
      return -1;
    }
    if(!((p.ScanFlag >= 1 && p.ScanFlag <= 6)
	 || (p.ScanFlag >= 1001 && p.ScanFlag <= 1002))){
      uM1("ScanFlag(%d) error!!", p.ScanFlag);
      return -1;
    }
    if(p.dsourc[0] < 0.0 || p.dsourc[0] > 2.0*PI){
      uM1("X_Rad(%lf) error!!", p.dsourc[0]);
      return -1;
    }
    if(p.dsourc[1] < -PI/2.0 || p.dsourc[1] > PI/2.0){
      uM1("Y_Rad(%lf) error!!", p.dsourc[1]);
      return -1;
    }
    if(p.ScanFlag >= 1 && p.ScanFlag <= 6){
      if(p.OffNumber < 1 && p.OffNumber > 2){
	uM1("checkParam(); OffNumber %d error!!.", p.OffNumber);
	return -1;
      }
      if(p.ScanFlag == 1 && p.ScanFlag == 3 && p.ScanFlag == 4){
	//! On,Grid,Random
	if(p.OffInterval <= 0){
	  uM1("checkParam(); OffInterval[%d] error!!", p.OffInterval);
	  return -1;
	}
      }
      if(!p.SetPattern){
	uM1("ScanFlag(%d) SetPattern error!!", p.ScanFlag);
	return -1;
      }
      if(p.OnNumber <= 0){
	uM2("ScanFlag(%d) OnNumber(%d) error!!", p.ScanFlag, p.OnNumber);
	return -1;
      }
      if(p.OffCoord < 1 || p.OffCoord > 3){
	uM1("OffCoord(%d) error!!", p.OffCoord);
	return -1;
      }
    }
    else if(p.ScanFlag == 1001){
      //! Source Tracking
    }
    if(p.LineTime < 0){
      uM1("LineTime(%lf) error!!", p.LineTime);
      return -1;
    }
  }
  return 0;
}

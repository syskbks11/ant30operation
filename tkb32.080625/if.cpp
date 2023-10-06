/*!
\file if.cpp
\author Y.Koide
\date 2006.11.28
\brief IF����p
*/
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>

#include "../libtkb/export/libtkb.h"
#include "trk45Sub.h"
#include "errno.h"
#include "if.h"

static const double loFreqThrsh1 = 1.0e-9; //!< ���e�덷[GHz]
static const double loAmpThrsh1 = 0.01;    //!< ���e�덷[dB]
static const double attThrsh1 = 0.01;      //!< ���e�덷[dB]
static const double loFreqThrsh2 = 1.0e-9; //!< ���e�덷[GHz]
static const double loAmpThrsh2 = 0.01;    //!< ���e�덷[dB]

//! 1stIF�ւ̃��N�G�X�g�\����
typedef struct sReqIf1{
  double reqLo1Freq;    //!< 1st Local Frequency [GHz]
  double reqLo1Amp;     //!< 1st Local Amplitude [dBm]
  double reqAtt1;       //!< 1st IF Step Att [-dB]
}tReqIf1;

//! 1stIF����̉����\����
typedef struct sAnsIf1{
  double ansLo1Freq;    //!< [GHz]
  double ansLo1Amp;     //!< [dBm]
  double ansAtt1;       //!< [-dB]
}tAnsIf1;

//! 2ndIF�ւ̃��N�G�X�g�\����
typedef struct sReqIf2{
  double reqLo2Freq;    //!< 2nd Local Frequency [GHz]
  double reqLo2Amp;     //!< 2nd Local Amplitude [dBm]
}tReqIf2;

//! 2ndIF����̉����\����
typedef struct sAnsIf2{
  double ansLo2Freq;    //!< [GHz]
  double ansLo2Amp;     //!< [dBm]
}tAnsIf2;

typedef struct sParamIf{
  //! �ڑ��p�p�����[�^
  int use1;             //!< If1���g�p���邩 0:���g�p 1:�g�p 2:�ʐM�ȊO�g�p
  int use2;             //!< If2���g�p���邩 0:���g�p 1:�g�p 2:�ʐM�ȊO�g�p
  char if1ip[256];      //!< 1stIF IP
  char if2ip[256];      //!< 2ndIF IP
  int  if1port;         //!< 1stIF Port
  int  if2port;         //!< 2ndIF Port

  //! �ϐ�
  void* vtm;            //!< �X�V���Ԃ̊Ǘ�
  void* net1;
  void* net2;
  int OnOffR;           //!< ���݊ϑ��Ώ� 0:On 1:Off 2:R
  double att;           //!< StepAtt ���݂̎w�ߒl [dB]

  //! �ϑ��p�����[�^
  double lo1amp;        //!< Local1 Amplitude [dBm]
  double lo2amp;        //!< Local2 Amplitude [dBm]
  double IfAtt;        //!< StepAtt at OnOff-Point [dB]
  double IfAttR;          //!< StepAtt at R [dB]
  double FifL;          //!< ���IF(Left)�o�͂̐M�����S���g�� [GHz]
  double fcenter;       //!< �ϑ�������g��Frf [Hz]
  //double safcenter;     //!< �����v�̒��S���g�� [Hz]
  int  if1Interval;     //!< IF������X�V���鎞�ԊԊu [sec]
  double trkTimeOffset; //!< ���b��̒ǔ��v�Z���s���� [sec]

  //! �ǔ����C�u�����p�p�����[�^
  int ioflg;            //!< �V�̃t���O 0:���z�n�O 1-11:�f�� 12:COMET 99:AZEL
  int iaoflg;           //!< �ʒu���W�n 0:�f�� 1:RADEC 2:LB 3:AZEL
  int ibjflg;           //!< ���_�t���O 1:B1950 2:J2000
  double drdlv;         //!< �V�̂̎������x [m/s]
  double dsourc[2];     //!< �V�̈ʒu [rad]

  //! �e��ϐ�
  XTRKI xin;            //!< �I�t�Z�b�g�ݒ�
  //double doff[2];       //!< OFF�_�p
  char cstart[24];      //!< �ϑ������̐ݒ� YYYYMMDDhhmmss.0 (JST)

  XTRKO xout;           //!< �A���e�i���x�Ȃ�DAZEL
  double dvrad;         //!< trk_velo�p�ϐ�
  double lo1freq;       //!< Lo1���g���o�͒l [GHz]
  double lo2freq;       //!< Lo2���g���o�͒l [GHz]
  tAnsIf1 aIf1;         //!< IF1����̉����f�[�^
  tAnsIf2 aIf2;         //!< IF2����̉����f�[�^

  int dayFlg;           //!< trk_00()���Ăяo���^�C�~���O�p�BUT�ɂ�������t���ς�����Ƃ��ĂԁB
}tParamIf;

static int start=0;
static tParamIf p;

static int ifCal();
static int ifNet();

static void setParam();
static int checkParam();

/*! \fn int ifInit()
\retval 0 ����
\retval 0�ȊO �G���[�R�[�h
*/
int ifInit(){
  //char cerr[256];

  memset(&p, 0, sizeof(p));
  start = 0;

  //! ����v���O�����ւ̐ڑ��p�����[�^�̂ݎ擾
  if(confSetKey("IfUse01"))
    p.use1 = atoi(confGetVal());
  if(confSetKey("IfUse02"))
    p.use2 = atoi(confGetVal());
  if(confSetKey("IfIp01"))
    strcpy(p.if1ip, confGetVal());
  if(confSetKey("IfIp02"))
    strcpy(p.if2ip, confGetVal());
  if(confSetKey("IfPort01"))
    p.if1port = atoi(confGetVal());
  if(confSetKey("IfPort02"))
    p.if2port = atoi(confGetVal());

  //! �p�����[�^�`�F�b�N
  if(p.use1 < 0 || p.use1 > 2){
    uM1("IfUse01(%d) error!!\n", p.use1);
    return -1;
  }
  if(p.use2 < 0 || p.use2 > 2){
    uM1("IfUse02(%d) error!!\n", p.use2);
    return -1;
  }

  if(p.use1 == 1){
    if(!confSetKey("IfIp01")){
      uM1("IfUse01(%d) IfIp01() error!!\n", p.use1); 
      return -1;
    }
    if(p.if1port <= 0){
      uM2("IfUse01(%d) IfPort01(%d) error!!\n", p.use1, p.if1port);
      return -1;
    }
  }
  if(p.use2 == 1){
    if(!confSetKey("IfIp02")){
      uM1("IfUse02(%d) IfIp02() error!!\n", p.use2);
      return -1;
    }
    if(p.if2port <= 0){
      uM2("IfUse02(%d) IfPort02(%d) error!!\n", p.use2, p.if2port);
      return -1;
    }
  }
  
  if(p.use1 == 1){
    p.net1 = netclInit(p.if1ip, p.if1port);
    if(p.net1 == NULL){
      uM("ifInit(); netclInit(); if2 error");
      return IF_NET_ERR;
    }
  }
  if(p.use2 == 1){  
    p.net2 = netclInit(p.if2ip, p.if2port);
    if(p.net2 == NULL){
      uM("ifInit(); netclInit(); if2 error");
      return IF_NET_ERR;
    }
  }
  p.vtm = tmInit();

  return 0;
}

/*! \fn int ifStart()
\brief �V�����ϑ����s���ۂɍŏ��ɌĂяo���B
\brief �ϑ��p�����[�^�̏��������s���B
*/
int ifStart(){
  char cerr[256];

  //! �ϑ��p�����[�^�̏�����
  p.lo1amp = 0;
  p.lo2amp = 0;
  p.IfAtt = 0;
  p.IfAttR = 0;
  p.FifL = 0;
  p.fcenter = 0;
  p.if1Interval = 0;
  p.trkTimeOffset = 0;
  p.ioflg = 0;
  p.iaoflg = 0;
  p.ibjflg = 0;
  p.drdlv = 0;
  memset(p.dsourc, 0, sizeof(p.dsourc[0]) * 2);
  memset(&p.xin, 0, sizeof(p.xin));
  
  //! �e��ϐ�������
  start = 0;
  memset(p.cstart, 0, sizeof(p.cstart[0]) * 24);
  memset(&p.xout, 0, sizeof(p.xout));
  p.dvrad = 0;
  p.lo1freq = 0;
  p.lo2freq = 0;
  memset(&p.aIf1, 0, sizeof(p.aIf1));
  memset(&p.aIf2, 0, sizeof(p.aIf2));
  p.att = 0;
  
  //! �ϑ��p�����[�^���Z�b�g����
  setParam();
  if(checkParam()){
    uM("ifInit(); checkParam(); error");
    return IF_PARAM_ERR;
  }

  //! �ǔ����C�u�����̏�����
  tmReset(p.vtm);
  strcpy(p.cstart, tmGetTimeStr(p.vtm, p.trkTimeOffset));
  if(trk_00(p.cstart, "", cerr) != 0) {
    uM1("ifStart(); %s",cerr);
    return IF_TRK00_ERR;
  }
  p.dayFlg = 1;

  return 0;
}


/*! \fn int ifEnd()
\brief �I�����ɌĂяo��
\retval 0 ����
*/
int ifEnd(){
  if(p.vtm){
    tmEnd(p.vtm);
    p.vtm = NULL;
  }
  if(p.net1){
    netclEnd(p.net1);
    p.net1 = NULL;
  }
  if(p.net2){
    netclEnd(p.net2);
    p.net2 = NULL;
  }
  return 0;
}

/*! \fn int ifRepeat()
\brief IF������s���B���[�J�����g���̍X�V�A�X�e�b�v�A�b�e�l�[�^�[�̐ݒ�B
\brief �Œ�X�V�Ԋu(p.if1Interval)�𒴂��Ă��Ȃ��ꍇ�͍X�V���Ȃ��B
\retval 0 ����
\retval 0�ȊO �G���[�R�[�h
*/
int ifRepeat(){
  //char tmp[1024];
  int ret;

  if(p.use1 == 0 && p.use2 == 0)
    return IF_NOT_USE;
  
  //! ����v���O�����֑��M
  ret = ifNet();
  if(ret){
    uM1("ifRepeat(); ifNet(); return [%d] error!!", ret);
    return ret;
  }

  //! �w�ߒl�ǂ���ɐݒ肳��Ă��邩�m�F
  if(p.use1 == 1){
    if(fabs(p.aIf1.ansLo1Freq - p.lo1freq) >= loFreqThrsh1){
      uM2("ifRepeat(); 1st Lo Freq req[%.10lf] ans[%.10lf] error!!",
	  p.lo1freq, p.aIf1.ansLo1Freq);
      ret = IF_STATE_ERR;
    }
    if(fabs(p.aIf1.ansLo1Amp - p.lo1amp) >= loAmpThrsh1){
      uM2("ifRepeat(); 1st Lo Amp req[%.2lf] ans[%.2lf] error!!",
	  p.lo1amp, p.aIf1.ansLo1Amp);
      ret = IF_STATE_ERR;
    }
    if(fabs(p.aIf1.ansAtt1 - p.att) >= attThrsh1){
      uM2("ifRepeat(); StepAtt req[%.2lf] ans[%.2lf] error!!",
	  p.aIf1.ansAtt1, p.att);
      ret = IF_STATE_ERR;
    }
  }

  if(p.use2 == 1){
    if(fabs(p.aIf2.ansLo2Freq - p.lo2freq) >= loFreqThrsh2){
      uM2("ifRepeat(); 2nd Lo Freq req[%.10lf] ans[%.10lf] error!!",
	  p.lo2freq, p.aIf2.ansLo2Freq);
      ret = IF_STATE_ERR;
    }
    if(fabs(p.aIf2.ansLo2Amp - p.lo2amp) >= loAmpThrsh2){
      uM2("ifRepeat(); 2nd Lo Amp req[%.2lf] ans[%.2lf] error!!",
	  p.lo2amp, p.aIf2.ansLo2Amp);
      ret = IF_STATE_ERR;
    }
  }

  return ret;
}

/*! \fn int ifUpdate(const int OnOffR)
\breif ���g���ǔ��̒l���X�V����
\return ifCal()�̖߂�l
*/
int ifUpdate(){
  int ret;

  //! if1Interval�b�ȉ��������珈�����Ȃ�
  if(start != 0 && tmGetLag(p.vtm) < p.if1Interval){
    ifRepeat();
    return 0;
  }

  //! ���g���ǔ����v�Z
  uLock();
  ret = ifCal();
  uUnLock();

  if(ret){
    uM1("ifUpdate(); ifCal(); return [%d] error!!", ret);
    return ret;
  }
  uM3("ifUpdate(); Fobs %+10.9e Hz,  Fdop %+10.9e Hz,  Vrad %+10.9lf m/s",
      p.xin.dobsfq, p.xout.dflkfq, p.dvrad);

  //! ���M
  ret = ifRepeat();
  uM1("ifUpdate(); DateTime %s", p.cstart);
  uM3("Request Lo1(%.9lf GHz, %.1lf dBm), Att %4.1lf dB", p.lo1freq, p.lo1amp, p.att);
  uM2("Request Lo2(%.9lf GHz, %.1lf dBm)", p.lo2freq, p.lo2amp);
  uM3("Answer  Lo1(%.9lf GHz, %4.1lf dBm), Att %4.1lf dB", p.aIf1.ansLo1Freq, p.aIf1.ansLo1Amp, p.aIf1.ansAtt1);
  uM2("Answer  Lo2(%.9lf GHz, %4.1lf dBm)", p.aIf2.ansLo2Freq, p.aIf2.ansLo2Amp);

  //! �G���[���Ȃ���΃^�C�}�[�����Z�b�g����B
  if(ret == 0){
    start = 1;
    tmReset(p.vtm);
  }
  return ret;
}

/*! \fn int ifCal()
\brief �h�b�v���[�ǔ��̎��g���v�Z���s��
\retval 0 ����
\retval 0�ȊO �G���[�R�[�h
*/
int ifCal(){
  //! DOPPLER SHIFT�̎擾
  //! trk_20�p�ϐ�
  double dtime;

  //! ��Ɨp�ϐ�
  char cerr[256];
  int  ival;


  //! UT�ɂ����ē��ɂ����ς���Ă�����trk_00�����s����
  strcpy(p.cstart, tmGetTimeStr(p.vtm, p.trkTimeOffset));
  if(p.cstart[8] == '0' && p.cstart[9] == '9'){           //!< JST�����Ƃ̔�r
    if(p.dayFlg == 0){
      uM1("ifCal(); trk_00(\"%s\")", p.cstart);
      if(trk_00(p.cstart, "", cerr) != 0) {
	uM1("ifCal(); %s",cerr);
	return IF_TRK00_ERR;
      }
      p.dayFlg = 1;
    }
  }
  else{
    p.dayFlg = 0;
  }

  //! trk_10
  //! ��Q�����͏����t���O 3:DOPPLER
  ival = trk_10(p.ioflg, 3, p.iaoflg, p.ibjflg, p.dsourc, p.drdlv);
  if (ival == 1) {
    uE("trk_10: Argument check error");
    return IF_TRK10_ARG_ERR;
  } else if (ival == 2) {
    uE("trk_10: Common area initial error");
    return IF_TRK10_COMAREA_ERR;
  }

  if (tjcd2m(p.cstart, &dtime) == 1) {  //!< JST���n�̗���t����MJD�ւ̕ϊ�
    uM("tjcd2m: Error");
    return IF_TJCD2M_ERR;
  }
  ival = trk_20(dtime, p.xin, &p.xout);
  if (ival == -1) {
    uM("trk_20: Argument check error");
    return IF_TRK20_ARG_ERR;
  } else if (ival == -2) {
    uM("trk_20: Local error end");
    return IF_TRK20_LOCAL_ERR;
  }

  //! VRAD (�A���e�i���猩���������x) �̎Z�o
  trk_velo_(&p.dvrad);

  p.lo1freq      = p.xout.dflkfq / 1.0e9 - p.FifL; //!< [GHz]
  p.lo2freq      = p.FifL - p.fcenter; //!< [GHz]
  return 0;
}

/*! \fn int ifNet()
\breif ����v���O�����ƒʐM���s��
\retval 0 ����
\retval 0�ȊO �G���[�R�[�h
*/
int ifNet(){
  int ret;
  int err;

  tReqIf1 rIf1;
  tReqIf2 rIf2;
  tAnsIf1 aIf1;
  tAnsIf2 aIf2;

  rIf1.reqLo1Freq = p.lo1freq;         //!< [GHz]
  rIf1.reqLo1Amp  = p.lo1amp;          //!< [dBm]
  rIf1.reqAtt1    = p.att;             //!< [dB]
  rIf2.reqLo2Freq = p.lo2freq;         //!< [GHz]
  rIf2.reqLo2Amp  = p.lo2amp;          //!< [dBm]

  err = 0;
  //! ��ꒆ�Ԏ��g��������v���O����
  if(p.use1 == 1){
    ret = netclWrite(p.net1, (const unsigned char*)&rIf1, sizeof(rIf1));
    if(ret < 0){
      uM1("ifNet(); netclWrite(); IF1 network error = %d", ret);
      err = IF_NET_ERR;
    }
    else if(ret != sizeof(rIf1)){
      uM1("ifNet(); netclWrite(); IF1 send size error = %d", ret);
      err = IF_NET_DAT_ERR;
    }
    else{
      ret = netclRead(p.net1, (unsigned char*)&aIf1, sizeof(aIf1));
      if(ret < 0){
	uM1("ifNet(); netclRead(); IF1 network error = %d", ret);
	err = IF_NET_ERR;
      }
      else if(ret != sizeof(aIf1)){
	uM1("ifNet(); netclRead(); IF1 receive size error = %d", ret);
	err = IF_NET_DAT_ERR;
      }
      else{
	memcpy(&p.aIf1, &aIf1, sizeof(p.aIf1));
      }
    }
  }

  //! ��񒆊Ԏ��g��������v���O����
  if(p.use2 == 1){
    //netuCnvLong((unsigned char*)&rIf2.reqLo2Freq);
    //netuCnvLong((unsigned char*)&rIf2.reqLo2Amp);
    ret = netclWrite(p.net2, (const unsigned char*)&rIf2, sizeof(rIf2));
    if(ret < 0){
      uM1("ifNet(); netclWrite(); IF2 network error = %d", ret);
      err = IF_NET_ERR;
    }
    else if(ret != sizeof(rIf2)){
      uM1("ifNet(); netclWrite(); IF2 send size error = %d", ret);
      err = IF_NET_DAT_ERR;
    }
    else{
      ret = netclRead(p.net2, (unsigned char*)&aIf2, sizeof(aIf2));
      if(ret < 0){
	uM1("ifNet(); netclRead(); IF2 network error = %d", ret);
	err = IF_NET_ERR;
      }
      else if(ret != sizeof(aIf2)){
	uM1("ifNet(); netclRead(); IF2 receive size error = %d", ret);
	err = IF_NET_DAT_ERR;
      }
      else{
	//netuCnvLong((unsigned char*)&aIf2.ansLo2Freq);
	//netuCnvLong((unsigned char*)&aIf2.ansLo2Amp);
	//aIf2.ansLo2Freq /= 1000 * 1000 * 1000;
	memcpy(&p.aIf2, &aIf2, sizeof(p.aIf2));
      }
    }
  }
  return err;
}

/*! \fn void ifSetLo1Freq(double Freq)
\brief 1stIF�̃��[�J�����g����ݒ肷��
\param[in] Freq [GHz]
*/
void ifSetLo1Freq(double Freq){
  uM1("ifSetLo1Freq(); Set p.lo1Freq = %.10e [GHz]", Freq);
  p.lo1freq = Freq;
  return;
}

/*! \fn void ifSetLo1Amp(double Amp)
\brief 1stIF�̃��[�J���̏o�͒l��ݒ肷��B
\param[in] Amp [dBm]
*/
void ifSetLo1Amp(double Amp){
  uM1("ifSetLo1Amp(); Set p.lo1amp = %lf [dBm]", Amp);
  p.lo1amp = Amp;
  return;
}

/*! \fn void ifSetLo1Att(double Att)
\brief 1stIF�̃A�b�e�l�[�^�[��ݒ肷��
\param[in] Att�K���}�C�i�X�l���Z�b�g���邱�� [dB]
*/
void ifSetLo1Att(double Att){
  if(Att > 0){
    uM1("ifSetLo1Att(); Att[%d] > 0 error!!", Att);
    return;
  }
  uM2("ifSetLo1Att(); Set p.att = %lf (OnOffR = %d)", Att, p.OnOffR);

  //! �Y������X�L�����̌����l���㏑������
  if(p.OnOffR == 0 || p.OnOffR == 1){
    p.IfAtt = Att;
  }
  else{
    p.IfAttR = Att;
  }

  return;
}

/*! \fn void ifSetLo2Freq(double Freq)
\brief 2ndIF�̃��[�J�����g����ݒ肷��
\param[in] Freq [GHz]
*/
void ifSetLo2Freq(double Freq){
  uM1("ifSetLo2Freq(); Set p.lo2Freq = %.10e [GHz]", Freq);
  p.lo2freq = Freq;
  return;
}

/*! \fn void ifSetLo2Amp(double Amp)
\brief 2ndIF�̃��[�J���̏o�͒l��ݒ肷��
\param[in] Amp [dBm]
*/
void ifSetLo2Amp(double Amp){
  uM1("ifSetLo2Amp(); Set p.lo2amp = %lf [dBm]", Amp);
  p.lo2amp = Amp;
  return;
}

/*! \fn int ifSetAttOnOffR(int OnOffR)
\brief �ό�����̒l��OnOff��R�Ő؂�ւ���
\param[in] OnOffR 0:On 1:Off 2:R
\return 0:Success 1:Fault
 */
int ifSetAttOnOffR(int OnOffR){
  //! �����`�F�b�N
  if(OnOffR < 0 || OnOffR > 2){
    uM1("ifSetAttOnOffR(%d) error!!", OnOffR);
    return -1;
  }

  p.OnOffR = OnOffR;

  if(OnOffR == 0 || OnOffR == 1){
    //! OnOff
    p.att = p.IfAtt;
  }
  else{
    //! R
    p.att = p.IfAttR;
  }
  uM2("ifSetAttOnOffR(); Set OnOffR=%d StepAtt=%.1lf [dB]", OnOffR, p.att);
  return 0;
}

/*! \fn void ifGetLo1PFreq(double* Lo1PFreq)
\brief 1stIF�̃��[�J���̎��g�����擾����(�w�ߒl)
\param[out] Lo1PFreq [GHz]
*/
void ifGetLo1PFreq(double* Lo1PFreq){
  *Lo1PFreq=p.lo1freq; //!< GHz
  return;
}

/*! \fn void ifGetLo1PAmp(double* Lo1PAmp)
\brief 1stIF�̃��[�J���̏o�͒l���擾����(�w�ߒl)
\param[out] Lo1PAmp [dBm]
*/
void ifGetLo1PAmp(double* Lo1PAmp){
  *Lo1PAmp=p.lo1amp;
  return;
}

/*! \fn void ifGetLo1PAtt(double* Lo1PAtt)
\brief 1stIF�̃X�e�b�v�A�b�e�l�[�^�[�̌����l���擾����(�w�ߒl)
\param[out] Lo1PAtt [dB]
*/
void ifGetLo1PAtt(double* Lo1PAtt){
  *Lo1PAtt=p.att;
  return;
}

/*! \fn void ifGetLo1RFreq(double* Lo1RFreq)
\brief 1stIF�̃��[�J���̎��g�����擾����(���ےl)
\param[out] Lo1RFreq [GHz]
*/
void ifGetLo1RFreq(double* Lo1RFreq){
  *Lo1RFreq = p.aIf1.ansLo1Freq; //!< GHz
  return;
}

/*! \fn void ifGetLo1RAmp(double* Lo1RAmp)
\brief 1stIF�̃��[�J���̏o�͒l���擾����(���ےl)
\param[out] Lo1RAmp [dBm]
*/
void ifGetLo1RAmp(double* Lo1RAmp){
  *Lo1RAmp = p.aIf1.ansLo1Amp;
  return;
}

/*! \fn void ifGetLo1RAtt(double* Lo1RAtt)
\brief 1stIF�̃X�e�b�v�A�b�e�l�[�^�[�̌����l���擾����(���ےl)
\brief �X�e�b�v�A�b�e�l�[�^�[������ےl���擾�ł��Ȃ��̂ŁA���݂͎w�ߒl�Ɠ����B
\param[out] Lo1RAtt [dB]
*/
void ifGetLo1RAtt(double* Lo1RAtt){
  *Lo1RAtt = p.aIf1.ansAtt1;
  return;
}

/*! \fn void ifGetLo2PFreq(double* Lo2PFreq)
\brief 2ndIF�̃��[�J���̎��g�����擾����(�v�Z�l)
\param[out] Lo2PFreq [GHz]
*/
void ifGetLo2PFreq(double* Lo2PFreq){
  *Lo2PFreq = p.lo2freq; //!< GHz
  return;
}

/*! \fn void ifGetLo2PAmp(double* Lo2PAmp)
\brief 2ndIF�̃��[�J���̏o�͒l���擾����(�w�ߒl)
\param[out] Lo2PAmp [dBm]
*/
void ifGetLo2PAmp(double* Lo2PAmp){
  *Lo2PAmp=p.lo2amp;
  return;
}

/*! \fn void ifGetLo2RFreq(double* Lo2RFreq)
\brief 2ndIF�̃��[�J���̎��g�����擾����(���ےl)
\param[out] Lo2RFreq [GHz]
*/
void ifGetLo2RFreq(double* Lo2RFreq){
  *Lo2RFreq = p.aIf2.ansLo2Freq; //!< GHz
  return;
}

/*! \fn void ifGetLo2RAmp(double* Lo2RAmp)
\brief 2ndIF�̃��[�J���̏o�͒l���擾����(���ےl)
\param[out] Lo2RAmp [dBm]
*/
void ifGetLo2RAmp(double* Lo2RAmp){
  *Lo2RAmp = p.aIf2.ansLo2Amp;
  return;
}

/*! \fn void ifGetVRAD(double* VRAD)
\brief VRAD���擾����
\param[out] VRAD [km/s]
*/
void ifGetVRAD(double* VRAD){
  *VRAD=p.dvrad / 1.0e3; //!< km/s
  return;
}

/*! \fn void setParam()
\brief �p�����[�^�[�����擾����
*/
void setParam(){
  //if(confSetKey("useIf1"))
  //  p.use1 = atoi(confGetVal());
  //if(confSetKey("useIf2"))
  //  p.use2 = atoi(confGetVal());
  //if(confSetKey("if1ip"))
  //  strcpy(p.if1ip, confGetVal());
  //if(confSetKey("if2ip"))
  //  strcpy(p.if2ip, confGetVal());
  //if(confSetKey("if1port"))
  //  p.if1port = atoi(confGetVal());
  //if(confSetKey("if2port"))
  //  p.if2port = atoi(confGetVal());
  if(confSetKey("Lo1Amp"))
    p.lo1amp = atof(confGetVal());
  if(confSetKey("Lo2Amp"))
    p.lo2amp = atof(confGetVal());
  if(confSetKey("IfAtt"))
    p.IfAtt = atof(confGetVal());
  if(confSetKey("IfAttR"))
    p.IfAttR = atof(confGetVal());

  if(confSetKey("FifL"))
    p.FifL = atof(confGetVal());            //!< GHz
  //if(confSetKey("Fif"))
  //  p.FifL = atof(confGetVal());    //!< �݊����̂��ߓ��ʎc���Ă���
  if(confSetKey("FsaL_H"))
    p.fcenter = atof(confGetVal());         //!< GHz
  //if(confSetKey("SaFcenter_H"))
  //  p.safcenter = atof(confGetVal());     //!< Hz
  if(confSetKey("LoInterval"))
    p.if1Interval = atoi(confGetVal());
  if(confSetKey("trkTimeOffset"))
    p.trkTimeOffset = atof(confGetVal());

  if(confSetKey("SourceFlag"))
    p.ioflg = atoi(confGetVal());           //!< �V�̃t���O 0:���z�n�O 1-9:�f�� 10:�� 11:���z 12:COMET 99:AZEL
  if(confSetKey("Coordinate")){
    if(p.ioflg == 0){
      p.iaoflg = atoi(confGetVal());        //!< �ʒu���W�n 0:�f�� 1:RADEC 2:LB 3:AZEL
    }
    else{
      p.iaoflg = 0;
    }
  }
  if(confSetKey("Epoch"))
    p.ibjflg = atoi(confGetVal());          //!< ���_�t���O 1:B1950 2:J2000
  if(confSetKey("Velocity"))
    p.drdlv = atof(confGetVal()) * 1.0e3;   //!< �V�̂̎������x [m/s]
  if(confSetKey("X_Rad"))
    p.dsourc[0] = atof(confGetVal());
  if(confSetKey("Y_Rad"))
    p.dsourc[1] = atof(confGetVal());

  if(1)
    p.xin.irpflg = 1;                       //!< �|�C���e�B���O 0:���p 1:���p
  if(1)
    p.xin.iapflg = 3;                       //!< �|�C���e�B���O���W��`�����l 1:RADEC 2:LB 3:AZEL
  if(1)
    p.xin.irsflg = 1;                       //!< �X�L�����j���O 0:���p 1:���p
  if(1){
    /*! �X�L�����j���O�̍��W��`�̏����l��RADEC���g�p����
     *  if.c�ł̓X�L���j���O�̍��W��`�͏����l�������Ɨp����
     */
    p.xin.iasflg = 1;
  }
  if(1)
    p.xin.itnscn = 1;                       //!< �X�L�����j���O�_��
  if(1)
    p.xin.inoscn = 1;                       //!< �X�L�����j���O�_
  if(confSetKey("Definition"))
    p.xin.ivdef = atoi(confGetVal());
  if(confSetKey("Frame"))
    p.xin.ivref = atoi(confGetVal());
  if(confSetKey("Ftrack"))
    p.xin.dobsfq = atof(confGetVal()) * 1.0e9; //!< �Î~�g���b�L���O���g�� [Hz]
}

/*! \fn int checkParam()
\brief �p�����[�^�[�̃`�F�b�N
\retval 0 ����
\retval -1 ���s
*/
int checkParam(){
  //if(p.use1 < 0 || p.use1 > 2){
  //  uM1("useIf1(%d) error!!\n", p.use1);
  //  return -1;
  //}
  //if(p.use2 < 0 || p.use2 > 2){
  //  uM1("useIf2(%d) error!!\n", p.use2);
  //  return -1;
  //}

  //if(p.use1 == 1){
  //  if(!confSetKey("if1ip")){
  //    uM1("useIf1(%d) if1ip() error!!\n", p.use1); 
  //    return -1;
  //  }
  //  if(p.if1port <= 0){
  //    uM2("useIf1(%d) if1port(%d) error!!\n", p.use1, p.if1port);
  //    return -1;
  //  }
  //}
  //if(p.use2 == 1){
  //  if(!confSetKey("if2ip")){
  //    uM1("useIf2(%d) if2ip() error!!\n", p.use2);
  //    return -1;
  //  }
  //  if(p.if2port <= 0){
  //    uM2("useIf2(%d) if2port(%d) error!!\n", p.use2, p.if2port);
  //    return -1;
  //  }
  //}
  if(p.use1 != 0){
    //if(p.lo1amp < -30 || p.lo1amp > 10)
    //  return -1;
    //if(p.att1 < -11 || p.att1 > 0){
    //  uM2("useIf1(%d) Att_H(%d) error!!\n", p.use1, p.att);
    //  return -1;
    //}
  }
  if(p.use2 != 0){
    //if(p.lo2amp < -30 || p.lo1amp > 10)
    //  return -1;
  }

  if(p.use1 != 0 || p.use2 != 0){
    //if(p.FifL < 4 || p.FifL > 6)
    //  return -1;
    if(p.if1Interval < 0){
      uM2("IfUse1(%d) LoInterval(%d) error!!\n", p.use1, p.if1Interval);
      return -1;
    }
    if(!((p.ioflg >= 0 && p.ioflg <= 12) || p.ioflg == 99)){
      uM2("IfUse(%d) SourceFlag(%d) error!!\n", p.use1, p.ioflg);
      return -1;
    }
    if(p.iaoflg < 0 || p.iaoflg > 3){
      uM2("IfUse1(%d) Coordinate(%d) error!!\n", p.use1, p.iaoflg);
      return -1;
    }
    if(p.ibjflg < 1 || p.ibjflg > 2){
      uM2("IfUse1(%d) Epoch(%d) error!!\n", p.use1, p.ibjflg);
      return -1;
    }
    if(p.xin.ivdef < 1 || p.xin.ivdef > 2){
      uM2("IfUse1(%d) Definition(%d) error!!\n", p.use1, p.xin.ivdef);
      return -1;
    }
    if(p.xin.ivref < 1 || p.xin.ivref > 2){
      uM2("IfUse1(%d) Frame(%d) error!!\n", p.use1, p.xin.ivref);
      return -1;
    }
    //if(p.xin.dobsfq < || p.xin.dobsfq > )
    //  return -1;
  }
  return 0;
}

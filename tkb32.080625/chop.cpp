/*!
\file chop.cpp
\author Y.Koide
\date 2006.11.20
\brief Chopper Wheel Function
*/
#include <stdlib.h>
#include <memory.h>

#include "../libtkb/export/libtkb.h"
#include "errno.h"
#include "chop.h"

const double ABS_ZERO = 273.15;

//! ���x�r�����u�ւ̃��N�G�X�g�\����
typedef struct sReqChop{
  int reqSet;        //!< ����R�}���h 0:none, 1:status, 2:close, 3:open, 4:origin, 5:stop
}tReqChop;

//! ���x�r�����u����̉����\����
typedef struct aAnsChop{
  int ansState;      //!< ��� 0:none, 1:close, 2:open(origin), 3:No Signal
  int ansMotor;      //!<  ���[�^�̏�� 0:none, 1:close�։�]��, 2:open, 3:origin�։�]��, 4:stop
  double ansTemp1;   //!< ���x�vCh1  -300:Error, other(>-273):��
  double ansTemp2;   //!< ���x�vCh2(Chopper)  -300:Error, other(>-273):��
  int ansInterlock;  //!< �C���^�[���b�N 0:OFF, 1:ON
  int ansError;      //!< �G���[�f�[�^(�\��) 0:No Error
}tAnsChop;

//! Chop�̃p�����[�^�[�\����
typedef struct sParamChop{
  int ChopUse;       //!< chop���g�p���邩 0:�g�p���Ȃ�, 1:�g�p, 2:�ʐM�ȊO�g�p
  char ChopIp[256];  //!< ����v���O������IP
  int ChopPort;      //!< ����v���O������Port
  int reqSet;        //!< ����v���O�����ւ̃��N�G�X�g�R�[�h
  tAnsChop aChop;    //!< ����v���O��������̉����f�[�^
}tParamChop;


static int init = 0;
static void* netChop = NULL;
static tParamChop p;

static int chopNet();
static void setParam();
static int checkParam();

/*! \fn int chopInit()
\brief chop���g�p����ہA�K���ŏ��ɌĂяo���B
\retval 0 ����
\retval 0�ȊO �G���[�R�[�h
*/
int chopInit(){

  if(init){
    chopEnd();
  }
  init = 0;
  memset(&p, 0, sizeof(p));
  setParam();
  if(checkParam()){
    uM("chopInit(); checkParam(); error");
    return CHOP_PARAM_ERR;
  }
  if(p.ChopUse==0){
    return CHOP_NOT_USE;
  }
  
  if(p.ChopUse==1){
    netChop = netclInit(p.ChopIp, p.ChopPort);
    if(netChop==NULL){
      uM("chopInit(); netclInit(); error");
      return CHOP_NET_ERR;
    }
  }
  init = 1;
  return 0;
}

/*! \fn int chopEnd()
\brief �I�����ɌĂяo���B
\retval 0 ����
*/
int chopEnd(){
  if(netChop){
    netclEnd(netChop);
    netChop = NULL;
  }
  return 0;
}

/*! \fn int chopRepeat()
\brief chop�̌J��Ԃ�������
\retval 0 ����
\retval 0�ȊO �G���[�R�[�h
*/
int chopRepeat(){
  int ret;
  //char tmp[1024];

  if(p.ChopUse==0)
    return CHOP_NOT_USE;
  if(p.ChopUse==1){
    ret=chopNet();
    if(ret)
      return ret;
    //sprintf(tmp, "chopRepeat(); answer\nState:%d, Motor:%d, Temp1:%lf, Temp2:%lf, Interlock:%d, Error:%d",
    //p.aChop.ansState, p.aChop.ansMotor, p.aChop.ansTemp1, p.aChop.ansTemp2, p.aChop.ansInterlock, p.aChop.ansError);
    //uM1("%s", tmp);
  }

  return 0;
}

/*! \fn int chopGetState(){
\return ����v���O�����̏��
*/
int chopGetState(){
  return p.aChop.ansState;
}

/*! \fn chopGetMoter()
\return ���[�^�|�̏��
*/
int chopGetMoter(){
  return p.aChop.ansMotor;
}

/*! \fn double chopGetTemp1()
\return ���x�vCh1
*/
double chopGetTemp1(){
  return p.aChop.ansTemp1;
}

/*! \fn double chopGetTemp2()
\return ���x�vCh2(Chopper)
*/
double chopGetTemp2(){
  return p.aChop.ansTemp2;
}

/*! \fn int chopGetInterlock()
\return �C���^�[���b�N�̏��
*/
int chopGetInterlock(){
  return p.aChop.ansInterlock;
}

/*! \fn int chopGetError()
\return ����v���O�����G���[���
*/
int chopGetError(){
  return p.aChop.ansError;
}

/*! \fn int chopSetReq(int reqSet)
\param[in] reqSet ����v���O�����֓n������R�[�h
\retval 0 ����
\retval 0�ȊO �G���[�R�[�h
*/
int chopSetReq(int reqSet){
  if(reqSet < 0 || reqSet >= 6){
    uM1("chopSetReq(); invalid reqSet=%d",reqSet);
    return CHOP_SET_ERR;
  }
  //uM1("debug chopSetReq(); reqSet = %d", reqSet);
  p.reqSet = reqSet;
  return 0;
}

/*! \fn int chopNet()
\brief ����v���O�����Ƃ̃l�b�g���[�N�ʐM����
\retval 0 ����
\retval 0�ȊO �G���[�R�[�h
*/
int chopNet(){
  tReqChop rChop;
  tAnsChop aChop;
  int ret;

  rChop.reqSet = p.reqSet;
  netuCnvInt((unsigned char*)&rChop.reqSet);
  
  ret = netclWrite(netChop, (const unsigned char*)&rChop, sizeof(rChop));
  if(ret < 0){
    uM1("chopNet(); Chopper network error = %d", ret);
    return CHOP_NET_ERR;
  }
  else if(ret != sizeof(rChop)){
    uM1("chopNet(); Chopper send size error = %d", ret);
    return CHOP_NET_DAT_ERR;
  }
  ret = netclRead(netChop, (unsigned char*)&aChop, sizeof(aChop));
  if(ret < 0){
    uM1("chopNet(); Chopper network error = %d", ret);
    return CHOP_NET_ERR;
  }
  if(ret != sizeof(aChop)){
    uM1("chopNet(); Chopper receive size error = %d", ret);
    return CHOP_NET_DAT_ERR;
  }
  netuCnvInt((unsigned char*)&aChop.ansState);
  netuCnvInt((unsigned char*)&aChop.ansMotor);
  netuCnvLong((unsigned char*)&aChop.ansTemp1);
  netuCnvLong((unsigned char*)&aChop.ansTemp2);
  netuCnvInt((unsigned char*)&aChop.ansInterlock);
  netuCnvInt((unsigned char*)&aChop.ansError);
  p.aChop.ansState = aChop.ansState;
  p.aChop.ansMotor = aChop.ansMotor;
  p.aChop.ansTemp1 = aChop.ansTemp1 + ABS_ZERO;
  p.aChop.ansTemp2 = aChop.ansTemp2 + ABS_ZERO;
  p.aChop.ansInterlock = aChop.ansInterlock;
  p.aChop.ansError = aChop.ansError;
  return 0;
}

/*! \fn void setParam()
\brief �p�����[�^�[�����擾����
*/
void setParam(){
  if(confSetKey("ChopUse"))
    p.ChopUse = atoi(confGetVal());
  if(confSetKey("ChopIp"))
    memcpy(&p.ChopIp, confGetVal(), strlen(confGetVal())+1);
  if(confSetKey("ChopPort"))
    p.ChopPort = atoi(confGetVal());
}

/*! \fn int checkParam()
\brief �p�����[�^�[�̃`�F�b�N
\retval 0 ����
\retval -1 ���s
*/
int checkParam(){
  
  if(p.ChopUse < 0 || p.ChopUse > 2){
    uM1("ChopUse(%d) error!!", p.ChopUse);
    return 1;
  }
  if(p.ChopUse == 1){
    if(!confSetKey("ChopIp")){
      uM1("ChopUse(%d) ChopIp() error!!", p.ChopUse);
      return 1;
    }
    if(p.ChopPort <= 0){
      uM2("ChopUse(%d) ChopPort(%d) error!!", p.ChopUse, p.ChopPort);
      return 1;
    }
  }
  return 0;
}

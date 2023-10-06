/*!
\file errno.cpp
\date 2007.02.09
\author Y.Koide
\brief Defien error code number
*/
#include <string.h>
#include <stdio.h>
#include "errno.h"

//! For errnoTrkZoneStr()
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

//! �G���[�R�[�h�\����(���g�p)
typedef struct sErr{
  int no;
  char* str;
}tErr;

//! �G���[�R�[�h�ɑΉ������G���[������
const tErr errString[] = {
  {NONE_ERR, "Normal"},
  {NOT_USE, "NotUse"},

  //! Common error
  {NET_ERR, "NetErr"},
  {NET_DAT_ERR, "NetDatErr"},
  {NET_TIME_OUT_ERR, "NetTimeOutErr"},
  {COM_ERR, "SerialErr"},
  {COM_DAT_ERR, "SerialDatErr"},
  {GPIB_ERR, "GpibErr"},
  {THRD_ERR, "ThrdErr"},
  {PARAM_ERR, "ParamErr"},

  {TRK00_ERR, "trk00Err"},
  {TRK10_ARG_ERR, "trk10ArgErr"},
  {TRK10_COMAREA_ERR, "trk10ComAreaErr"},
  {TRK20_ARG_ERR, "trk20ArgErr"},
  {TRK20_LOCAL_ERR, "trk20LocalErr"},
  {TRK20_EL_ERR, "trk20El<=0Err"},
  {TRK30_LOCAL_ERR, "trk30LocalErr"},
  {TJCD2M_ERR, "tjcd2mErr"},

  //! weath

  //! trk
  //{TRK_THRD_RAN_ERR, "ThrdRanErr"},
  {TRK_THRD_STOPPED_ERR, "ThrdStopErr"},
  {TRK_SET_ERR, "DatSetErr"},
  {TRK_SAFETY_ERR, "AcuStatErr"},
  {TRK_LIMIT_AZ_ERR, "AzLimErr"},
  {TRK_LIMIT_EL_ERR, "ElLimErr"},
  {TRK_LIMIT_AZ_EL_ERR, "AzElLimErr"},
  {TRK_TRACKING_TIMEOUT_ERR, "TrakingTimeOutErr"},
  {TRK_STANDBY_ERR, "TrackingStandbyErr"},

  //! chop
  {CHOP_SET_ERR, "DatSetErr"},  
  {CHOP_STATE_ERR, "ChopStateErr"},
  {CHOP_MOVING_TIMEOUT_ERR, "ChopMovingTimeoutErr"},
  
  //! fe
  {FE_NOT_INIT_ERR, "FrontendNotInitializeErr"},

  //! if
  {IF_STATE_ERR, "IfStateErr"},

  //! saacq
  {SAACQ_REJECT, "SaacqReject"},
  {SAACQ_STATE_ERR, "SaacqStateErr"},
  {SAACQ_NOT_INIT_ERR, "SaacqNotInitializeErr"},

  //! be
  {BE_REJECT, "BackendReject"},
  {BE_STATE_ERR, "BackendStateErr"},
  {BE_NOT_INIT_ERR, "BackendNotInitializeErr"},

  {0, "***"} //! Tarmineter
};

static char num[256];

/*! \fn const char* errnoStr(int errno)
\brief �G���[�R�[�h����G���[������𓾂�
\param[in] errno �G���[�R�[�h
\return �G���[������
*/
const char* errnoStr(int errno){
  const char* str;
 
  for(int i=0; ; i++){
    if(strcmp(errString[i].str, "***") == 0){
      //!Search end
      sprintf(num, "%d", errno);
      str = num;
      break;
    }
    if(errno == errString[i].no){
      str = errString[i].str;
      break;
    }
  }
  return str;
}

/*! \fn const char* errnoEpoch(int epoch)
\brief ���_�𕶎���œ���
\param[in] epoch ���_���ʃR�[�h 1:B1950 2:J2000
\return ���_������
*/
const char* errnoEpoch(int epoch){
  static char e[6];

  if(epoch == 1){
    sprintf(e, "B1950");
  }
  else if(epoch == 2){
    sprintf(e, "J2000");
  }
  else{
    sprintf(e, "???");
  }
  return e;
}

/*! \fn const char* errnoCoord(int coord)
\brief ���W�n���ʃR�[�h������W�n�̕�����𓾂�
\param[in] coord ���W�n���ʃR�[�h 0:����` 1:RADEC 2:LB 3:AZEL
\return ���W�n�̕�����
*/
const char* errnoCoord(int coord){
  static char c[6];

  if(coord == 0){
    sprintf(c, "0");
  }
  else if(coord == 1){
    sprintf(c, "RADEC");
  }
  else if(coord == 2){
    sprintf(c, "LB");
  }
  else if(coord == 3){
    sprintf(c, "AZEL");
  }
  else{
    sprintf(c, "?????");
  }
  return c;
}

/*! \fn const char* errnoFrame(int frame)
\brief ���x�̊��`�R�[�h���當����𓾂�
\param[in] frame ���x�̊��`�R�[�h 1:LSR 2:Heliocentric
\return ���x�̊��`�̕�����
*/
const char* errnoFrame(int frame){
  static char f[13];

  if(frame == 1){
    sprintf(f, "LSR");
  }
  else if(frame == 2){
    sprintf(f, "Heliocentric");
  }
  else{
    sprintf(f, "???");
  }
  return f;
}

/*! \fn const char* errnoDefinition(int definition)
\brief �h�b�v���[�V�t�g�̒�`�R�[�h���當����𓾂�
\param[in] definition �h�b�v���[�V�t�g�̒�`�R�[�h 1:Radio 2:Optical
\return �h�b�v���[�V�t�g�̕�����
*/
const char* errnoDefinition(int definition){
  static char d[8];

  if(definition == 1){
    sprintf(d, "Radio");
  }
  else if(definition == 2){
    sprintf(d, "Optical");
  }
  else{
    sprintf(d, "???");
  }
  return d;
}

/*! \fn const char* errnoTrkPZoneStr(int zone)
\brief �]�[���R�[�h���當����𓾂�
\param[in] zone �]�[���R�[�h 0:Auto 1:CW 2:CCW
\return �]�[���̕�����
*/
const char* errnoTrkPZoneStr(int zone){
  static char z[5];

  if(zone == 0){
    sprintf(z, "Auto");
  }
  else if(zone == 1){
    sprintf(z, "CW");
  }
  else if(zone == 2){
    sprintf(z, "CCW");
  }
  else{
    sprintf(z, "???");
  }
  return z;
}

/*! \fn const char* errnoTrkRZoneStr(int zone)
\brief �]�[���R�[�h���當����𓾂�
\param[in] zone �]�[���R�[�h 1:CW 2:CCW
*/
const char* errnoTrkRZoneStr(int zone){
  static char z[4];

  if(zone == 1){
    sprintf(z, "CW");
  }
  else if(zone == 2){
    sprintf(z, "CCW");
  }
  else{
    sprintf(z, "???");
  }
  return z;
}

/*! \fn const char* errnoTrkACUStatusStr(int status, int statusNo)
\brief ACU�X�e�[�^�X�R�[�h����ACU�̃f�[�^�𓾂�
\param[in] status ACU�X�e�[�^�X�R�[�h
\param[in] statusNo ���Ԗڂ̃X�e�[�^�X�R�[�h��
\return �X�e�[�^�X�R�[�h������
*/
const char* errnoTrkACUStatusStr(int status, int statusNo){
  static char statStr[5][1024];
  char* tmp;
  //int err;

  //! Check AZEL Drive
  if(statusNo == 1){
    tmp = statStr[0];
    tmp[0] = '\0';
    if((status & EL_DRIVE_DISABLE) == EL_DRIVE_DISABLE){
      sprintf(tmp+strlen(tmp), "EL_DIS ");
      //err=-1;
    }
    if((status & AZ_DRIVE_DISABLE) == AZ_DRIVE_DISABLE){
      sprintf(tmp+strlen(tmp), "AZ_DIS ");
      //err=-1;
    }
    if((status & EL_DRIVE_ENABLE) == EL_DRIVE_ENABLE){
      //sprintf(tmp+strlen(tmp), "EL_DRIVE_ENABLE ");
    }
    if((status & AZ_DRIVE_ENABLE) == AZ_DRIVE_ENABLE){
      //sprintf(tmp+strlen(tmp), "AZ_DRIVE_ENABLE ");
    }
  }
  //! Check Tracking Mode
  if(statusNo == 2){
    tmp = statStr[1];
    tmp[0] = '\0';
    if((status & TRACK_MODE_FAULT) == TRACK_MODE_FAULT){
      sprintf(tmp+strlen(tmp), "TRACK_MODE_FAULT ");
      //err=-1;
    }
    if((status & STANDBY) == STANDBY){
      sprintf(tmp+strlen(tmp), "STANDBY ");
    }
    if((status & PROGRAM_TRACK_MODE) == PROGRAM_TRACK_MODE){
      sprintf(tmp+strlen(tmp), "PROGRAM_TRACK ");
    }
  }
  //! Check Control Mode
  if(statusNo == 3){
    tmp = statStr[2];
    tmp[0] = '\0';
    if((status & REMOTE_CONTROL_MODE) == REMOTE_CONTROL_MODE){
      sprintf(tmp+strlen(tmp), "REMOTE_MODE ");
    }
    if((status & LOCAL_CONTROL_MODE) == LOCAL_CONTROL_MODE){
      sprintf(tmp+strlen(tmp), "LOCAL_MODE ");
      //err=-1;
    }
    if((status & STOW) == STOW){
      sprintf(tmp+strlen(tmp), "STOW ");
    }
  }
  //! Check Total Alarm
  if(statusNo == 4){
    tmp = statStr[3];
    tmp[0] = '\0';
    if((status & TOTAL_ALARM) == TOTAL_ALARM){
      sprintf(tmp+strlen(tmp),"TOTAL_ALARM ");
      //err=-1;
    }
  }
  //! Check DCPA
  if(statusNo == 5){
    tmp = statStr[4];
    tmp[0] = '\0';
    if((status & ANGLE_FAULT) == ANGLE_FAULT){
      sprintf(tmp+strlen(tmp), "ANGLE_FAULT ");
      //err=-1;
    }
    if((status & AZ_DCPA_FAULT) == AZ_DCPA_FAULT){
      sprintf(tmp+strlen(tmp), "AZ_DCPA_FAULT ");
      //err=-1;
    }
    if((status & EL_DCPA_FAULT) == EL_DCPA_FAULT){
      sprintf(tmp+strlen(tmp), "EL_DCPA_FAULT ");
      //err=-1;
    }
  }
  if(statusNo >= 1 && statusNo <= 5){
    if(strlen(tmp) == 0){
      sprintf(tmp, "Normal");
    }
  }
  return tmp;
}

/*! \fn const char* errnoTrkTrackingStr(int trackstat)
\brief �ǔ���ԃR�[�h���當����𓾂�
\param[in] trackstat �ǔ���ԃR�[�h
\return �ǔ���Ԃ̕�����
*/
const char* errnoTrkTrackingStr(int trackstat){
  static char ts[20];
  
  if(trackstat == 0){
    sprintf(ts, "Tracking");
  }
  else if(trackstat == 1){
    sprintf(ts, "Swing");
    //sprintf(ts, "Counting");
  }
  else if(trackstat == -8){
    sprintf(ts, "ResetState");
  }
  else if(trackstat < 0){
    trackstat *= -1;
    sprintf(ts, "Swing (");
    if((trackstat & 4) == 4){
      sprintf(ts + strlen(ts), "Zone ");
    }
    if((trackstat & 1) == 1){
      sprintf(ts + strlen(ts), "Az ");
    }
    if((trackstat & 2) == 2){
      sprintf(ts + strlen(ts), "El ");
    }
    sprintf(ts + strlen(ts), ")");
  }
  else{
    sprintf(ts, "???");
  }
  return ts;
}

/*! \fn const char* errnoChopChopStr(int chop)
\brief ���x�r�����u�̏�ԃR�[�h���當����𓾂�
\param[in] chop ���x�r�����u�̏�ԃR�[�h
\return ���x�r�����u�̏�ԕ�����
*/
const char* errnoChopChopStr(int chop){
  static char c[17];
  // ��� 0:none, 1:close, 2:open(origin), 3:No Signal
  
  if(chop == 0){
    sprintf(c, "None");
  }
  else if(chop == 1){
    sprintf(c, "Close");
  }
  else if(chop == 2){
    sprintf(c, "Open (or Origin)");
  }
  else if(chop == 3){
    sprintf(c, "No Signal");
  }
  else{
    sprintf(c, "???");
  }
  return c;
}

/*! \fn const char* errnoChopMoterStr(int moter)
\brief ���x�r�����u�̃��[�^�̏�ԃR�[�h���當����𓾂�
\param[in] moter ���x�r�����u�̃��[�^�̏�ԃR�[�h
\return ���x�r�����u�̃��[�^�̏�ԕ�����
*/
const char* errnoChopMoterStr(int moter){
  static char m[15];

  //! ���[�^�̏�� 0:none, 1:close�։�]��, 2:open, 3:origin�։�]��, 4:stop
  if(moter == 0){
    sprintf(m, "None");
  }
  else if(moter == 1){
    sprintf(m, "Move to close");
  }
  else if(moter == 2){
    sprintf(m, "Move to open");
  }
  else if(moter == 3){
    sprintf(m, "Move to origin");
  }
  else if(moter == 4){
    sprintf(m, "Stop");
  }
  else{
    sprintf(m, "???");
  }
  return m;
}

/*! \fn const char* errnoSaacqExe(int exe)
\brief �����v�̏�ԃR�[�h���當����𓾂�
\param[in] exe �����v�̏�ԃR�[�h
\return �����v�̏�ԕ�����
*/
const char* errnoSaacqExe(int exe){
  static char e[10];
  //! 0:None -1:FAILED(�ϕ����s) 1:SUCCESS 2:STANDBY 3:EXECUTION 4:ACCEPT(�ʐM) 5:REJECT(�ʐM)

  if(exe == 0){
    sprintf(e, "None");
  }
  else if(exe == -1){
    sprintf(e, "FAILED");
  }
  else if(exe == 1){
    sprintf(e, "Success");
  }
  else if(exe == 2){
    sprintf(e, "Standby");
  }
  else if(exe == 3){
    sprintf(e, "Execution");
  }
  else if(exe == 4){
    sprintf(e, "ACCEPT");
  }
  else if(exe == 5){
    sprintf(e, "REJECT");
  }
  else{
    sprintf(e, "???");
  }
  return e;
}

/*! \fn const char* errnoSaacqErr(int err)
\brief �����v�̃G���[�R�[�h���當����𓾂�
\param[in] err �����v�̃G���[�R�[�h
\return �����v�̃G���[������
*/
const char* errnoSaacqErr(int err){
  static char e[11];
  //0:InitFailed 1:NORMAL 2:ADCovfl 3:ACCovfl 4:ADACCovfl

  if(err == 0){
    sprintf(e, "None");
  }
  else if(err == 1){
    sprintf(e, "Normal");
  }
  else if(err == 2){
    sprintf(e, "ADCovfl");
  }
  else if(err == 3){
    sprintf(e, "ACCovfl");
  }
  else if(err == 4){
    sprintf(e, "ADACCovcl");
  }
  return e;
}


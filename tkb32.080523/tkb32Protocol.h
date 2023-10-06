/*!
\file tkb32Protocol.h
\date 2007.01.03
\author Y.Koide
\brief �ʐM�p�\����
*/
#ifndef __TKB32PROTOCOL_H__
#define __TKB32PROTOCOL_H__

#define TKB32_REQ_VER 001
#define TKB32_ANS_VER 001

typedef struct sReq{
  long size;            //!< �f�[�^�T�C�Y byte
  int reqVer;           //!< �v���g�R���o�[�W����
  int reqFlg;           //!< ���N�G�X�g�̎�� 0:Status 1:Initialize(SetParam) 2:RemoteControle 3:StartObs 4:StopObs 5:EndObs 6:GetParam
  char data;            //!< �e�f�[�^�̐擪�o�C�g reqFlg=0,3,4�̏ꍇ��0���Z�b�g
}tReq;

typedef struct sReqInit{
  long paramSize;       //!< �p�����[�^�[�̕�����
  char param;           //!< �p�����[�^�[
}tReqInit;

typedef struct sReqRemote{
  unsigned int trkReq;   //!< Bit Flag 0:none 1:AzEl 2:�|�C���e�B���O�I�t�Z�b�g 4:Stop 8:unStow 16:Stow 32:Program Tracking 64:ACU Standby 128:Drive unlock 256:Drive lock
  unsigned int chopReq;  //!<          0:none 1:Close 2:Open 3:Origine
  unsigned int if1Req;   //!< Bit Flag 0:none 1:Set Frequency 2:Set Amplitude 4:Set Step Attenuter
  unsigned int if2Req;   //!< Bit Flag 0:none 1:Set Frequency 2:Set Amplitude
  unsigned int saacqReq; //!< Bit Flag 0:none 1:Set Integ Time 2:Start Integration 4:Stop
  unsigned int sasoftReq;//!<          0:none
  double trkAzEl[2];    //!< trkReq&=1 AzEl [deg]
  double trkPof[2];     //!< trkReq&=2 Poff [deg]
  int trkPofCoord;      //!< trkReq&=2 (���g�p)Poff Coordinate 1:RADEC 2:LB 3:AZEL
  int feCompSw;         //!< (���g�p)�R���v���b�T�[�d���X�C�b�` 0:None 1:On�ɂ��� 2:Off�ɂ���
  double if1Freq;       //!< if1Req&=1 [GHz]
  double if1Amp;        //!< if1Req&=2 [dBm]
  double if1Att;        //!< if1Req&=4 [dB]
  double if2Freq;       //!< if2Req&=1 [GHz]
  double if2Amp;        //!< if2Req&=2 [dBm]
  int saacqIntegTime;   //!< saacqReq&&1�̎��̐ϕ����Ԃ̎w�� [sec]
  int saacqOnOffR;      //!< saacqReq&&2�̎��̊ϑ��Ώ� 1:On�_ 2:Off�_ 3:R-Sky
}tReqRemote;

typedef struct sAns{
  long size;            //!< �f�[�^�T�C�Y byte
  int ansVer;           //!< �v���g�R���o�[�W����
  int ansFlg;           //!< �A���T�[�̎�� 0:state 1:Initialize 2:RemoteControle 3:StopObs 4:StartObs 5:End
  char data;            //!< �e�f�[�^�̐擪�o�C�g ansFlg=1,2,3,4�̎���0:���� other:���s
}tAns;

typedef struct sAnsState{
  unsigned int UT;      //!< time(&UT)�œ�����l
  unsigned int LST;     //!< trk20()�œ�����LST�l
  //! weath
  int weathState;       //!< 0:none other:error code
  int dummy1;
  double weathData[6];  //!< �C�� [K] �C�� [hPa] ���x [hPa] ����(North=0) [deg] ���� [m/s] �ő�u�ԕ���[m/s]
  //! trk
  int trkState;         //!< 0:none other:error code
  int trkScanNo;        //!< ���݂̃X�L�����ʒu�ԍ� 0:R-Sky ~-1:off�_�̔ԍ� 1~:On�_�̔ԍ�
  int trkScanCnt;       //!< ON�_�̊ϑ���
  int trkXYCoord;       //!< trkXY�̍��W�n 0:Planet 1:RADEC 2:LB 3:AZEL
  int trkSofCoord;      //!< �X�L�����I�t�Z�b�g�̍��W�n 1:RADEC 2:LB 3:AZEL
  int trkPofCoord;      //!< �|�C���e�B���O�I�t�Z�b�g�̍��W�n 1:RADEC 2:LB 3:AZEL
  double trkXY[2];      //!< �ݒ�l RADEC, LB, AZEL �̂ǂꂩ�ݒ肵������
  double trkSof[2];     //!< �X�L�����I�t�Z�b�g X Y [sec]
  double trkPof[2];     //!< �|�W�V�����I�t�Z�b�g X Y [sec]
  int trkPZONE;         //!< �A���e�i��Zone�v�Z�l 0:Auto 1:CW 2:CCW
  int trkRZONE;         //!< �A���e�i��Zone���ےl 1:CW 2:CCW
  double trkPAZEL[2];   //!< �A���e�i�����v�Z�l(���]�E���]�E�΍��E�͓��ȂǊ�{�ƂȂ�덷�␳)
  double trkP2AZEL[2];  //!< �A���e�i�����v�Z�l2(�C�ە␳����)
  double trkP3AZEL[2];  //!< �A���e�i�����v�Z�l3(�C�ە␳�A�퍷�␳����)
  double trkRAZEL[2];   //!< �A���e�i�������ےl
  double trkEAZEL[2];   //!< �A���e�i�����덷(= trkRAZEL - trkP3AZEL)
  int trkTracking;      //!< �g���b�L���O���Ă��邩 1:Tracking 0:Swing or Stop
  int trkACUStatus[5];  //!< ACU�̃X�e�[�^�X
  //! chop
  int chopState;        //!< 0:none other:error code
  int chopStat;         //!< chop�̏��
  int chopMoter;        //!< chop�̃��[�^�[�̏��
  int dummy2;
  double chopTemp1;     //!< ���x�vch1
  double chopTemp2;     //!< ���x�vch2 ���x�r���̂̉��x K
  //! fe
  int feState;          //!< 0:none other:error code
  int dummy3;
  double feK;           //!< �t�����g�G���h���x K
  double fePa;          //!< �t�����g�G���h�C�� Pa
  int feCompStat1;      //!< �R���v���b�T�[���x�x��M�� 0:None 1:���� 2:�ُ�
  int feCompStat2;      //!< �R���v���b�T�[�^�]�\�� 0:None 1:���� 2:�ُ�
  int feCompStat3;      //!< �R���v���b�T�[����d�� 0:None 1:���� 2:�ُ�
  //! if1
  int if1State;         //!< 0:none other:error code
  double ifVrad;        //!< Vrad [km/s]
  double if1PFreq;      //!< 1stIF���g�� Hz
  double if1PAmp;
  double if1PAtt;       //!< �A�b�e�l�[�^�[�l dB
  double if1RFreq;
  double if1RAmp;
  double if1RAtt;

  //! if2
  int if2State;         //!< 0:none other:error code
  int dummy6;
  double if2PFreq;      //!< 2ndIF���g�� Hz
  double if2PAmp;
  double if2RFreq;
  double if2RAmp;

  //! be
  int saacqState;       //!< 0:none other:error code
  int saacqExe;         //!< �����v�X�e�[�^�X 0:�ϕ����s 1:�ϕ����� 2:�X�^���o�C 3:���s��
  int saacqErr;         //!< �����v�̃G���[�X�e�[�^�X 1:���� 2:ADCovfl 3:ACCovfl 4:ADACCovfl
  int dummy7;
  double saacqTsys;     //!< Tsys [K]
  double saacqTotPow;   //!< �����v�̃X�y�N�g���̐ϕ��l�̕��� [dBm/GHz]
}tAnsState;

#endif

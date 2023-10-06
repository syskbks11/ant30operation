/*!
\file beProtocol.h
\author Y.Koide
\date 2008.03.07
\brief ��������o�b�N�G���h�ԒʐM�v���g�R��
*/
#ifndef __BEPROTOCOL_H__
#define __BEPROTOCOL_H__

//! �������䂩��o�b�N�G���h�ւ̏��������N�G�X�g�\����
typedef struct beReqInit_s{
  long size;             //!< ���M�f�[�^�T�C�Y
  int paramSize;         //!< param�̃T�C�Y
  char param;            //!< �����p�����[�^�[������.(PARAMETER1\tVALUE1\nPARAMETER2\tVALUE2\n....\n�̌`�œ����Ă���)
} beReqInit_t;

//! �������䂩��o�b�N�G���h�ւ̃��N�G�X�g�\����
typedef struct beReqInteg_s{
  long size;              //!< ���M�f�[�^�T�C�Y
  int exeFlag;            //!< STOP=0, START=1, STATUS=2
  int OnOffRoad;          //!< ON=0, OFF=1, ROAD=2
  //int returnFlag;         //!< NORETSPE=0, RETSPE=1 (�����s�v)
  int scanId;             //!< ���݂̊ϑ��Ώ�ID 0:Load -1:OFF�_A -2:OFF�_B 1:ON�_1�� 2:ON�_2�� ...
  unsigned long nowTime;  //!< time(&now) nowTime=(unsigned long)now [sec] �P�ɃL���X�g
  int crctSec;            //!< �ϕ��w�߂��͂�����������A����ڂ̐��b���ׂ�����ϕ����X�^�[�g�����邩
  double DRA;             //!< �X�L�����I�t�Z�b�g ���Ԍo[rad]
  double DDEC;            //!< �X�L�����I�t�Z�b�g ���Ԉ�[rad]
  double DGL;             //!< �X�L�����I�t�Z�b�g ����o[rad]
  double DGB;             //!< �X�L�����I�t�Z�b�g �����[rad]
  double DAZ;             //!< �X�L�����I�t�Z�b�g ��Az[rad]
  double DEL;             //!< �X�L�����I�t�Z�b�g ��El[rad]
  double RA;              //!< �ϑ��_�̍��W�̐�Βl �Ԍo[rad]
  double DEC;             //!< �ϑ��_�̍��W�̐�Βl �Ԉ�[rad]
  double GL;              //!< �ϑ��_�̍��W�̐�Βl ��o[rad]
  double GB;              //!< �ϑ��_�̍��W�̐�Βl ���[rad]
  double AZ;              //!< �ϑ��_�̍��W�̐�Βl Az[rad]
  double EL;              //!< �ϑ��_�̍��W�̐�Βl El[rad]
  double PAZ;             //!< �v�Z��̃A���e�i���ʊp Az[rad]
  double PEL;             //!< �v�Z��̃A���e�i�p El[rad]
  double RAZ;             //!< ���ۂ̃A���e�i���ʊp Az[rad]
  double REL;             //!< ���ۂ̃A���e�i�p El[rad]
  double PA;              //!< �|�W�V�����A���O��(�s�v,���������Ƀp�����[�^�œn���Ă���)
  //double HPBW;            //!< �r�[���T�C�Y
  //double EFFA;            //!< �J���\��
  //double EFFB;            //!< ��r�[���\��
  //double EFFL;            //!< �A���e�i�\��
  //double EFSS;            //!< FSS�\��
  //double GAIN;            //!< �A���e�i����
  //double FQTRK;           //!< �Î~�g���b�L���O���g�� [Hz]
  double FQIF1;           //!< ��ꒆ�Ԏ��g�� [Hz]
  double VRAD;            //!< Vrad [km/s]
  int IFATT;              //!< ���Ԏ��g����������̉��Z�l [db]
  int dummy2;
  double TEMP;            //!< �C�� --- 10�b���ς������� [degC]
  double PATM;            //!< �C�� --- 10�b���ς������� [hPa]
  double PH2O;            //!< �����C�� --- 10�b���ς������� [hPa]
  double VWIND;           //!< ���� --- 10�b���ς������� [m/s]
  double DWIND;           //!< ���� --- 10�b���ς������́B����=0 NE=45 E=90 SE=135 S=180 SW=225 W=270 NW=315 N=360 [deg]�Ȃǂŕ\�����16����
  double TAU;             //!< ��C�̌��w�I����(���0)
  double TSYS;            //!< �V�X�e���G�����x [K](�s�v)
  double BATM;            //!< ��C�̉��x [K] (�ڍוs��TEMP�Ƃ̈Ⴂ��?)
  double IPINT;           //!< ���x�r���̂̉��x [K]--- �u�Ԓl
} beReqInteg_t;


//! �o�b�N�G���h���璆������ւ̉����\����
typedef struct beAnsStatus_s{
  long size;              //!< ���M�f�[�^�T�C�Y
  int endExeFlag;         //!< -1:FAILED 1:SUCCESS 2:STANDBY 3:EXECUTION 4:ACCEPT 5:REJECT
  int acqErr;             //!< 1:NORMAL 2:ADCovfl 3:ACCovfl 4:ADACCovfl
  float tsys;             //!< Tsys [K]
  float totalPower;       //!< Ptotal [dBm]
  int nbrOfChannel;       //!< number of spectrum-channel (�����s�v)
  double spectrum;        //!< == float spectrum[nbrOfChannel] spectrum data (�����s�v)
} beAnsStatus_t;

#endif //!< __BEPROTOCOL_H__

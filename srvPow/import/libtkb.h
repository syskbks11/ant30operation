/*!
\file libtkb.h
\date 2007.11.12
\author Y.Koide
\brief ��v�ȃ��W���[�����܂Ƃ߂����C�u����
*/
#ifndef __LIBTKB_H__
#define __LIBTKB_H__

#include <stdio.h>
/*! u */
#define uMWrite(str, x1, x2, x3, x4) \
{ \
  fprintf(stdout, "%s ", uGetTime()); \
  fprintf(stdout, str, x1, x2, x3, x4); \
  fprintf(stdout, "\n"); \
  if(uFp()){ \
  fprintf(uFp(), "[%s-%s-%d] ", uGetDate(), __FILE__, __LINE__); \
  fprintf(uFp(), str, x1, x2, x3, x4); \
  fprintf(uFp(), "\n"); \
  } \
}

#define uEWrite(str, x1, x2, x3, x4) \
{ \
  fprintf(stderr, "%s ", uGetTime()); \
  fprintf(stderr, str, x1, x2, x3, x4); \
  fprintf(stderr, "\n"); \
  if(uFp()){ \
  fprintf(uFp(), "[%s-%s-%d] ERROR ", uGetDate(), __FILE__, __LINE__); \
  fprintf(uFp(), str, x1, x2, x3, x4); \
  fprintf(uFp(), "\n"); \
  exit(1); \
  } \
}

/*! \fn uM(str)
\brief ���b�Z�[�W�\���A���O�L�^�p�֐�
\brief �W���o�͂Ɏ����ƃ��b�Z�[�W�������\������Ƌ��Ƀ��O�Ɏ����A�t�@�C�����A�s�ԍ������b�Z�[�W�������\������B
\param[in] str ���b�Z�[�W������
*/
#define uM(str) { uMWrite((str), NULL, NULL, NULL, NULL); }

/*! \fn uM1(str)
\brief ���b�Z�[�W�\���A���O�L�^�p�֐�
\brief printf();�`���ŕϐ���1�̏ꍇ�Ɏg�p�B����uM();���Q��
\param[in] str ������
\param[in] x1 �ϐ�1
*/
#define uM1(str,x1) { uMWrite((str), (x1), NULL, NULL, NULL); }

/*! \fn uM2(str)
\brief ���b�Z�[�W�\���A���O�L�^�p�֐�
\brief printf();�`���ŕϐ���2�̏ꍇ�Ɏg�p�B����uM();���Q��
\param[in] str ������
\param[in] x1 �ϐ�1
\param[in] x2 �ϐ�2
*/
#define uM2(str,x1,x2) { uMWrite((str), (x1), (x2), NULL, NULL); }

/*! \fn uM3(str)
\brief ���b�Z�[�W�\���A���O�L�^�p�֐�
\brief printf();�`���ŕϐ���3�̏ꍇ�Ɏg�p�B����uM();���Q��
\param[in] str ������
\param[in] x1 �ϐ�1
\param[in] x2 �ϐ�2
\param[in] x3 �ϐ�3
*/
#define uM3(str,x1,x2,x3) { uMWrite((str), (x1), (x2), (x3), NULL); }

/*! \fn uM4(str)
\brief ���b�Z�[�W�\���A���O�L�^�p�֐�
\brief printf();�`���ŕϐ���4�̏ꍇ�Ɏg�p�B����uM();���Q��
\param[in] str ������
\param[in] x1 �ϐ�1
\param[in] x2 �ϐ�2
\param[in] x3 �ϐ�3
\param[in] x4 �ϐ�4
*/
#define uM4(str,x1,x2,x3,x4) { uMWrite((str), (x1), (x2), (x3), (x4)); }

#define uE(str) { uEWrite((str), NULL, NULL, NULL, NULL); }
#define uE1(str,x1) { uEWrite((str), (x1), NULL, NULL, NULL); }
#define uE2(str,x1,x2) { uEWrite((str), (x1), (x2), NULL, NULL, NULL); }
#define uE3(str,x1,x2,x3) { uEWrite((str), (x1), (x2), NULL, NULL); }
#define uE4(str,x1,x2,x3,x4) { uEWrite((str), (x1), (x2), (x3), NULL); }

typedef time_t;

#ifdef __cplusplus
template <class T> T uEndian(T* p);
extern "C"{
#endif

  /*! conf */
  /*! \fn const char* confVersion();
  \return conf�̃o�[�W�����������Ԃ� 
  */  
  const char* confVersion();

  /*! \fn int confInit()
  \brief conf�̏��������s���B
  \return 0:Success
  */
  int confInit();

  /*! \fn int confAddFile(const char* fileName)
  \brief �t�@�C������p�����[�^���擾����
  \param[in] fileName �t�@�C�������݂̃p�X
  \reutrn 0:Success 1:Fault
  */
  int confAddFile(const char* fileName);

  /*! \fn int confAddStr(const char* str)
  \brief 1�s�������͕����s�̕����񂩂�p�����[�^���擾����
  \param[in] str �p�����[�^�̋L���ꂽ������
  \reutrn 0:Success
  */
  int confAddStr(const char* str);

  /*! \fn const char* confGetStr(const char* _setKey)
  \brief _setKey�ɑΉ�����p�����[�^�̒l���擾����B
  \param[in] _setKey �L�[�ƂȂ镶����
  \return NULL:Fault other:�l�̓�����������ւ̃|�C���^
  */
  const char* confGetStr(const char* key);
  
  /*! \fn int confSetKey(const char* _setKey)
  \brief confGetVal();�Œl�����o�����߂ɐݒ肷��L�[�̓o�^
  \param[in] _setKey �L�[�ƂȂ镶����
  \return 0:Fault 1:Success
  */  
  int confSetKey(const char* _setKey);
  
  /*! \fn const char* confGetVal()
  \brief confSetKey();�Őݒ肳�ꂽ�L�[�ɑΉ�����p�����[�^�̒l���擾����
  \return NULL:Fault other:�Ή����镶����ւ̃|�C���^
  */
  const char* confGetVal();
  
  /*! \fn const char* confGetAllKeyVal()
  \return �S�p�����[�^�̕�����B1�s�ɃL�[�ƒl����g�������Ă���B
  */
  const char* confGetAllKeyVal();
  
  /*! \fn void confPrint()
  \brief �S�p�����[�^��\���A���O�ɋL�^����B
  */
  void confPrint();


  /*! netcl */
  /*! \fn const char* netclVersion()
  \return netcl�̃o�[�W�����������Ԃ� 
  */
  const char* netclVersion();

  /*! \fn void* netclInit(const char* serverName, const int serverPort)
  \brief ���������s���B
  \param[in] serverName �ڑ���T�[�o�̃z�X�g�l�[���܂���IP
  \param[in] serverPort �ڑ���T�[�o�̃|�[�g
  \return netcl�p�����[�^ NULL:Error
  */
  void* netclInit(const char* serverName, const int serverPort);

  /*! \fn void* netclInitWithTimeOut(const char* serverName, const int serverPort, const double t)
  \brief �^�C���A�E�g���ŏ��������s���B
  \param[in] serverName �ڑ���T�[�o�̃z�X�g�l�[���܂���IP
  \param[in] serverPort �ڑ���T�[�o�̃|�[�g
  \param[in] t �^�C���A�E�g���� sec
  \return NULL netcl�p�����[�^
  */
  void* netclInitWithTimeOut(const char* serverName, const int serverPort, const double t);

  /*! \fn int netclEnd(void* _param)
  \brief �I���������s���B
  \param[in] _param netcl�p�����[�^
  \return 0:����I�� -1:Error
  */
  int netclEnd(void* _param);

  /*! \fn void netclSetTimeOut(void* _param, double t)
  \brief ��M���̃^�C���A�E�g���`����
  \param[in] _param netcl�p�����[�^
  \param[in] t �^�C���A�E�g���� sec
  */
  void netclSetTimeOut(void* _param, const double t);

  /*! \fn int netclWrite(void* _param, const unsigned char* buf, const size_t bufSize)
  \brief �f�[�^���M�B
  \param[in] _param netcl�p�����[�^
  \param[in] buf ���M�f�[�^
  \param[in] bufSize ���M�f�[�^�T�C�Y
  \return >=0:���M�����o�C�g���@<0:Errror
  */
  int netclWrite(void* _param, const unsigned char* buf, const size_t bufSize);

  /*! \fn int netclRead(void* _param, unsigned char* buf, size_t bufSize)
  \brief �f�[�^����M����
  \param[in] _param netcl�p�����[�^
  \param[in] buf ��M�f�[�^
  \param[in] bufSize ��M�f�[�^�i�[�̈�T�C�Y
  \return >=0:��M�����o�C�g�� <0:Error
  */
  int netclRead(void* _param, unsigned char* buf, size_t bufSize);


  /*! netsv */
  /*! \fn const char* netsvVersion()
  \return  netsv�̃o�[�W�����������Ԃ� 
  */
  const char* netsvVersion();

  /*! \fn void* netsvInit(const int serverPort)
  \brief netsv������������ۂɌĂяo���B�P��N���C�A���g�̂ݎ�t�\�B
  \param[in] serverPort �҂���������|�[�g�ԍ�
  \return NULL:���s other:�f�[�^�\���̂̃|�C���^
  */
  void* netsvInit(const int serverPort);

  /*! \fn int netsvEnd(void* _param)
  \brief �I�����ɌĂяo���B
  \param[in] _param netsv �p�����[�^�[
  \return 0:Success
  */
  int netsvEnd(void* _param);

  /*! \fn int netsvWaiting(void* _param)
  \brief Server listening
  \param[in] _param 
  \retval 0 Success
  \retval -1 Not connection
  \retval -2 Select error catche
  \retval -3 Over processMax
  */
  int netsvWaiting(void* _param);

  /*! \fn void* netsvWaitingProcess(void* _param)
  \brief netsv�p�����[�^���q�v���Z�X�p�ɃR�s�[����B�ڑ��𕡐��s���ꍇ�Ɏg�p�B
  \param[in] _param netsv�̃p�����[�^�[
  \return NULL:error other:netsv pointer
  */
  void* netsvWaitingProcess(void* _param);

  /*! \fn int netsvDisconnect(void* _param)
  \brief �\�P�b�g�����
  \param[in] _param netsv�̃p�����[�^�[
  \return 0:����
  */
  int netsvDisconnect(void* _param);

  /*! \fn void netsvSetTimeOut(void* _param, double t)
  \brief ��M���̃^�C���A�E�g���`����
  \param[in] _param netcl�p�����[�^
  \param[in] t �^�C���A�E�g���ԁB-1�Ȃ�Select�����Ȃ� sec
  */
  void netsvSetTimeOut(void* _param, const double t);

  /*! \fn int netsvWrite(void* _param, const unsigned char* buf, const size_t bufSize)
  \brief �f�[�^���M�B
  \param[in] _param netsv�p�����[�^
  \param[in] buf ���M�f�[�^
  \param[in] bufSize ���M�f�[�^�T�C�Y
  \return >=0 ���M�����o�C�g��
  \return <0 Errror
  */
  int netsvWrite(void* _param, const unsigned char* buf, const size_t bufSize);

  /*! \fn int netsvRead(void* _param, unsigned char* buf, size_t bufSize)
  \brief �f�[�^����M����
  \param[in] _param netsv�p�����[�^
  \param[in] buf ��M�f�[�^
  \param[in] bufSize ��M�f�[�^�i�[�̈�T�C�Y
  \return >=0 ��M�����o�C�g��
  \return <0 Error
  */
  int netsvRead(void* _param, unsigned char* buf, size_t bufSize);


  /*! netu */
  /*! \fn const char* netuVersion()
  \return version
  */
  const char* netuVersion();

  /*! \fn void netuCnvInt(unsigned char* pa)
  \brief LabView�Ƃ̒ʐM���Ƀr�b�N�G���f�B�A���Ƃ̑��ݕϊ����s�Ȃ��B
  \brief 4�o�C�g�̌^�ɑΉ�
  */
  void netuCnvInt(unsigned char* a);

  /*! \fn void netuCnvLong(unsigned char* pa)
  \brief LabView�Ƃ̒ʐM���Ƀr�b�N�G���f�B�A���Ƃ̑��ݕϊ����s�Ȃ��B
  \brief 8�o�C�g�̌^�ɑΉ�
  */
  void netuCnvLong(unsigned char* a);


  /*! rs232c */
  /*! \fn const char* rs232cVersion()
  \reutrn �o�[�W����������
  */
  const char* rs232cVersion();

  /*! \fn int rs232cInit(int comNo, int bps, int byteSize, int parityBit, int stopBit)
  \brief rs232c���C�u�������g�p����ۂɍŏ��ɌĂяo��
  \param[in] comNo com�|�[�g�ԍ��B����1�`
  \param[in] bps �ʐM���x
  \param[in] byteSize 7:7bit 8:8bit
  \param[in] parityBit 0:none 1:oddParity 2:evenParity
  \param[in] stopBit 0,1:1bit 2:2bit 3:1.5bit
  \return 0:Success other:Error
  */
  int rs232cInit(int comNo, int bps, int byteSize, int parityBit, int stopBit);

  /*! \fn int rs232cEnd(int comNo)
  \brief rs232c���C�u�������I��������ۂɌĂ�
  \param[in] comNo com�|�[�g�ԍ��B����1�`
  \return 0:Success other:Error
  */
  int rs232cEnd(int comNo);

  /*! \fn int rs232cWrite(int comNo, unsigned char* buf, unsigned int bufSize)
  \brief RS-232C����f�[�^���o�͂���
  \param[in] comNo com�|�[�g�ԍ��B����1�`
  \param[in] buf �o�̓f�[�^�ւ̃|�C���^
  \param[in] bufSize �o�̓f�[�^�̃o�C�g�T�C�Y
  \return over0:writeSize under0:Error
  */
  int rs232cWrite(int comNo, unsigned char* buf, const unsigned int bufSize);

  /*! \fn int rs232cRead(int comNo, unsigned char* buf, unsigned int bufSize)
  \brief RS-232C����f�[�^��Ǎ���
  \param[in] comNo com�|�[�g�ԍ��B����1�`
  \param[in] buf �Ǎ��݃f�[�^�i�[�̈�
  \param[in] bufSize �Ǎ��݃f�[�^�i�[�̈�̃o�C�g�T�C�Y
  \return[in] over0:ReadSize, under0:Error
  */
  int rs232cRead(int comNo, unsigned char* buf, const unsigned int bufSize);


  /*! tm */
  /*! \fn const char* tmVersion()
  \return �o�[�W�����������Ԃ�
  */
  const char* tmVersion();

  /*! \fn void* tmInit()
  \brief tm���g�p����Ƃ��ɍŏ��ɌĂяo���B
  \return NULL:Error Other:�f�[�^�\���̂ւ̃|�C���^
  */
  void* tmInit();

  /*! int tmEnd(void* _p)
  \brief tm�̎g�p���I������Ƃ��Ƀ������Ȃǂ��J������B
  \return 0:Success
  */
  int tmEnd(void* _p);

  /*! \fn int tmReset(void* _p)
  \brief �������Z�b�g����
  \return 0:Success
  */
  int tmReset(void* _p);


  /*! \fn double tmGetLag(void* _p)
  \brief tmGetInit(), tmGetReset()����Ă���̎��Ԃ�[sec]�ŕԂ�
  \param[in] _p tm�̃p�����[�^�[
  \return ���� [sec]
  */
  double tmGetLag(void* _p);

  /*! \fn const char* tmGetTimeStr(void* _p, double offset)
  \brief ���Ԃ��擾����
  \param[in] _p tm�p�����[�^�[
  \param[in] offset �I�t�Z�b�g�b�B���ݎ������牽�b��̎����𕶎���Ƃ��ďo�͂��邩�B
  \return YYYYMMDDhhmmss.ssssss(22����)�`���ŕ������Ԃ�
  */
  const char* tmGetTimeStr(void* _p, double offset);

  /*! \fn double tmGetDiff(const time_t t, const double msec)
  \brief �w�肳�ꂽ�����̌��ݎ����Ƃ̍���Ԃ��B
  \brief tmInit()�ɂ�鏉�����̕K�v�Ȃ��B
  \param[in] t time(t)�ɂ���ē�����b�Ɠ�����`
  \param[in] msec msec
  \return �����̍�(=[���͎���]-[���ݎ���])
  */
  double tmGetDiff(const time_t t, const double msec);

  /*! int tmSleepMSec(double msec)
  \brief �w�肵���}�C�N���b�ԑ҂�
  \brief tmInit()�ɂ�鏉�����̕K�v�Ȃ��B
  \param[in] msec �ҋ@���鎞�ԁB�}�C�N���b
  \return 0:����, -1:���s
  */
  int tmSleepMSec(double msec);


  /*! u */
  /*! \fn const char* uVersion()
  \return version
  */
  const char* uVersion();

  /*! \fn int uInit(const char* projectName)
  \brief u�Ŏg�p���郍�O�t�@�C���l�[����ݒ肷��
  \param[in] profectName ���O�t�@�C���̃p�X�Ɠ�����������
  \return 0 ����I��
  \return 1 �G���[
  */
  int uInit(const char* projectName);

  /*! \fn int uEnd()
  \brief u�̎g�p�I�����ɌĂԁB
  \return 0 ����I��
  \return 1 �G���[
  */
  int uEnd();

  /*! \fn const char uGetLogName()
  \reutrn LogFileName
  */
  const char* uGetLogName();

  /*! \fn void uLock()
  \brief ���b�N����B�Z�}�t�H���擾����B
  */
  void uLock();

  /*! \fn void uUnLock()
  \brief ���b�N����������B�Z�}�t�H��j������B
  */
  void uUnLock();

  /*! \fn FILE* uFp()
  \brief �t�@�C���|�C���^�[��Ԃ��B
  */
  FILE* uFp();

  /*! \fn const char* uGetDate()
  \brief �Ăяo�������̓����𕶎���Ƃ��ĕԂ��B
  \return �����̕�����B
  */
  const char* uGetDate();

  /*! \fn const char* uGetTime()
  \brief �Ăяo�������̎����𕶎���Ƃ��ĕԂ��B
  \return �����̕�����B
  */
  const char* uGetTime();

  /*! \fn int uSigFunc(void(*_sigFunc)(void))
  \brief �V�O�i���������ɃR�[���o�b�N�֐�_sigFunc���Ăяo���悤�ݒ肷��
  \brief _sigFunc��void _sigFunc(void)�̊֐��łȂ���΂Ȃ�Ȃ�
  \return 0:���� 1:���s
  */
  int uSigFunc(void(*_sigFunc)(void));

#ifdef __cplusplus
}
#endif

#endif /*!< __LIBTKB_H__ */

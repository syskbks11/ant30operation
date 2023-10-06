/*!
\file u.h
\author Y.Koide
\date 2006.11.27
\brief Utilities.
*/
#ifndef __U_H__
#define __U_H__

/*! \fn #define uMWrite(str, x1, x2, x3, x4)
\brief �W���o�͂ƃ��O�t�@�C���Ƀ��b�Z�[�W���o�͂���B
\param[in] thisFile __FILE__�œn�����Ăяo�����̃t�@�C�����B
\param[in] fileLine __LINE__�œn�����Ăяo�����̍s�ԍ��B
\param[in] str �\��������̏����Bfprintf�ɏ������������B
\param[in] x1 ����str�̒��Ɋ܂܂��ϐ�1
\param[in] x2 ����str�̒��Ɋ܂܂��ϐ�1
\param[in] x3 ����str�̒��Ɋ܂܂��ϐ�1
\param[in] x4 ����str�̒��Ɋ܂܂��ϐ�1
*/
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

/*! \fn #define uEWrite(str, x1, x2, x3, x4)
\brief �G���[�o�͂ƃ��O�t�@�C���ɃG���[���b�Z�[�W���o�͂��ُ�I������B
\param[in] thisFile __FILE__�œn�����Ăяo�����̃t�@�C�����B
\param[in] fileLine __LINE__�œn�����Ăяo�����̍s�ԍ��B
\param[in] str �\��������̏����Bfprintf�ɏ������������B
\param[in] x1 ����str�̒��Ɋ܂܂��ϐ�1
\param[in] x2 ����str�̒��Ɋ܂܂��ϐ�1
\param[in] x3 ����str�̒��Ɋ܂܂��ϐ�1
\param[in] x4 ����str�̒��Ɋ܂܂��ϐ�1
*/
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

/*! \fn #define uM*(str)
\brief ���b�Z�[�W�o�͗p
\brief �W���o�͂ƃ��O�t�@�C���ɏo�͂���B
*/
#define uM(str) { uMWrite((str), NULL, NULL, NULL, NULL); }
#define uM1(str,x1) { uMWrite((str), (x1), NULL, NULL, NULL); }
#define uM2(str,x1,x2) { uMWrite((str), (x1), (x2), NULL, NULL); }
#define uM3(str,x1,x2,x3) { uMWrite((str), (x1), (x2), (x3), NULL); }
#define uM4(str,x1,x2,x3,x4) { uMWrite((str), (x1), (x2), (x3), (x4)); }

/*! \fn #define uE*(str)
\brief �G���[�o�͗p
\brief �G���[�o�͂ƃ��O�t�@�C���ɏo�͂���B
*/
#define uE(str) { uEWrite((str), NULL, NULL, NULL, NULL); }
#define uE1(str,x1) { uEWrite((str), (x1), NULL, NULL, NULL); }
#define uE2(str,x1,x2) { uEWrite((str), (x1), (x2), NULL, NULL, NULL); }
#define uE3(str,x1,x2,x3) { uEWrite((str), (x1), (x2), NULL, NULL); }
#define uE4(str,x1,x2,x3,x4) { uEWrite((str), (x1), (x2), (x3), NULL); }


#ifdef __cplusplus
//! C++�̂ݑΉ��̐錾
template <class T> T uEndian(T* p);

extern "C"{

#endif

  //! C,C++�ɑΉ��̐錾
  const char* uVersion();
  int uInit(const char* projectName);
  int uEnd();
  const char* uGetLogName();
  void uLock();
  void uUnLock();
  FILE* uFp();
  const char* uGetDate();
  const char* uGetTime();
  short uEndianShort(short* pa);
  int uEndianInt(int* pa);
  long long uEndianLongLong(long long* pa);
  int uSigFunc(void* _sigFunc);

#ifdef __cplusplus
}
#endif

#endif

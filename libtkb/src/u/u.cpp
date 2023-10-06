/*!
\file u.cpp
\author Y.Koide
\date 2006.11.27
\brief Utilities.
*/
#ifdef WIN32
#else
#include <signal.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../tm/tm.h"
#include "u.h"

static const char version[] = "0.0.0";

static FILE* fp=NULL;
static int sema=0;
static char fname[256]={'\0'};

#if 0
int main(int argc, char* argv[])
{
  uInit("test");
  uM("test0");
  uM1("test1", 1);
  uM2("test2 %d", 1, 2);
  uM3("test3 %d %d", 1, 2, 3);
  uM4("test4 %d %d %d", 1, 2, 3, 4);
  int a = 1;
  uEndianInt(&a);
  uE1("a = %d", a);
  uEnd();
}
#endif

/*! \fn const char* uVersion()
\return version
*/
const char* uVersion()
{
  return version;
}

/*! \fn int uInit(const char* projectName)
\brief u�Ŏg�p���郍�O�t�@�C���l�[����ݒ肷��
\param[in] profectName ���O�t�@�C���̃p�X�Ɠ�����������
\return 0 ����I��
\return 1 �G���[
*/
int uInit(const char* projectName)
{
  //char fname[128];
  time_t t;
  struct tm* tmt;

  time(&t);
  tmt = localtime(&t);
  sprintf(fname, "%s-%04d%02d%02d%02d%02d%02d.log",
      projectName, tmt->tm_year+1900, tmt->tm_mon+1, tmt->tm_mday, tmt->tm_hour, tmt->tm_min, tmt->tm_sec);
  if(fp != NULL){
    //uM("uInit(); Log file was already opened.\n");
    //return 0;
    uEnd();
  }
  if(projectName == NULL){
    fp = NULL;
    return 0;
  }
  fp = fopen(fname,"a");
  if(fp == NULL){
    uE("uInit(); FileOpenError!!\n");
    return -1;
  }
  uM1("Use logfile(%s)",fname);
  return 0;
}

/*! \fn int uEnd()
\brief u�̎g�p�I�����ɌĂԁB
\return 0 ����I��
\return 1 �G���[
*/
int uEnd()
{
  if(fp==NULL){
    uM("uEnd(); u not initialized\n");    
    return 1; 
  }
  fclose(fp);
  fp=NULL;
  return 0;
}
/*! \fn const char uGetLogName()
\reutrn LogFileName
*/
const char* uGetLogName(){
  return (const char*)fname;
}

/*! \fn uLock()
\brief ���b�N����B�Z�}�t�H���擾����B
*/
void uLock()
{
  while(sema){
  }
  sema=1;
}

/*! \fn uUnLock()
\breif ���b�N����������B�Z�}�t�H��j������B
*/
void uUnLock()
{
  sema=0;
}

/*! \fn uFp()
\brief �t�@�C���|�C���^�[��Ԃ��B
*/
FILE* uFp()
{
  return fp;
}

/*! \fn const char* uGetDate()
\brief �Ăяo�������̓����𕶎���Ƃ��ĕԂ��B
\return �����̕�����B
*/
const char* uGetDate()
{
  static char tmp[128];
  time_t t;
  double ms;
  struct tm* tmt;
  
  //time(&t);
  tmGetTime(0, &t, &ms);
  tmt=localtime(&t);
  sprintf(tmp, "%04d/%02d/%02d-%02d:%02d:%02d.%03.0lf",
	  tmt->tm_year+1900, tmt->tm_mon+1, tmt->tm_mday, tmt->tm_hour, tmt->tm_min, tmt->tm_sec, ms);
  return tmp;
}

/*! \fn const char* uGetTime()
\brief �Ăяo�������̎����𕶎���Ƃ��ĕԂ��B
\return �����̕�����B
*/
const char* uGetTime()
{
  static char tmp[128];
  time_t t;
  double ms;
  struct tm* tmt;
  
  //time(&t);
  tmGetTime(0, &t, &ms);
  tmt=localtime(&t);
/*
  sprintf(tmp, "%02d:%02d:%02d.%03.0lf",
	  tmt->tm_hour, tmt->tm_min, tmt->tm_sec, ms);
*/
  sprintf(tmp, "%02d:%02d:%02d.%1.0lf",
	  tmt->tm_hour, tmt->tm_min, tmt->tm_sec, ms);
  return tmp;
}

/*!
\brief LabView�Ƃ̒ʐM���Ƀr�b�N�G���f�B�A���Ƃ̑��ݕϊ����s�Ȃ��B
\brief �e��Bit�ɑΉ�
*/
template <class T> T uEndian(T* p)
{
  int pSize = sizeof(T);
  unsigned char* ps = (unsigned char*)p;
  unsigned char* pe = (unsigned char*)p + pSize - 1;
  int pSizeHalf = pSize / 2;
  for(int i = 0; i < pSizeHalf; i++)
  {
    unsigned char a;
    a = *ps;
    *ps = *pe;
    *pe = a;
    *ps++;
    *pe--;
  }
  return *p;
}

/*!
\brief LabView�Ƃ̒ʐM���Ƀr�b�N�G���f�B�A���Ƃ̑��ݕϊ����s�Ȃ��B
*/
short uEndianShort(short* pa)
{
  return uEndian(pa);
}

int uEndianInt(int* pa)
{
  return uEndian(pa);
}

long long uEndianLongLong(long long* pa)
{
  return uEndian(pa);
}

/*!
\brief �V�O�i����M�֘A
*/
#ifdef WIN32
#include <windows.h>
#include <wincon.h>
#else
#include <signal.h>
#endif

typedef void(*uSigFunc_t)(void);
void* uSigFuncP;

#ifdef WIN32
BOOL WINAPI uSigCallFunc(DWORD sig){
  switch(sig){
  case CTRL_C_EVENT:
  case CTRL_BREAK_EVENT:
  case CTRL_CLOSE_EVENT:
  case CTRL_LOGOFF_EVENT:
  case CTRL_SHUTDOWN_EVENT:
    //((sigFunc_t)(sigFuncP))();
    exit(0);
    break;
  default:
    return TRUE;
  }
  return FALSE; //!< �v���Z�X���I������̂�FALSE��Ԃ�
}
#else
void uSigCallFunc(int sig){
  switch(sig){
    //! �I������
  case SIGINT:
  case SIGFPE:
  case SIGTERM:
    //case SIGKILL:
    //((sigFunc_t)(sigFuncP))();
    exit(0);
    break;

    //! �I�����Ȃ�����
  case SIGSEGV:
    return;

    //! �I�����Ȃ�����
  case SIGALRM:
  case SIGCHLD:
  case SIGTSTP:
  case SIGCONT:
  default:
    return;
  }
  return;
}
#endif

int uSigFunc(void* _sigFunc){
  int ret =0;
  uSigFuncP = (void*)_sigFunc;
#ifdef WIN32
  ::SetConsoleCtrlHandler(uSigCallFunc, TRUE);
#else
  if(signal(SIGINT, uSigCallFunc) == SIG_ERR){
    ret = 1;
  }
  if(signal(SIGTERM, uSigCallFunc) == SIG_ERR){
    ret = 1;
  }
#endif
  return ret;
}

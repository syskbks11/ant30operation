/*!
\file tm.cpp
\author Y.Koide
\date 2006.12.17
\brief �^�C�}�[���W���[��
*
* Changed by NAGAI Makoto
* for 32-m telescope & 30-cm telescope
*/
#ifdef WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <sys/time.h>
#endif

#include <time.h>
#include <memory>
#include <memory.h>
#include <stdio.h>
#include "tm.h"

static const char version[] = "0.0.0";

typedef struct sTm{
  time_t t;
  double msec;
  char date[32];
}tTm;

/*! \fn const char* tmVersion()
\return �o�[�W�����������Ԃ�
*/
const char* tmVersion(){
  return version;
}

/*! \fn void* tmInit()
\brief tm���g�p����Ƃ��ɍŏ��ɌĂяo���B
\return NULL:Error Other:�f�[�^�\���̂ւ̃|�C���^
*/
tTm* tmInit(){
//void* tmInit(){
  tTm* p;

  p=(tTm*)malloc(sizeof(tTm));
  if(!p){
    return NULL;
  }

  memset(p, 0, sizeof(*p));
  tmReset(p);
//  tmReset((void*)p);

  return p;
//  return (void*)p;
}

/*! int tmEnd(void* _p)
\brief tm�̎g�p���I������Ƃ��Ƀ������Ȃǂ��J������B
\return 0:Success
*/
int tmEnd(tTm* p){
//int tmEnd(void* _p){
//  tTm* p = (tTm*)_p;
  if(p){
    free(p);
    p=NULL;
  }
  return 0;
}

/*! \fn int tmReset(void* _p)
\brief �������Z�b�g����
\return 0:Success
*/
int tmReset(tTm* p){
//int tmReset(void* _p){
//  tTm* p;
//  p=(tTm*)_p;

  tmGetTime(0, &p->t, &p->msec);
  return 0;
}

/*! \fn double tmGetLag(void* _p)
\brief tmGetInit(), tmGetReset()����Ă���̎��Ԃ�[sec]�ŕԂ�
\param[in] _p tm�̃p�����[�^�[
\return ���� [sec]
*/
double tmGetLag(const tTm* p){
//double tmGetLag(void* _p){
//  tTm* p = (tTm*)_p;
  time_t t;
  double msec;
  double lag;

  tmGetTime(0, &t, &msec);
  lag  = t - p->t;
  lag += (msec - p->msec) / 1000.0;

#ifdef WIN32
#else
  lag -= 0.02; //!< �擾����̂ɂ����鎞�ԁB�o���l sec
  if(lag < 0){
    lag = 0;
  }
#endif

  return lag;
}

/*! \fn const char* tmGetTimeStr(void* _p, double offset)
\brief ���Ԃ��擾����
\param[in] _p tm�p�����[�^�[
\param[in] offset �I�t�Z�b�g�b�B���ݎ������牽�b��̎����𕶎���Ƃ��ďo�͂��邩�B
\return YYYYMMDDhhmmss.ssssss(22����)�`���ŕ������Ԃ�
*/
const char* tmGetTimeStr(tTm* p, double offset){
//const char* tmGetTimeStr(void* _p, double offset){
//  tTm* p = (tTm*)_p;
  struct tm* tmt;

  //! offset���Ԃ�ǉ������l��ݒ�
  time_t t = p->t + (time_t)offset;
  double ms = p->msec + (offset - (double)((int)offset)) * 1000.0;
//  double us;

  if(ms >= 1000.0){
    t++;
    ms -= 1000.0;
  }
  else if(ms < 0){
    t--;
    ms += 1000.0;
  }
  const double us = ms * 1000.0;

  tmt = localtime(&t);
  sprintf(p->date, "%04d%02d%02d%02d%02d%02d.%06.0lf",
	  tmt->tm_year+1900, tmt->tm_mon+1, tmt->tm_mday,
	  tmt->tm_hour, tmt->tm_min, tmt->tm_sec, us);

  return (const char*)p->date;
}

/*! \fn double tmGetDiff(const time_t t, const double msec)
\brief �w�肳�ꂽ�����̌��ݎ����Ƃ̍���Ԃ��B
\brief tmInit()�ɂ�鏉�����̕K�v�Ȃ��B
\param[in] t time(t)�ɂ���ē�����b�Ɠ�����`
\param[in] msec msec
\return �����̍�(=[���͎���]-[���ݎ���])
*/
double tmGetDiff(const time_t t, const double msec){
  double dif;
  time_t sec;
  double ms;

  tmGetTime(0, &sec, &ms);

  if(t > sec){
    dif = t - sec;
  }
  else{
    dif = sec - t;
    dif *= -1.0;
  }
  dif += (msec - ms) / 1000.0;
  return dif;
}


/*! int tmSleepMSec(double msec)
\brief �w�肵���}�C�N���b�ԑ҂�
\brief tmInit()�ɂ�鏉�����̕K�v�Ȃ��B
\param[in] msec �ҋ@���鎞�ԁB�~���b
\return 0:����, -1:���s, -2:����������NaN
*/
int tmSleepMSec(double msec){
	if(msec == 0)
		return 0;
	if(!(msec > 0)){
		return -2;
	}
#ifdef WIN32
	Sleep((DWORD)msec);
	return 0;
#else
	return usleep(msec*1000.0);
#endif

/*
  if(msec > 0){
#ifdef WIN32
    Sleep((DWORD)msec);
    return 0;
#else
    return usleep(msec*1000.0);
#endif
  }
*/
}

/*! \fn void tmGetTime(double offset, time_t* t, int* msec)
\brief �������~���b�P�ʂŎ擾����B
\brief tmInit()�ɂ�鏉�����̕K�v�Ȃ��B
\param[in] offset �擾�����ɑ΂���I�t�Z�b�g�b 1000����1�b�܂Ŏw��\
\param[out] t �����̕b���Z�̒l�Btime(&t)�œ�������̂Ɠ��`�B
\param[out] msec �擾�����̃~���b�̒l�B
*/
void tmGetTime(double offset, time_t* t, double* msec){
  time_t sec;
  double ms;

#ifdef WIN32
  SYSTEMTIME sysTime;
  struct tm tmt0;

  GetLocalTime(&sysTime);  
  tmt0.tm_year  = sysTime.wYear - 1900;
  tmt0.tm_mon   = sysTime.wMonth - 1;
  tmt0.tm_mday  = sysTime.wDay;
  tmt0.tm_hour  = sysTime.wHour;
  tmt0.tm_min   = sysTime.wMinute;
  tmt0.tm_sec   = sysTime.wSecond;
  tmt0.tm_isdst = -1;

  //! mktime()��Local�Ōv�Z���Ă��܂��B
  sec = mktime(&tmt0);
  ms = sysTime.wMilliseconds;

#else
  struct timeval tv;

  gettimeofday(&tv, NULL);
  sec = tv.tv_sec;
  ms = tv.tv_usec / 1000.0;
  offset -= 0.02; //!< gettimeofday()�Ŏ擾����̂ɂ����鎞�ԁB�o���l sec
#endif

  //! offset���Ԃ�ǉ������l��ݒ�
  sec += (time_t)offset;
  ms += (double)(offset - (double)((int)(offset))) * 1000;
  if(ms >= 1000){
    sec++;
    ms -= 1000;
  }
  else if(ms < 0){
    sec--;
    ms += 1000;
  }
	if(ms >= 999.500) {	//*** msec��999.500�ȏ�̎����O��1000�ƕ\�������̂ŁA�����sec��1�J��グ��B
		ms = 0.0;
		sec++;
	}
  *t = sec;
  *msec = ms;

  return;
}

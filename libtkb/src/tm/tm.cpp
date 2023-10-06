/*!
\file tm.cpp
\author Y.Koide
\date 2006.12.17
\brief タイマーモジュール
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
\return バージョン文字列を返す
*/
const char* tmVersion(){
  return version;
}

/*! \fn void* tmInit()
\brief tmを使用するときに最初に呼び出す。
\return NULL:Error Other:データ構造体へのポインタ
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
\brief tmの使用を終了するときにメモリなどを開放する。
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
\brief 時刻をセットする
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
\brief tmGetInit(), tmGetReset()されてからの時間を[sec]で返す
\param[in] _p tmのパラメーター
\return 時間 [sec]
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
  lag -= 0.02; //!< 取得するのにかかる時間。経験値 sec
  if(lag < 0){
    lag = 0;
  }
#endif

  return lag;
}

/*! \fn const char* tmGetTimeStr(void* _p, double offset)
\brief 時間を取得する
\param[in] _p tmパラメーター
\param[in] offset オフセット秒。現在時刻から何秒後の時刻を文字列として出力するか。
\return YYYYMMDDhhmmss.ssssss(22文字)形式で文字列を返す
*/
const char* tmGetTimeStr(tTm* p, double offset){
//const char* tmGetTimeStr(void* _p, double offset){
//  tTm* p = (tTm*)_p;
  struct tm* tmt;

  //! offset時間を追加した値を設定
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
\brief 指定された時刻の現在時刻との差を返す。
\brief tmInit()による初期化の必要なし。
\param[in] t time(t)によって得られる秒と同じ定義
\param[in] msec msec
\return 時刻の差(=[入力時刻]-[現在時刻])
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
\brief 指定したマイクロ秒間待つ
\brief tmInit()による初期化の必要なし。
\param[in] msec 待機する時間。ミリ秒
\return 0:成功, -1:失敗, -2:引数が負かNaN
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
\brief 時刻をミリ秒単位で取得する。
\brief tmInit()による初期化の必要なし。
\param[in] offset 取得時刻に対するオフセット秒 1000分の1秒まで指定可能
\param[out] t 時刻の秒換算の値。time(&t)で得られるものと同義。
\param[out] msec 取得時刻のミリ秒の値。
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

  //! mktime()はLocalで計算してしまう。
  sec = mktime(&tmt0);
  ms = sysTime.wMilliseconds;

#else
  struct timeval tv;

  gettimeofday(&tv, NULL);
  sec = tv.tv_sec;
  ms = tv.tv_usec / 1000.0;
  offset -= 0.02; //!< gettimeofday()で取得するのにかかる時間。経験値 sec
#endif

  //! offset時間を追加した値を設定
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
	if(ms >= 999.500) {	//*** msecが999.500以上の時ログに1000と表示されるので、代わりにsecを1繰り上げる。
		ms = 0.0;
		sec++;
	}
  *t = sec;
  *msec = ms;

  return;
}

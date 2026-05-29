#ifndef INCLUDE_GUARD_UUID_d24bff52_d0db_4cbb_834e_962bdb68785d
#define INCLUDE_GUARD_UUID_d24bff52_d0db_4cbb_834e_962bdb68785d
/*!
  ¥file srv_penweather.cpp
  ¥author S.Honda
  ¥date 2024.09.28
  ¥brief server program of weather monitoring for ant30 sub-mm telecscope
*/

#include <memory.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include <math.h>
#include <stdlib.h>
#include <sys/file.h>
#include <unistd.h>
#include <errno.h>

#include "data_penweather.h"
#include "penguin_weather.h"
#include "../libtkb/src/libtkb.h"

#define DEVFILE "/dev/serial/by-id/usb-FTDI_USB_HS_SERIAL_CONVERTER_FTRTK9ES-if00-port0"
#define LOGFILE "../log/weather/penguin-weather-%s.log" // %s = "YYYYMMDD"
#define LOCKFILE "/tmp/.penguin-weather.lock"
#define HEADER "date  temperature[K]  humidity%  water[hPa]  pressure[hPa]  windDirec  windVel  windVelmax  statusCode"
#define SRVPORT 10102
#define ABS_ZERO 273.15 // [degC] --> [K]

static netServerClass_t* net = NULL;
static tThrdWeath thrd;
static int checkdata[3];

static void _weathSaveLog(tDatWeath* dst, tmClass_t* dtm);
static int _weathGetData(tDatWeath *dst, tmClass_t* dtm);
static double _weathCalWater(const double T, const double moisture);
static double _weathCalWater_lowtemp(const double T, const double moisture);

static void _weathManageReq(tReqWeath* rst);

static void* _weathThrdStatus(void* _p);

/*! ¥fn void_weathSaveLog(tDatWeath* dst)
  ¥brief データ保存関数
  ¥param[in] dst データ格納構造体
*/
void _weathSaveLog(tDatWeath* dst, tmClass_t* dtm){
  //save log file
  FILE* fp;
  time_t t;
  struct tm* tmt;
  char fname[256];
  char datestr[10];

  memset(datestr, 0, 10);
  memset(fname, 0, 256);
  // if(dtm==NULL){
  //   dtm = tmInit();
  // }
  strncpy(datestr, tmGetTimeStr(dtm,0), 8);
  sprintf(fname, LOGFILE, datestr);
  fp = fopen(fname, "r+");
  if(fp == NULL){
    // If file doesn't exist, open in "w" mode to create the file
    fp = fopen(fname, "w");
    if (fp == NULL) {
      uM1("Error opening log file\"%s\"",fname);
      return;
    }
    // Write "#HEADER" in the first line
    fprintf(fp, "%s%s%s", "#", HEADER, "\n");
  } else {
    // File exists, move file pointer to the end
    fseek(fp, 0, SEEK_END);
  }
  // Write additional content to the file
  fprintf(fp, "%s  %.2f  %.2f  %.2f  %.2f  %d  %.2f  %.2f  %d\n",
          tmGetTimeStr(dtm,0),
          dst->temp, dst->humid, dst->water, dst->press,
          dst->direct, dst->windVel, dst->windVelMax,
          dst->status);

  fclose(fp);
}


int _weathGetData(tDatWeath *dst, tmClass_t* dtm){
  double rawdata[3];
  int ret = penguin_weather_init(DEVFILE);
  if(ret){
    uM1("ERROR:: penguin_weather_init() return %d", ret);
    penguin_weather_end();
    return -1;
  }
  dst->status = 0;
  penguin_weather_getData(rawdata);
  tmReset(dtm);

  if(rawdata == NULL){
    uM("_weathGetData(); penguin_weather_getData() failed.");
    dst->status = 1;
    _weathSaveLog(dst, dtm);
  }
  else{
    if(rawdata[0] < -90 || rawdata[0] > 30){ // [degC]
      uM1("bad penguin_weather temperature = %lf.", rawdata[0]);
      dst->status += 2;
    }else{
      dst->temp = rawdata[0] + ABS_ZERO; // [K]
    }
    if(rawdata[1] < 0 || rawdata[1] > 100){ // [%]
      uM1("bad penguin_weather temperature = %lf.", rawdata[1]);
      dst->status += 4;
    }else{
      dst->humid = rawdata[1];
    }
    if(rawdata[2] < 400 || rawdata[2] > 1200){ // [hPa]
      uM1("bad penguin_weather pressure = %lf.", rawdata[2]);
      dst->status += 8;
    }else{
      dst->press = rawdata[2];
    }

    /** dummy for 30-cm */
    // dst->direct =0; //!< 360:N 90:E 180:S 270:W とする方位角(右回り系) 0:カーム(静穏時)
    // dst->windVel = 0;
    // dst->windVelMax = 0;
    dst->water = _weathCalWater(dst->temp, dst->humid); //!< 水蒸気圧 hPa
    dst->time = tmToUnixTime(dtm);

    if((checkdata[0]!=int(rawdata[0]*100)) ||
       (checkdata[1]!=int(rawdata[1]*100)) ||
       (checkdata[2]!=int(rawdata[2]*100))){
      _weathSaveLog(dst, dtm);
      checkdata[0]=int(rawdata[0]*100);
      checkdata[1]=int(rawdata[1]*100);
      checkdata[2]=int(rawdata[2]*100);
    }
  }

  penguin_weather_end();
  return dst->status;
}


/*! ¥fn double _weathCalWater(double T, double moisture)
  ¥brief 湿度と温度から水蒸気圧を算出する
  ¥brief 計算にはワグナーの式を用いている
  ¥brief 換算は273〜313Kまでしかサポートしていないので低温化で使用する場合は注意が必要
  ¥param[in] T 気温 [K]
  ¥param[in] moisture 湿度 [%]
  ¥return 水蒸気圧 [hPa]
*/
double _weathCalWater(const double T, const double moisture){
  const double A = -7.76451;
  const double B = 1.45838;
  const double C = -2.7758;
  const double D = -1.23303;
  const double Tc = 647.3; //!< K
  const double Pc = 22120; //!< kPa(使用範囲275〜647.3 K、273〜313 Kにおいて0.1hPa以下の誤差)
  double tau = 1.0-T/Tc;
  double Pw;
  double tmp;

  tmp = (A * tau + B * pow(tau, 1.5) + C * pow(tau, 3) + D * pow(tau, 6)) / (T / Tc);
  Pw = pow(M_E, tmp) * Pc; //!< [kPa]
  Pw *= 10;                //!< [hPa]
  Pw *= moisture / 100.0;  //!< Pwater [hPa]
  return Pw;
}


/*! ¥fn double _weathCalWater_lowtemp(double T, double moisture)
  ¥brief 温度から飽和水蒸気圧を算出し、飽和水蒸気圧と相対湿度から追尾計算に必要な水蒸気圧を計算
  ¥brief Sonntag (1990)の式（-100℃から対応）
  ¥param[in] T 気温 [K]
  ¥param[in] moisture 湿度 [%]
  ¥return W_vapor 水蒸気圧 [hPa]
*/
double _weathCalWater_lowtemp(const double T, const double moisture){
  const double A = -6096.9385;
  const double B = 21.2409642;
  const double C = 0.02711193;
  const double D = 0.00001673952;
  const double E = 2.433502;
  double S_vapor; // saturated vapor
  double W_vapor; // water vapor

  S_vapor = exp((A/T) + B - C*T + D*pow(T, 2) + E*log(T));
  W_vapor = S_vapor * (moisture / 100); // [hPa]

  return W_vapor;
}


void _weathManageReq(tReqWeath* rst){
  // no actions in srv_penweather
  switch (rst->reqSet) {
  default: // invalid request
    break;
  }
}


/*! ¥fn void* _weathThrdStatus(void* _p)
  ¥brief データ取得スレッド
  ¥param[in] _p パラメータ配列(未使用)
  ¥return NULL(0):成功
  * thread 2, phase *
  */
void* _weathThrdStatus(void* p){
  //void _weathThrdStatus(){
  int ret;
  double interval = 0.; // to do action at the first iteration

  if(thrd.thrdStatusRun){
    uM("_weathThrdStatus(); Weather Service already started");
    // return NULL;
  }

  tmReset(thrd.vtm);
  thrd.thrdStatusRun = 1;

  while(thrd.thrdStatusRun){
    if(tmGetLag(thrd.vtm) < interval){
      tmSleepMSec(interval * 10.0);
      continue;
    }
    interval = thrd.interval; // from second iteration, wait for interval [sec]
    thrd.thrdRet = -1;
    tmReset(thrd.vtm);
    ret = _weathGetData(&thrd.p.datWeath,thrd.dtm);
    if(ret!=0){
      uM1("ERROR:: weathGetData has invalid status (%d)",ret);
    }
    thrd.thrdRet = 0;
  }
  thrd.thrdStatusRun = 0;
  return NULL;
}

int create_lockfile(const char *lockfile){

  // Open or create the lock file
  int fd = open(lockfile, O_RDWR | O_CREAT, 0666);
  if (fd == -1) {
    perror("Error opening lock file");
    exit(EXIT_FAILURE);
  }

  // Try to acquire an exclusive lock
  if (flock(fd, LOCK_EX | LOCK_NB) == -1) {
    if (errno == EWOULDBLOCK) {
      printf("Another instance is already running.\n");
      close(fd);
      exit(EXIT_FAILURE);
    } else {
      perror("Error locking file");
      close(fd);
      exit(EXIT_FAILURE);
    }
  }

  return fd;
}

void release_lockfile(int fd){
  flock(fd, LOCK_UN);
  close(fd);
  printf("Lock released. Exiting program.\n");
}

int main(int argc, char* argv[]){
  char* c;
  int ret;
  int fdlock;

  memset(&thrd, 0, sizeof(thrd));
  fdlock = create_lockfile(LOCKFILE);

  //sprintf("./%s", argv[0]);
  ret = uInit("../log/server/srvWeather");
  if(ret!=0) return -1;
  uM2("This program is Compiled at %s %s", __DATE__, __TIME__);

  //! Initialize timer module
  thrd.vtm = tmInit();
  thrd.interval = 1;
  thrd.dtm = tmInit();

  //! Initialize data and penguin module
  thrd.p.datWeath.temp  = 0;
  thrd.p.datWeath.humid = 0;
  thrd.p.datWeath.press = 0;
  thrd.p.datWeath.water = 0;
  thrd.p.datWeath.direct =0;
  thrd.p.datWeath.windVel = 0;
  thrd.p.datWeath.windVelMax = 0;
  thrd.p.datWeath.status = 9999;
  thrd.p.datWeath.time = 0.0;

  checkdata[0] = 0;
  checkdata[1] = 0;
  checkdata[2] = 0;

  ret = penguin_weather_init(DEVFILE);
  if(ret){
    uM1("main(); penguin_weather_init() return %d", ret);
    uEnd();
    return -1;
  }
  uM1("%s opened as a penguin_weather.\n", DEVFILE);
  penguin_weather_end();

  //! ステータス取得開始
  int status = pthread_create(&thrd.thrdStatusID, NULL, _weathThrdStatus, NULL);
  if(status != 0){
    uM1("main(); pthread_create(weathTrhdStatus); status=%d error", status);
    uEnd();
    return -1;
  }

  net = netsvInit(SRVPORT);
  if(net==NULL){
    uM1("main(); netsvInit(srvPort=%d) returns NULL",SRVPORT);
    uEnd();
    return -1;
  }

  while(1){
    if(netsvWaiting(net) < 0){
      continue;
    }
    while(1){
      memset(&thrd.p.reqWeath, 0, sizeof(tReqWeath));
      ret = netsvRead(net, (unsigned char*)&thrd.p.reqWeath, sizeof(tReqWeath));
      if(ret <= 0){
        uM1("main(); netsvRead(); return %d, failed to manage", ret);
        break;
      }
      else{
        _weathManageReq(&thrd.p.reqWeath);
      }
      ret = netsvWrite(net, (const unsigned char*)&thrd.p.datWeath, sizeof(tDatWeath));
      if(ret <= 0){
        uM1("main(); netsvWrite(); return %d", ret);
        break;
      }
    }
    netsvDisconnect(net);
  }
  uEnd();
  penguin_weather_end();
  netsvEnd(net);

  release_lockfile(fdlock);
  return 0;
}



#endif


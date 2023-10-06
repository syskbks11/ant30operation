/*!
¥file weath.cpp
¥author Y.Koide
¥date 2006.12.12
¥brief 気象データを取得する
* 
* Changed by NAGAI Makoto,
* For Tsukuba 32-m telescope & Antarctic 30-cm telescope. 
*/
#include <stdlib.h>
#include <memory.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <sys/types.h>
#include <pthread.h>

#include "configuration.h"
#include "errno.h"
#include "weath.h"

const double ABS_ZERO = 273.15;

typedef struct sWeathDat{
  int    direct;        //!< 風向 16
  double windVel;       //!< 風速 m/s
  double windVelMax;    //!< 最大瞬間風速 m/s
  double temp;          //!< 気温 K
  double water;         //!< 水蒸気圧 hPa
  double press;         //!< 気圧 hPa
}tWeathDat;

typedef struct sParamWeath{
  int WeathUse;
  int WeathCom;         //!<  気象観測装置に接続するRS-232Cポート番号
  double WeathInterval; //!<  気象取得を行う間隔 sec
  tWeathDat dat;        //!<  気象データ格納用

  tmClass_t* vtm;
  int thrdRet;          //!<  スレッドの戻り値
  int thrdRun;          //!<  スレッドの状態 1:実行 0:停止
  pthread_t thrdID;     //!<  スレッドのID
}tParamWeath;

static int init=0;
static tParamWeath p;
static int comAct=0;    //!< RS-232Cの状態 0:未接続 1:接続

static int _weathInitRS232c();
static int _weathGetDat();
static void* _weathThrdMain(void* _p);
static double _weathCalWater(const double T, const double moisture);
static void _setParam();
static int _checkParam();

/*! ¥fn int weathInit()
¥brief 初期化
¥retval 0 成功
¥retval 0以外 エラーコード
*/
int weathInit(){
  
  if(init){
    weathEnd();
    init = 0;
  }
  memset(&p, 0, sizeof(p));
  _setParam();
  if(_checkParam()){
    uM("weathInit(); _checkParam(); error");
    return WEATH_PARAM_ERR;
  }
  //p.dat.temp = 273;
  //p.dat.water = 5;
  //p.dat.press = 1024;

  init = 1;
  if(p.WeathUse == 0)
    return WEATH_NOT_USE;

  //! 初期化
  comAct = 0;
  p.thrdRun = 0;
  p.vtm = tmInit();

	cssl_start();//081118 in
	if(p.WeathUse != 1){//that is, ==2; use dummy data
		p.dat.temp =  280;
		p.dat.water = 10;
		p.dat.press = 1000;
		p.dat.direct = 10;
		p.dat.windVel = 1;
		p.dat.windVelMax = 5;
		return 0;
	}
	return _weathInitRS232c();
/* 081118 out
  if(p.WeathUse == 1){
	return _weathInitRS232c();
  }
  else{
    p.dat.temp =  280;
    p.dat.water = 10;
    p.dat.press = 1000;
    p.dat.direct = 10;
    p.dat.windVel = 1;
    p.dat.windVelMax = 5;
  }
  return 0;
*/
}

/*! ¥fn int weathEnd()
¥brief 終了時に呼ぶ
¥retval 0 成功
¥retval 0以外 エラーコード
*/
int weathEnd(){
  init = 0;
  if(p.thrdRun){
    p.thrdRun = 0;
    //tmSleepMSec(p.WeathInterval * 1000.0);
    pthread_join(p.thrdID, NULL);
    p.thrdID = NULL;
  }
  if(comAct == 1){
	const int res = glacier_serial_dettach(p.WeathCom);
	if(res){
		uM1("weathEnd(); RS-232C port%d already dettached. ", p.WeathCom);
	}
/* 081118 out
    rs232cEnd(p.WeathCom);
*/
    comAct = 0;
  }
  if(p.vtm){
    tmEnd(p.vtm);
    p.vtm = NULL;
  }
  
  return 0;
}

/*! ¥fn int weathRepeat()
¥brief 気象データの取得処理が正常に行われているか確認する。
¥brief スレッドが実行されているかを確認する。
¥retval 0 正常
¥retval 0以外 エラーコード
*/
int weathRepeat(){
  if(p.WeathUse == 0)
    return WEATH_NOT_USE;
  if(p.thrdRun == 0){
    return WEATH_THRD_ERR;
  }
  return 0;
}

/*! ¥fn int weathStart()
¥brief 気象データ取得を開始する。スレッドを生成する
¥retval 0 成功
¥retval 0以外 エラーコード
*/
int weathStart(){
  //void* ret;
  int state;

  if(p.WeathUse == 0){
    return WEATH_NOT_USE;
  }

  if(p.thrdRun){
    uM("weathStart(); Weath already started");
    //return WEATH_THRD_RAN_ERR;
    return 0;
  }
  state = pthread_create(&p.thrdID, NULL, _weathThrdMain, (void*)NULL);
  if(state != 0){
    uM1("weathStart(); pthread_create(); state=%d error", state);
    return WEATH_THRD_ERR;
  }
  return 0;
}

/*! ¥fn void* _weathThrdMain(void* _p)
¥brief 気象データ取得処理のメイン処理部
¥param[in] _p weathのパラメーターへのポインタ。スレッド生成時に渡される。
¥retval NULL 正常終了
*/
void* _weathThrdMain(void* _p){
  int ret;

  if(p.WeathUse == 0)
    return NULL;
  
  p.thrdRun = 1;
  while(p.thrdRun){
    if(p.WeathUse == 1){
      if(!comAct){
	//! 気象観測装置
	//! bps:1200, start:1bit, data:8bit, parity:odd, stop:2bit
	if(_weathInitRS232c()){
/* 081118 out
	if(rs232cInit(p.WeathCom, 1200, 8, 2, 2)){// parity is odd, isn't it?
*/
		uM1("_weathThrdMain(); RS-232C port %d open error", p.WeathCom);//fail
	}else{
		uM1("_weathThrdMain(); RS-232C port%d open", p.WeathCom);//success
		comAct = 1;
	}
/* 081118 out
	if(rs232cInit(p.WeathCom, 1200, 8, 2, 2) == 0){
	  uM1("_weathThrdMain(); RS-232C port%d open", p.WeathCom);
	  comAct = 1;
	}
*/
      }
      if(comAct){
	ret = _weathGetDat();
	if(ret){
	  printf("_weathThrdMain(); weathGetDat(); return %d", ret);
	}
	if(!comAct){
		const int res = glacier_serial_dettach(p.WeathCom);
		if(res){
			uM1("_weathThrdMain(); RS-232C port%d already dettached. ", p.WeathCom);
		}		
/*
	  rs232cEnd(p.WeathCom);
*/
	}
      }
    }
    tmSleepMSec(p.WeathInterval * 1000.0);
  }
  p.thrdRun = 0;
  p.thrdRet = 0;
  return NULL;
}

//! 観測データの構造体。気象変換機からこのフォーマットでデータが来る。
typedef struct sWeathDatTmp{
  unsigned char sl;       //!< '/'
  unsigned char d[2];     //!< 風向 16方位 00:(カーム)無風 16:N 04:E 08:S 12:W 15:NNW
  unsigned char v[4];     //!< 風速 m/s
  unsigned char vmax[4];  //!< 最大瞬間風速 m/s
  unsigned char t[6];     //!< 気温 Degree/12 4.411.9  8.97 49 985.8
  unsigned char m[3];     //!< 湿度 %
  unsigned char p[6];     //!< 気圧 hPa
  unsigned char cr;       //!< CR=0x0D
  unsigned char lf;       //!< LF=0x0A
}tWeathDatTmp;

int _weathInitRS232c(){
	if(p.WeathCom < 0){
		uM1("_weathInitRS232c(); RS-232C port %d open error; /dev/ttyS* is no longer supported. ", p.WeathCom);
		return WEATH_COM_ERR;
	}
	char devName[20];
	sprintf(devName, RS232C_DEVICE_NAME, p.WeathCom-1);
	cssl_t *serial = cssl_open(devName, glacier_serial_callback_FIXED, p.WeathCom, 1200, 8, 1, 2);
	if(!serial){//081118 in; error management
		uM2("_weathInitRS232c(); RS-232C port %d open error; %s", p.WeathCom, cssl_geterrormsg());
/* 081118 out
	if(rs232cInit(p.WeathCom, 1200, 8, 1, 2)){
		uM1("weathInit(); RS-232C port %d open error", p.WeathCom);//081118 in; error management
*/
		return WEATH_COM_ERR;
	}
	uM1("_weathInitRS232c(); RS-232C port%d open", p.WeathCom);//081118 in; normal flow
	int res = glacier_serial_attach(serial, GLACIER_SERIAL_1ST_NONE);
	if(res){
		uM1("_weathInitRS232c(); RS-232C port%d already attached. ", p.WeathCom);
	}else{
		uM1("_weathInitRS232c(); RS-232C port%d attached successfully. ", p.WeathCom);
	}
	comAct = 1;
	glacier_serial_set_bytes_to_read(sizeof(tWeathDatTmp));
/* 081118 out
    if(rs232cInit(p.WeathCom, 1200, 8, 1, 2) == 0){
      uM1("weathInit(); RS-232C port%d open", p.WeathCom);
      comAct = 1;
    }
    else{
      uM1("weathInit(); RS-232C port %d open error", p.WeathCom);
      return WEATH_COM_ERR;
    }
*/
	return 0;//normal end
}

/*! ¥fn int _weathGetDat()
¥brief 気象変換機から気象データを取得する
¥retval 0 正常終了
¥retval 0以外 エラーコード
*/
int _weathGetDat(){
  const unsigned int bufSize = 128;
	uint8_t* buf;
/* 081118 out
  unsigned char buf[128];
*/
  tWeathDatTmp datTmp;
  int ret;
  int size = 0;
  int cnt = 0;

/* 081118 out
  memset(buf, 0, bufSize);
*/
	if(glacier_serial_wait_for_massage_complete(p.WeathCom)){
		return WEATH_COM_ERR;
	}
	buf = glacier_serial_get_massage(p.WeathCom);
/* 081118 out
  while(size < sizeof(datTmp)){
    //! 気象観測データ読込み
    ret = rs232cRead(p.WeathCom, buf + size, bufSize - size);
    if(ret == 0){
      //! 更新データ無し
      cnt++;
      if(cnt > 2){
	return 0;
      }
    }
    else if(ret < 0){
      comAct=0;
      return WEATH_COM_ERR;
    }
    //else if(ret != sizeof(datTmp)){
    //  return WEATH_COM_DAT_ERR;
    //}
    size += ret;
    tmSleepMSec(100);
    //! debug
    memcpy(&datTmp, buf, sizeof(datTmp));
    //printf("weathGetDat();1 %s", (char*)buf);
  }
*/
  memcpy(&datTmp, buf, sizeof(datTmp));
  //printf("weathGetDat();2 %s", (char*)buf);
  //if(datTmp.sl != '/' || datTmp.cr != 0x0D || datTmp.lf != 0x0A){
  if(datTmp.sl != '/'){
    //! 異常データ
    //tmSleepMSec(10);
/* 081118 out
    ret = rs232cRead(p.WeathCom, buf, bufSize); //!< 残りのデータをバッファから消去
*/
    return WEATH_COM_DAT_ERR;
  }
  char tmp[128];
/* 081118 out
  double moi;
*/
  memset(&p.dat, 0, sizeof(tWeathDat));
  memcpy(tmp, &datTmp.d, 2);
  tmp[2] = '\0';
  p.dat.direct = atoi(tmp);                       //!< 風向 16方位
  memcpy(tmp, &datTmp.v, 4);
  tmp[4] = '\0';
  p.dat.windVel = atof(tmp);                      //!< 風速 [m/s]
  memcpy(tmp, &datTmp.vmax, 4);
  tmp[4] = '\0';
  p.dat.windVelMax = atof(tmp);                   //!< 最大瞬間風速 [m/s]
  memcpy(tmp, &datTmp.t, 6);
  tmp[6] = '\0';
  p.dat.temp = atof(tmp);                         //!< 温度 [℃]
  memcpy(tmp, &datTmp.m, 3);
  tmp[3] = '\0';
  const double moi = atoi(tmp);                                //!< 湿度 [%]
  memcpy(tmp, &datTmp.p, 6);
  tmp[6] = '\0';
  p.dat.press = atof(tmp);                        //!< 気圧 [hPa]

  p.dat.direct *= 360.0 / 16.0;                   //!< 360:N 90:E 180:S 270:W とする方位角(右回り系) 0:カーム(静穏時)
  p.dat.temp += ABS_ZERO;                         //!< 温度 K
  p.dat.water = _weathCalWater(p.dat.temp, moi);   //!< 水蒸気圧 hPa

  //sprintf(tmp, "Tamb=%lf Pamb=%lf Pwater=%lf WindD=%d WindAvg=%lf WindMax=%lf",
  //	  p.dat.temp, p.dat.press, p.dat.water, p.dat.direct, p.dat.windVel, p.dat.windVelMax);
  //uM1("%s", tmp);

  FILE* fp;
  time_t t;
  struct tm* tmt;

  time(&t);
  tmt = localtime(&t);
  sprintf(tmp, "../log/weath-%04d%02d%02d.log", tmt->tm_year+1900, tmt->tm_mon+1, tmt->tm_mday);
  fp = fopen(tmp, "a");
  if(fp){
    int len = sizeof(tWeathDatTmp);
    strncpy(tmp, (const char*)buf, len);
    tmp[len] = '\0';
    fprintf(fp, "\"%04d/%02d/%02d-%02d:%02d:%02d\", %s",
	    tmt->tm_year+1900, tmt->tm_mon+1, tmt->tm_mday, tmt->tm_hour, tmt->tm_min, tmt->tm_sec, tmp);
    fclose(fp);
  }
  return 0;
}

/*! ¥fn int weathGetWeath(double* weath)
¥brief 気象データ気温・気圧・水上気圧を取得する
¥param[out] weath データを格納するための配列。[気温][気圧][水上気圧]の順
¥retval 0 成功
*/
int weathGetWeath(double* weath){
  weath[0] = p.dat.temp;     //!< 気温 [K]
  weath[1] = p.dat.press;    //!< 気圧 [hPa]
  weath[2] = p.dat.water;    //!< 水蒸気圧 [hPa]
  return 0;
}
/*! int weathGetWind(double* wind)
¥brief 気象データの風向・風速・最大瞬間風速を取得する
¥param[out] wind データを格納するための配列。[風向][風速][最大瞬間風速]の順
¥retval 0 成功
*/
int weathGetWind(double* wind){
  wind[0] = p.dat.direct;     //!< 風向 16方位
  wind[1] = p.dat.windVel;    //!< 風速 [m/s]
  wind[2] = p.dat.windVelMax; //!< 最大瞬間風速 [m/s]
  return 0;
}

/*! ¥fn double _weathCalWater(double T, double moisture)
¥brief 湿度と温度から水蒸気圧を算出する。
¥brief 計算にはワグナーの式を用いている。
¥brief ここでの換算は273〜313Kまでしかサポートしていないので低温化で使用する場合は注意が必要。
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

/*! ¥fn void _setParam()
¥brief パラメーター情報を取得する
*/
void _setParam(){
  if(confSetKey("WeathUse"))
    p.WeathUse = atoi(confGetVal());
  if(confSetKey("WeathInterval"))
    p.WeathInterval = atof(confGetVal());
  if(confSetKey("WeathCom"))
    p.WeathCom = atoi(confGetVal());
}

/*! ¥fn int _checkParam()
¥brief パラメーターのチェック
¥retval 0 成功
¥retval -1 失敗
*/
int _checkParam(){
  if(p.WeathUse < 0 || p.WeathUse > 2){
    uM1("WeathUse(%d) error!!", p.WeathUse);
    return -1;
  }
  if(p.WeathUse == 1 || p.WeathUse == 2){
    if(p.WeathInterval <= 0){
      uM2("WeathUse(%d) WeathInterval(%d) error!!", p.WeathUse, p.WeathInterval);
      return -1;
    }
    if(p.WeathUse == 1){
      if(p.WeathCom < 0){
	uM2("WeathUse(%d) WeathCom(%d) error!!", p.WeathUse, p.WeathCom);
	return -1;
      }
    }
  }
  return 0;
}

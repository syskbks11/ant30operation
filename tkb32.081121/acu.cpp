/*!
¥file acu.cpp
¥author NAGAI Makoto
¥date 2008.11.13
¥brief ACU Program for 32-m telescope
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <memory.h>
//#include <math.h>
#include <sys/types.h>

#include "configuration.h"
#include "acu.h"
#include "errno.h"

//#define PI  3.14159265358979323846
//#define PI M_PI

#define STX 0x02
#define ETX 0x03
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

//const double ABS_ZERO = 273.15;
//const int mapNumMax = 20;
//const double sec2rad = PI / (180.0 * 3600.0);
//const double rad2sec = 180.0 * 3600.0 / PI;


//! ACUの状態を格納しておく構造体
typedef struct sAcu{
  int zone;             //!< アンテナの在るゾーン 1:CW 2:CCW
  double dazel[2];      //!< アンテナのAZELの現在値 [deg]
  int s[5];
}tAcu;


typedef struct {
	int TrkCom;           //!< RS-232Cポート
	tAcu acuStat;         //!< ACU現在のステータス
	int acuStatSBefore[5]; //!< 一つ前に取得したACUのステータスコード
	double doazelC[2];    //!< ACUへ出力用のAZEL [deg]
}tParamACU;

static int init=0;
//static tParamTrk *p;
static tParamACU pACU;
static int comAct=0;    //!< RS-232Cの状態. 1:使用可, 0:使用不可
//static double sps;     //!< Second Per Step 1Step処理するのにかかる時間 [sec]

static int _acuInitRS232c();
static void setParam();
static int checkParam();


//! AZEL設定コマンド
typedef struct sAcuAzel{
  unsigned char stx;
  unsigned char c;
  unsigned char z;       //!< アンテナの在るゾーン '1':CW, '2':CCW
  unsigned char x100;    //!< AZ方向の100の位の値ASCII文字 [deg]
  unsigned char x10;
  unsigned char x1;
  unsigned char x0_1;
  unsigned char x0_01;
  unsigned char x0_001;
  unsigned char y10;     //!< EL方向の10の位の値ASCII文字 [deg]
  unsigned char y1;
  unsigned char y0_1;
  unsigned char y0_01;
  unsigned char y0_001;
  unsigned char etx;     //!< ETX(0x03)
}tAcuAzel;

//! 駆動モード制御
typedef struct sAcuMood{
  unsigned char stx;
  unsigned char m;
  unsigned char s1;      //!< 0x30:スタンバイ, 0x32:プログラム追尾, 0x33:STOW(格納)
  unsigned char etx;
}tAcuMood;

//! 駆動禁止制御
typedef struct sAcuBan{
  unsigned char stx;
  unsigned char d;
  unsigned char s1;
  unsigned char etx;
}tAcuBan;


//! AZELステータス
typedef struct sAcuStat{
  unsigned char stx;      //! STX(0x02)
  unsigned char s;        //! ASCIIのS(0x53)
  unsigned char z;        //! アンテナの在るゾーン '1':CW, '2':CCW
  unsigned char x100;     //! AZ方向の100の位の値ASCII文字 [deg]
  unsigned char x10;
  unsigned char x1;
  unsigned char x0_1;
  unsigned char x0_01;
  unsigned char x0_001;
  unsigned char y10;      //! EL方向の10の位の値ASCII文字 [deg]
  unsigned char y1;
  unsigned char y0_1;
  unsigned char y0_01;
  unsigned char y0_001;
  unsigned char s1;
  unsigned char s2;
  unsigned char s3;
  unsigned char s4;
  unsigned char s5;
  unsigned char etx;      //! ETX(0x03)
}tAcuStat;

/*! ¥fn int acuInit()
¥brief ACUからデータを取得する
¥retval 0 成功
¥retval 0以外 エラーコード参照
*/
int acuInit(){
//int acuInit(tParamTrk *param){
//	p.TrkUse = param.TrkUse;
	if(init == 0){
	}
	//p = param;
	comAct = 0;
	init = 1;
	if(confSetKey("TrkCom"))
		pACU.TrkCom = atoi(confGetVal());
	if(pACU.TrkCom == 0){ //!< プラス値はttyUSBを使用することを意味する。マイナス値はttySを使用
		uM1("TrkCom(%d) error!!¥n", pACU.TrkCom);
		//uM2("TrkUse(%d) TrkCom(%d) error!!¥n", p->TrkUse, pACU.TrkCom);
		return -1;
	}
	memset(&pACU.acuStat, 0, sizeof(pACU.acuStat));
	memset(pACU.acuStatSBefore, 0, sizeof(pACU.acuStatSBefore[0]) * 5);
	memset(pACU.doazelC, 0, sizeof(pACU.doazelC[0]) * 2);

	return 0;
}
/*! ¥fn int acuEnd()
¥brief ACUからデータを取得する
¥retval 0 成功
¥retval 0以外 エラーコード参照
*/
int acuEnd(){
	if(comAct){
		const int res = glacier_serial_dettach(pACU.TrkCom);
		if(res){
			uM1("_weathThrdMain(); RS-232C port%d already dettached. ", pACU.TrkCom);
		}
		comAct = 0;	
/*
		rs232cEnd(pACU.TrkCom);
*/
	}
	return 0;
}

/*! ¥fn int acuGetACU()
¥brief ACUからデータを取得する
¥retval 0 成功
¥retval 0以外 エラーコード参照
*/
int acuGetACU(){

	//! ステータス要求コマンド
	static const unsigned char acuGStat[]={STX, 'S', ETX};

  const unsigned int bufSize=1024;
	uint8_t* buf;
/*
  unsigned char buf[1024] = {0};
*/
  tAcuStat stat;
  int ret;
  int i,size;

  //uM("debug trkGetACU()");

  if(!comAct){
	if(_acuInitRS232c()){
/*
	if(rs232cInit(pACU.TrkCom, 4800, 7, 1, 1)){
*/
		return TRK_COM_ERR;
	}
	uM1("trkGetACU(); RS-232C port%d open", pACU.TrkCom);
	comAct = 1;
/* 081118 out
    if(rs232cInit(pACU.TrkCom, 4800, 7, 1, 1) == 0){
      uM1("trkGetACU(); RS-232C port%d open", pACU.TrkCom);
      comAct = 1;
    }
    else{
      return TRK_COM_ERR;
    }
*/
  }
  //! ステータス要求
  ret = glacier_serial_putdata(pACU.TrkCom, (unsigned char*)acuGStat, sizeof(acuGStat));
/*
  ret=rs232cWrite(pACU.TrkCom, (unsigned char*)acuGStat, sizeof(acuGStat));
*/
  if(ret<0){
    comAct=0;
	glacier_serial_dettach(pACU.TrkCom);
/*
    rs232cEnd(pACU.TrkCom);
*/
    return TRK_COM_ERR;
  }
/* 081118 out, since 0 return means success. 
  else if(ret!=sizeof(acuGStat)){
    return TRK_COM_ERR;
  }
*/
  //! ステータス読込み
	glacier_serial_wait_for_massage_complete(pACU.TrkCom);
	buf = glacier_serial_get_massage(pACU.TrkCom);
/*
  size=0;
  for(i=0; i<5; i++){
    //ret=rs232cRead(pACU.TrkCom, buf+size, sizeof(tAcuStat)-size);
    ret=rs232cRead(pACU.TrkCom, buf+size, bufSize-size);
    if(ret<0){
      comAct=0;
      rs232cEnd(pACU.TrkCom);
      return TRK_COM_ERR;
    }
    else if(ret<sizeof(tAcuStat)-size){
      size+=ret;
    }
    else{
      break;
    }
    tmSleepMSec(10);
  }
  if(i==5){
    //buf[size] = '\0';
    //uM1("%s", buf);
    uM1("trkGetACU(); time out. current size=%d", size);
    return TRK_COM_DAT_ERR;
  }
*/
  //! debug
  //printf("trkGetACU(); debug ");
  //for(i=0; i<sizeof(tAcuStat); i++){
  //  printf("%02X ",((unsigned char*)&buf)[i]);
  //}
  //puts("");

  memcpy(&stat, buf, sizeof(tAcuStat));
  if(stat.stx!=STX || stat.s!='S' || stat.etx!=ETX){
    uM("trkGetACU(); invalid status date");
    return TRK_COM_DAT_ERR;
  }

	memcpy(pACU.acuStatSBefore, pACU.acuStat.s, sizeof(pACU.acuStatSBefore[0]) * 5);
//  memcpy(pACU.acuStatSBefore, pACU.acuStat.s, sizeof(pACU.acuStatSBefore[0]) * 5);//081113 out
  //! データ解析
  pACU.acuStat.zone = stat.z - 0x30;
  pACU.acuStat.dazel[0]  =
    100.0*(stat.x100-0x30)
    +10.0*(stat.x10-0x30)+(stat.x1-0x30)+0.1*(double)(stat.x0_1-0x30)
    +0.01*(double)(stat.x0_01-0x30)+0.001*(double)(stat.x0_001-0x30);

  pACU.acuStat.dazel[1]  =
    10.0*(stat.y10-0x30)+(stat.y1-0x30)+0.1*(double)(stat.y0_1-0x30)
    +0.01*(double)(stat.y0_01-0x30)+0.001*(double)(stat.y0_001-0x30);

  //uM2("RAZEL %lf %lf",pACU.acuStat.dazel[0], pACU.acuStat.dazel[1]);
  pACU.acuStat.s[0] = (int)stat.s1;
  pACU.acuStat.s[1] = (int)stat.s2;
  pACU.acuStat.s[2] = (int)stat.s3;
  pACU.acuStat.s[3] = (int)stat.s4;
  pACU.acuStat.s[4] = (int)stat.s5;

  return 0;
}

int _acuInitRS232c(){
	char devName[20];
	sprintf(devName, RS232C_DEVICE_NAME, pACU.TrkCom-1);
	cssl_t *serial = cssl_open(devName, glacier_serial_callback_FIXED, pACU.TrkCom, 4800, 7, 1, 1);
	if(!serial){//081118 in; error management
		uM2("weathInit(); RS-232C port %d open error; %s", pACU.TrkCom, cssl_geterrormsg());
		return TRK_COM_ERR;
	}
	uM1("_acuInitRS232c(); RS-232C port%d open", pACU.TrkCom);//081118 in; normal flow
	int res = glacier_serial_attach(serial, GLACIER_SERIAL_1ST_NONE);
	if(res){
		uM1("_acuInitRS232c(); RS-232C port%d already attached. ", pACU.TrkCom);
	}else{
		uM1("_acuInitRS232c(); RS-232C port%d attached successfully. ", pACU.TrkCom);
	}
	comAct = 1;
	glacier_serial_set_bytes_to_read(sizeof(tAcuStat));
	return 0;//normal end

//	return rs232cInit(pACU.TrkCom, 4800, 7, 1, 1);
}

int acuSetACU(int antZone){
//int acuSetACU(){//081113 out
  const unsigned int bufSize=1024;
  unsigned char buf[1024];
  tAcuAzel azel;
  double az,el;
  int ret;

  memset(buf, 0, bufSize);

  az = pACU.doazelC[0]+0.0005;   //!< 四捨五入
  el = pACU.doazelC[1]+0.0005;
//  az = p->doazelC[0]+0.0005;   //!< 四捨五入
//  el = p->doazelC[1]+0.0005;//081113 out
  
  azel.stx   = STX;
  azel.c     = 'C';
  if(antZone == 1){
//  if(p->antZone == 1){//081113 out
    azel.z   = '1';
  }
  else if(antZone == 2){
//  else if(p->antZone == 2){//081113 out
    azel.z   = '2';
  }
  else{
    azel.z   = '0';           //! 最小駆動方向
  }
  azel.x100  = 0x30+((int)(az/100)%10);
  azel.x10   = 0x30+((int)(az/10)%10);
  azel.x1    = 0x30+((int)(az)%10);
  azel.x0_1  = 0x30+((int)(az/0.1)%10);
  azel.x0_01 = 0x30+((int)(az/0.01)%10);
  azel.x0_001= 0x30+((int)(az/0.001)%10);
  azel.y10   = 0x30+((int)(el/10)%10);
  azel.y1    = 0x30+((int)(el)%10);
  azel.y0_1  = 0x30+((int)(el/0.1)%10);
  azel.y0_01 = 0x30+((int)(el/0.01)%10);
  azel.y0_001= 0x30+((int)(el/0.001)%10);
  azel.etx   = ETX;

  //! debug
  //printf("trkSetACU(); debug ");
  //for(i=1; i<sizeof(azel)-1; i++){
  //  printf("%c",((char*)&azel)[i]);
  //}
  //puts("");
  //! アンテナへの制御コマンド出力
  if(!comAct){
	if(_acuInitRS232c()){
/*
	if(rs232cInit(pACU.TrkCom, 4800, 7, 2, 1)){//which parity is right??
*/
		return TRK_COM_ERR;
	}
	uM1("trkSetACU(); RS-232C port%d open", pACU.TrkCom);
	comAct = 1;
/*
    if(rs232cInit(pACU.TrkCom, 4800, 7, 2, 1) == 0){
      uM1("trkSetACU(); RS-232C port%d open", pACU.TrkCom);
      comAct = 1;
    }
    else{
      return TRK_COM_ERR;
    }
*/
  }

  ret = glacier_serial_putdata(pACU.TrkCom, (unsigned char*)&azel, sizeof(azel));
/*
  ret=rs232cWrite(pACU.TrkCom, (unsigned char*)&azel, sizeof(azel));
*/
  if(ret<0){
    comAct=0;
	glacier_serial_dettach(pACU.TrkCom);
/*
    rs232cEnd(pACU.TrkCom);
*/
    return TRK_COM_ERR;
  }
  return 0;
}

/*! ¥fn int acuSetAcuMood(int mood)
¥brief ACUのモード制御
¥param[in] mood 0:スタンバイ 2:プログラム追尾 3:StowLock
¥return 0:成功 -:失敗
*/
int acuSetAcuMood(int mood){
  tAcuMood acuMood;
  int ret;

  if(mood != 0 && mood != 2 && mood != 3){
    return TRK_SET_ERR;
  }
  acuMood.stx = STX;
  acuMood.m = 'M';
  acuMood.s1 = 0x30 + ((unsigned char)mood);
  acuMood.etx = ETX;

//  if(p->TrkUse == 1){
	ret = glacier_serial_putdata(pACU.TrkCom, (unsigned char*)&acuMood, sizeof(acuMood));
/*
    ret = rs232cWrite(pACU.TrkCom, (unsigned char*)&acuMood, sizeof(acuMood));
*/
    if(ret < 0){
      comAct = 0;
      return TRK_COM_ERR;
    }
//  }
  return 0;
}

/*! ¥fn int acuSetAcuBan(int ban)
¥brief 駆動禁止制御
¥param[in] ban 0:駆動可 1:駆動禁止
¥return 0:成功 -:失敗
*/
int acuSetAcuBan(int ban){
  tAcuBan acuBan;
  int ret;
  
  if(ban!=0 && ban!=1){
    return TRK_SET_ERR;
  }
  acuBan.stx=STX;
  acuBan.d='D';
  acuBan.s1=0x30 + ((unsigned char)ban);
  acuBan.etx=ETX;

//  if(p->TrkUse == 1){
	ret = glacier_serial_putdata(pACU.TrkCom, (unsigned char*)&acuBan, sizeof(acuBan));
/*
    ret=rs232cWrite(pACU.TrkCom, (unsigned char*)&acuBan, sizeof(acuBan));
*/
    if(ret<0){
      comAct=0;
      return TRK_COM_ERR;
    }
//  }
  return 0;
}

/*! ¥fn int acuSetAcuUnStow()
¥brief StowUnlock制御
¥retval 0 成功
¥retval 0以外 エラーコード
*/
int acuSetAcuUnStow(){
//! STOW解除
	static const unsigned char acuStow[]={STX, 'E', ETX};


  int ret;

//  if(p->TrkUse == 1){
	ret = glacier_serial_putdata(pACU.TrkCom, (unsigned char*)acuStow, sizeof(acuStow));
/*
    ret = rs232cWrite(pACU.TrkCom, (unsigned char*)acuStow, sizeof(acuStow));
*/
    if(ret < 0){
      comAct = 0;
      return TRK_COM_ERR;
    }
//  }
  return 0;
}

/*! ¥fn int acuSafetyCheck()
¥brief ACUのステータスでエラーが無いかチェックする
¥return 0:成功, -1:失敗
*/
int acuSafetyCheck(){
  int err;
  char tmp[1024];

  err=0;
  sprintf(tmp, "trkSafetyCheck();¥n");
  //! AZELドライブ確認
  if((pACU.acuStat.s[0] & EL_DRIVE_DISABLE) == EL_DRIVE_DISABLE){
    sprintf(tmp+strlen(tmp), "S1 = EL_DRIVE_DISABLE¥n");
    //err=-1;
  }
  if((pACU.acuStat.s[0] & AZ_DRIVE_DISABLE) == AZ_DRIVE_DISABLE){
    sprintf(tmp+strlen(tmp), "S1 = AZ_DRIVE_DISABLE¥n");
    //err=-1;
  }
  if((pACU.acuStat.s[0] & EL_DRIVE_ENABLE) == EL_DRIVE_ENABLE){
    sprintf(tmp+strlen(tmp), "S1 = EL_DRIVE_ENABLE¥n");
  }
  if((pACU.acuStat.s[0] & AZ_DRIVE_ENABLE) == AZ_DRIVE_ENABLE){
    sprintf(tmp+strlen(tmp), "S1 = AZ_DRIVE_ENABLE¥n");
  }
  //! 追尾モード確認
  if((pACU.acuStat.s[1] & TRACK_MODE_FAULT) == TRACK_MODE_FAULT){
    sprintf(tmp+strlen(tmp), "S2 = TRACK_MODE_FAULT¥n");
    //err=-1;
  }
  if((pACU.acuStat.s[1] & STANDBY) == STANDBY){
    sprintf(tmp+strlen(tmp), "S2 = STANDBY¥n");
  }
  if((pACU.acuStat.s[1] & PROGRAM_TRACK_MODE) == PROGRAM_TRACK_MODE){
    sprintf(tmp+strlen(tmp), "S2 = PROGRAM_TRACK_MODE¥n");
  }
  //! 制御モード確認
  if((pACU.acuStat.s[2] & REMOTE_CONTROL_MODE) == REMOTE_CONTROL_MODE){
    sprintf(tmp+strlen(tmp), "S3 = REMOTE_CONTROL_MODE¥n");
  }
  if((pACU.acuStat.s[2] & LOCAL_CONTROL_MODE) == LOCAL_CONTROL_MODE){
    sprintf(tmp+strlen(tmp), "S3 = LOCAL_CONTROL_MODE¥n");
    //err=-1;
  }
  if((pACU.acuStat.s[2] & STOW) == STOW){
    sprintf(tmp+strlen(tmp), "S3 = STOW¥n");
  }
  //! 統合アラーム確認
  if((pACU.acuStat.s[3] & TOTAL_ALARM) == TOTAL_ALARM){
    sprintf(tmp+strlen(tmp),"S4 = TOTAL_ALARM error¥n");
    //err=-1;
  }
  //! 角度検知器の確認
  if((pACU.acuStat.s[4] & ANGLE_FAULT) == ANGLE_FAULT){
    sprintf(tmp+strlen(tmp), "S5 = ANGLE_FAULT¥n");      
    err=-1;
  }
  if((pACU.acuStat.s[4] & AZ_DCPA_FAULT) == AZ_DCPA_FAULT){
    sprintf(tmp+strlen(tmp), "S5 = AZ_DCPA_FAULT¥n");      
    err=-1;
  }
  if((pACU.acuStat.s[4] & EL_DCPA_FAULT) == EL_DCPA_FAULT){
    sprintf(tmp+strlen(tmp), "S5 = EL_DCPA_FAULT¥n");      
    err=-1;
  }
  if(pACU.acuStat.s[0] != pACU.acuStatSBefore[0]
     || pACU.acuStat.s[1] != pACU.acuStatSBefore[1]
     || pACU.acuStat.s[2] != pACU.acuStatSBefore[2]
     || pACU.acuStat.s[3] != pACU.acuStatSBefore[3]
     || pACU.acuStat.s[4] != pACU.acuStatSBefore[4]){
    uM1("%s",tmp);
    //memcpy(pACU.acuStatSBefore, pACU.acuStat.s, sizeof(pACU.acuStat.s[0]) * 5);
  }
  if(err){
    return TRK_SAFETY_ERR;
  }
  return 0;
}


/*! ¥fn int acuSetDoAzC()
¥brief パラメーター情報を取得する
*/
void acuSetDoAzC(double az){
	pACU.doazelC[0] = az;
}
void acuSetDoElC(double el){
	pACU.doazelC[1] = el;
}

/*! ¥fn void acuGetRZONE(int* RZONE)
¥brief アンテナZoneの実際値
¥param[out] RZONE 0:最小駆動角方向 1:CW 2:CCW
*/
//void acuGetRZONE(int* RZONE){
//  *RZONE = p.acuStat.zone;
//  return;
//}
int acuGetRZONE(){
	return pACU.acuStat.zone;
}

/*! ¥fn void acuGetACUStatus(int* status)
¥brief ACUのステータスを取得する
¥param[out] status int[5]となっておりそれぞれにステータス情報がビットフラグで格納されている。
*/
void acuGetACUStatus(int* status){
	memcpy(status, pACU.acuStat.s, sizeof(int) * 5);
	return;
}

/*! ¥fn void trkGetRAZEL(double* RAZEL)
¥brief アンテナ実際角RAZELの取得
¥param[out] RAZEL [rad]
*/
void acuGetRAZEL(double* RAZEL){
	memcpy(RAZEL, pACU.acuStat.dazel, sizeof(double)*2);
	return;
}


/*! ¥fn void setParam()
¥brief パラメーター情報を取得する
*/
void setParam(){
}

/*! ¥fn int checkParam()
¥brief パラメーターのチェック
¥retval 0 成功
¥retval -1 失敗
*/
int checkParam(){
  return 0;
}

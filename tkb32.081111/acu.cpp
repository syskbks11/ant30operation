/*!
¥file acu.cpp
¥author NAGAI Makoto
¥date 2008.11.13
¥brief ACU Program for 32-m telescope
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>
#include <sys/types.h>

#include "../libtkb/src/libtkb.h"
#include "acu.h"
#include "trkAcu.h"

//#define PI  3.14159265358979323846
#define PI M_PI

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

const double ABS_ZERO = 273.15;
//const int mapNumMax = 20;
const double sec2rad = PI / (180.0 * 3600.0);
const double rad2sec = 180.0 * 3600.0 / PI;


static int init=0;
static tParamTrk p;
static int comAct=0;    //!< RS-232Cの状態. 1:使用可, 0:使用不可
//static double sps;     //!< Second Per Step 1Step処理するのにかかる時間 [sec]

static int acuSetACU();
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


/*! ¥fn int acuGetACU()
¥brief ACUからデータを取得する
¥retval 0 成功
¥retval 0以外 エラーコード参照
*/
int acuGetACU(){

	//! ステータス要求コマンド
	static unsigned char acuGStat[]={STX, 'S', ETX};

  const unsigned int bufSize=1024;
  unsigned char buf[1024] = {0};

  tAcuStat stat;
  int ret;
  int i,size;

  //uM("debug trkGetACU()");

  if(!comAct){
    if(rs232cInit(p.TrkCom, 4800, 7, 1, 1) == 0){
      uM1("trkGetACU(); RS-232C port%d open", p.TrkCom);
      comAct = 1;
    }
    else{
      return TRK_COM_ERR;
    }
  }
  
  //! ステータス要求
  ret=rs232cWrite(p.TrkCom, acuGStat, sizeof(acuGStat));
  if(ret<0){
    comAct=0;
    rs232cEnd(p.TrkCom);
    return TRK_COM_ERR;
  }
  else if(ret!=sizeof(acuGStat)){
    return TRK_COM_ERR;
  }

  //! ステータス読込み
  size=0;
  for(i=0; i<5; i++){
    //ret=rs232cRead(p.TrkCom, buf+size, sizeof(tAcuStat)-size);
    ret=rs232cRead(p.TrkCom, buf+size, bufSize-size);
    if(ret<0){
      comAct=0;
      rs232cEnd(p.TrkCom);
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

  memcpy(p.acuStatSBefore, p.acuStat.s, sizeof(p.acuStatSBefore[0]) * 5);
  //! データ解析
  p.acuStat.zone = stat.z - 0x30;
  p.acuStat.dazel[0]  =
    100.0*(stat.x100-0x30)
    +10.0*(stat.x10-0x30)+(stat.x1-0x30)+0.1*(double)(stat.x0_1-0x30)
    +0.01*(double)(stat.x0_01-0x30)+0.001*(double)(stat.x0_001-0x30);

  p.acuStat.dazel[1]  =
    10.0*(stat.y10-0x30)+(stat.y1-0x30)+0.1*(double)(stat.y0_1-0x30)
    +0.01*(double)(stat.y0_01-0x30)+0.001*(double)(stat.y0_001-0x30);

  //uM2("RAZEL %lf %lf",p.acuStat.dazel[0], p.acuStat.dazel[1]);
  p.acuStat.s[0] = (int)stat.s1;
  p.acuStat.s[1] = (int)stat.s2;
  p.acuStat.s[2] = (int)stat.s3;
  p.acuStat.s[3] = (int)stat.s4;
  p.acuStat.s[4] = (int)stat.s5;

  return 0;
}

int acuSetACU(){
  const unsigned int bufSize=1024;
  unsigned char buf[1024];
  tAcuAzel azel;
  double az,el;
  int ret;

  memset(buf, 0, bufSize);

  az = p.doazelC[0]+0.0005;   //!< 四捨五入
  el = p.doazelC[1]+0.0005;
  
  azel.stx   = STX;
  azel.c     = 'C';
  if(p.antZone == 1){
    azel.z   = '1';
  }
  else if(p.antZone == 2){
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
    if(rs232cInit(p.TrkCom, 4800, 7, 2, 1) == 0){
      uM1("trkSetACU(); RS-232C port%d open", p.TrkCom);
      comAct = 1;
    }
    else{
      return TRK_COM_ERR;
    }
  }

  ret=rs232cWrite(p.TrkCom, (unsigned char*)&azel, sizeof(azel));
  if(ret<0){
    comAct=0;
    rs232cEnd(p.TrkCom);
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

  if(p.TrkUse == 1){
    ret = rs232cWrite(p.TrkCom, (unsigned char*)&acuMood, sizeof(acuMood));
    if(ret < 0){
      comAct = 0;
      return TRK_COM_ERR;
    }
  }
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

  if(p.TrkUse == 1){
    ret=rs232cWrite(p.TrkCom, (unsigned char*)&acuBan, sizeof(acuBan));
    if(ret<0){
      comAct=0;
      return TRK_COM_ERR;
    }
  }
  return 0;
}

/*! ¥fn int acuSetAcuUnStow()
¥brief StowUnlock制御
¥retval 0 成功
¥retval 0以外 エラーコード
*/
int acuSetAcuUnStow(){
//! STOW解除
	static unsigned char acuStow[]={STX, 'E', ETX};


  int ret;

  if(p.TrkUse == 1){
    ret = rs232cWrite(p.TrkCom, (unsigned char*)acuStow, sizeof(acuStow));
    if(ret < 0){
      comAct = 0;
      return TRK_COM_ERR;
    }
  }
  return 0;
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

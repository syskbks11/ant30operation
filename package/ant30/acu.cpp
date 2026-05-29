#ifndef INCLUDE_GUARD_UUID_ca591323_c5de_4d1b_ae88_49cdba38da49
#define INCLUDE_GUARD_UUID_ca591323_c5de_4d1b_ae88_49cdba38da49
/*!
  ¥file acu.cpp
  ¥author NAGAI Makoto
  ¥date 2008.12.8
  ¥brief ACU Program for 30-cm telescope at antarctica
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>//usleep()
#include <time.h> //20240109

#include "configuration.h"
#include "acu.h"
#include "errno.h"

#include "../motor_rpcomfit/penguin_motor.h"

// FILE *fp; //20240109 //20240409 HONDA CO
// static char fname[256]; //20240109 // 20240409 HONDA CO

/**
 * pulse = (x/[deg] - MOTOR_B) * MOTOR_A
 * x/[deg] = pulse/MOTOR_A + MOTOR_B
 * MOTOR_B: encoder origin in degree
 * MOTOR_A:
 */

#define AZ_MOTOR_A (PULSE_360DEG/360.0)
#define EL_MOTOR_A (PULSE_360DEG/360.0)
#define AZ_MOTOR_B 83.0//(190.7)
#define EL_MOTOR_B 67.59//(64.9) 2023:69.0 //68.49(20240125) //20241221 67.59

#define AZ_MAXPULSE 6000000
#define AZ_MINPULSE -11400000
#define EL_MAXPULSE 669100 //280000
#define EL_MINPULSE -2200000 //-2200000

// for optical pointing
//#define EL_MAXPULSE -279308  
//#define EL_MINPULSE -1972158 

#define DUMMY_MOTOR 1

#ifdef DUMMY_MOTOR
int _azcurrentPulse;
int _elcurrentPulse;
#endif


//! ACUの状態を格納しておく構造体
typedef struct sAcu{
  int zone;             //!< アンテナの在るゾーン 1:CW 2:CCW
  double dazel[2];      //!< アンテナのAZELの現在値 [deg]
  int s[5];
}tAcu;


typedef struct {
  penguin_motor_t* azMotor;
  penguin_motor_t* elMotor;
  //	int TrkCom;           //!< RS-232Cポート
  tAcu acuStat;         //!< ACU現在のステータス
  int acuStatSBefore[5]; //!< 一つ前に取得したACUのステータスコード
  double doazelC[2];    //!< ACUへ出力用のAZEL [deg]; Antenna zone 処理前, 0 <= Az < 360 in the Northern Hemisphere, -180 <= Az < 180 in the Southern Hemisphere
  int hemisphere;//0: Northern, 1: Southern
  double azMotorB;//090920 in
}tParamACU;

static int isInitialized=0;
static tParamACU pACU;
static int comAct=0;    //!< RS-232Cの状態. 1:使用可, 0:使用不可

static int _acuInitRS232c();

/*! ¥fn int acuInit()
  ¥brief ACUとの通信を初期化する
  ¥retval 0 成功
  ¥retval 0以外 エラーコード参照
  * thread 1, phase A0
  */
int acuInit(){
  //time_t t; //20240409 HONDA
  //struct tm* tmt;   //20240409 HONDA
  const char* projectName = "AZEL";  

  // time(&t); //20240409 HONDA
  // tmt = localtime(&t); //20240409 HONDA
  // sprintf(fname, "../../tcs01/log/ant30/%s-%04d%02d%02d%02d%02d%02d.log", projectName, tmt->tm_year+1900,tmt->tm_mon+1, tmt->tm_mday, tmt->tm_hour, tmt->tm_min, tmt->tm_sec); //20240409 HONDA
  // fp = fopen(fname,"a"); //20240109 //20240409 HONDA

  if(isInitialized == 0){
  }
  //p = param;
  comAct = 0;
  isInitialized = 1;

  int res = _acuInitRS232c();
  if(res){
    uM("acuInit(), RS232c initialize failed.");
    return res;
  }

  memset(&pACU.acuStat, 0, sizeof(pACU.acuStat));
  memset(pACU.acuStatSBefore, 0, sizeof(pACU.acuStatSBefore[0]) * 5);
  memset(pACU.doazelC, 0, sizeof(pACU.doazelC[0]) * 2);

  /* 091003 out
     pACU.hemisphere = 1;//In Southern Hemisphere
  */
  if(pACU.hemisphere){//In Southern Hemisphere
    uM("acuInit(), SOUTHERN hemisphere mode.");
    pACU.azMotorB = AZ_MOTOR_B -180;
  }else{//In Northern Hemisphere
    uM("acuInit(), NORTHERN hemisphere mode.");
    pACU.azMotorB = AZ_MOTOR_B;
  }

  return 0;
}
/*! ¥fn int acuEnd()
  ¥brief ACUとの通信を終了する
  ¥retval 0 成功
  ¥retval 0以外 エラーコード参照
  * thread 1, phase A2
  */
int acuEnd(){
#ifdef DUMMY_MOTOR
  return 0;
#endif
  if(comAct){
    int res = 0;
    res = penguin_motor_end(pACU.azMotor);
    if(res){
      uM("acuEnd(), Az motor end failed.");
    }
    res = penguin_motor_end(pACU.elMotor);
    if(res){
      uM("acuEnd(), El motor end failed.");
    }
    comAct = 0;
    if(res){
      return res;
    }
  }
  return 0;
  //fclose(fp);//20240109 //20240409 HONDA CO
}

/*! ¥fn int acuGetACU()
  ¥brief ACUからデータを取得する
  ¥retval 0 成功
  ¥retval 0以外 エラーコード参照
*/
int acuGetACU(){
  //uM("acuGetACU(); simple implementation.");
  //uM("acuGetACU(); not implemented yet.");

#ifdef DUMMY_MOTOR
  int azRealPulse = _azcurrentPulse;
  int elRealPulse = _elcurrentPulse;
#else
  int azRealPulse = penguin_motor_getPulse(pACU.azMotor);
  int elRealPulse = penguin_motor_getPulse(pACU.elMotor);
#endif
  //usleep(1000*500);
  //int azRealVelocity = penguin_motor_getVelocityActual(pACU.azMotor);
  //int elRealVelocity = penguin_motor_getVelocityActual(pACU.elMotor);
  pACU.acuStat.dazel[0] = (double)azRealPulse/(double)AZ_MOTOR_A + pACU.azMotorB;//090920 in
  /* 090920 out
     pACU.acuStat.dazel[0] = (double)azRealPulse/(double)AZ_MOTOR_A + (double)AZ_MOTOR_B;
  */
  pACU.acuStat.dazel[1] = (double)elRealPulse/(double)EL_MOTOR_A + (double)EL_MOTOR_B;
  //uM2("acuGetACU(); Az: %d pulse is %f [deg].", azRealPulse, pACU.acuStat.dazel[0]);
  //uM2("acuGetACU(); El: %d pulse is %f [deg].", elRealPulse, pACU.acuStat.dazel[1]);

  pACU.acuStat.zone = 0;
  pACU.acuStat.s[0] = 0;
  pACU.acuStat.s[1] = 0;
  pACU.acuStat.s[2] = 0;
  pACU.acuStat.s[3] = 0;
  pACU.acuStat.s[4] = 0;

  /* 090629 out to improve performace
     usleep(1000*10);
  */
  return 0;
}

/**
 *
 * thread 1, phase A0
 */
int _acuInitRS232c(){
  int azCom, elCom;
  int res;
  if(confSetKey("AzCom"))
    azCom = atoi(confGetVal());
  if(azCom == 0){
    uM1("AzCom(%d) error!!¥n", azCom);
    return -1;
  }
  if(confSetKey("ElCom"))
    elCom = atoi(confGetVal());
  if(elCom == 0){
    uM1("ElCom(%d) error!!¥n", elCom);
    return -1;
  }
  if(confSetKey("Hemisphere"))//091003 in
    pACU.hemisphere = atoi(confGetVal());

  //char devName[20];
  //memset(devName, 0, sizeof(devName));
  //sprintf(devName, "/dev/ttyUSB%d", azCom);
#ifdef DUMMY_MOTOR
  uM("USE DUMMY MOTOR");
  _azcurrentPulse=0;
  _elcurrentPulse=0;
#else
  pACU.azMotor = penguin_motor_init("192.168.11.3", 1, AZ_MAXPULSE,AZ_MINPULSE);
  pACU.elMotor = penguin_motor_init("192.168.11.3", 2, EL_MAXPULSE,EL_MINPULSE);
  if(!pACU.azMotor){
    uM1("Az motor (devID=%d) cannot initialized",1);
    return -1;
  }
  res = penguin_motor_servOn(pACU.azMotor);
  if(res<0){
    uM1("Az motor failed to servo-ON... (returned %d)",res);
    return -1;
  }
  penguin_motor_setVelocity(pACU.azMotor, 10); //20240116
  penguin_motor_goOrigin(pACU.azMotor);

  //memset(devName, 0, sizeof(devName));
  //sprintf(devName, "/dev/ttyUSB%d", elCom);
  if(!pACU.elMotor){
    uM1("El motor (devID=%d) cannot initialized",2);
    return -1;
  }
  res = penguin_motor_servOn(pACU.elMotor);
  if(res<0){
    uM1("El motor failed to servo-ON... (returned %d)",res);
    return -1;
  }
  penguin_motor_setVelocity(pACU.elMotor, 10); //20240116
  penguin_motor_goOrigin(pACU.elMotor);
#endif

  comAct = 1;
  return 0;//normal end
}

/*! ¥fn int acuSetACU(int antZone)
  ¥brief ACUにアンテナを駆動させる
  ¥param[in] antZone 0:最小駆動方向 1:CW 2:CCW
  ¥return 0:成功 -:失敗
*/
int acuSetACU(int antZone){

  /* 090629 out to improve performace
     usleep(1000*10);
  */
  /* Az */
  switch(antZone){
  case 0://nearest
    pACU.acuStat.zone = antZone;//it's fake implemetation
    break;
  case 1://CW
    //uM("acuSetACU(); not implemented yet.");
    pACU.acuStat.zone = antZone;//it's fake implemetation
    break;
  case 2://CCW
    //uM("acuSetACU(); not implemented yet.");
    pACU.acuStat.zone = antZone;//it's fake implemetation
    break;
  default:
    uM("acuSetACU(); CAUTION! invalid antZone.");
  }
  if(pACU.hemisphere){//090920 in, in thee Southern hemisphere
    //Antenna zone 処理前, -180 <= Az < 180
    if(-180 <= pACU.doazelC[0] && pACU.doazelC[0] < -95){//command B/E
      //		if(-180 <= pACU.doazelC[0] && pACU.doazelC[0] < -90){//command B/E
      // 20121023 in
      // 20120928 out
      if(pACU.acuStat.dazel[0] <= 95){//current A/B/C
        //			if(pACU.acuStat.dazel[0] <= 90){//current A/B/C
        //uM2("Antenna is in zone A/B/C (%3.2lf deg) and move to zone B (%3.2lf deg).", pACU.acuStat.dazel[0], pACU.doazelC[0]); //20240208 コメントアウト（本多）
      }else{//current D/E
        pACU.doazelC[0] += 360;
        //uM2("Antenna is in zone D/E (%3.2lf deg) and move to zone E (%3.2lf deg).", pACU.acuStat.dazel[0], pACU.doazelC[0]); //20240208 コメントアウト（本多）
      }

    }else if(-95 <= pACU.doazelC[0] && pACU.doazelC[0] <= 95){//command C
      //		}else if(-90 <= pACU.doazelC[0] && pACU.doazelC[0] <= 90){//command C
      // 20121023in
      // 20120928 out
      //uM2("Antenna is in zone A/B/C/D/E (%3.2lf deg) and move to zone C (%3.2lf deg).", pACU.acuStat.dazel[0], pACU.doazelC[0]); //20240208 コメントアウト（本多）

    }else if(95 < pACU.doazelC[0] && pACU.doazelC[0] < 180){//command A/D
      //		}else if(90 < pACU.doazelC[0] && pACU.doazelC[0] < 180){//command A/D
      if(-95 <= pACU.acuStat.dazel[0]){//current C/D/E
        //			if(-90 <= pACU.acuStat.dazel[0]){//current C/D/E
        // 20121023 in
        // 20120928 out
        //uM2("Antenna is in zone C/D/E (%3.2lf deg) and move to zone D (%3.2lf deg).", pACU.acuStat.dazel[0], pACU.doazelC[0]); //20240208 コメントアウト（本多）

      }else{//current A/B
        pACU.doazelC[0] -= 360;
        // 20121023 in
        // 20120928 out
        //uM2("Antenna is in zone A/B (%3.2lf deg) and move to zone A (%3.2lf deg).", pACU.acuStat.dazel[0], pACU.doazelC[0]); //20240208 コメントアウト（本多）

      }
    }else{
      uM("FATAL LOGICAL ERROR in acuSetACU() #1 !!!");
    }
    //Antenna zone 処理後, -270 < Az < 270
  }else{//In the Northern hemisphere
    //Antenna zone 処理前, 0 <= Az < 360
    if(0 <= pACU.doazelC[0] && pACU.doazelC[0] < 85){//command B/E
      //		if(0 <= pACU.doazelC[0] && pACU.doazelC[0] < 90){//command B/E
      if(pACU.acuStat.dazel[0] <= 275){//current A/B/C
        //			if(pACU.acuStat.dazel[0] <= 270){//current A/B/C
        //M2("Antenna is in zone A/B/C (%3.2lf deg) and move to zone B (%3.2lf deg).", pACU.acuStat.dazel[0], pACU.doazelC[0]); //20240208 コメントアウト（本多）
      }else{//current D/E
        pACU.doazelC[0] += 360;
        //uM2("Antenna is in zone D/E (%3.2lf deg) and move to zone E (%3.2lf deg).", pACU.acuStat.dazel[0], pACU.doazelC[0]); //20240208 コメントアウト（本多）
      }
    }else if(85 <= pACU.doazelC[0] && pACU.doazelC[0] <= 275){//command C
      //		}else if(90 <= pACU.doazelC[0] && pACU.doazelC[0] <= 270){//command C
      //uM2("Antenna is in zone A/B/C/D/E (%3.2lf deg) and move to zone C (%3.2lf deg).", pACU.acuStat.dazel[0], pACU.doazelC[0]); //20240208 コメントアウト（本多）
    }else if(275 < pACU.doazelC[0] && pACU.doazelC[0] < 360){//command A/D
      //		}else if(270 < pACU.doazelC[0] && pACU.doazelC[0] < 360){//command A/D
      if(85 <= pACU.acuStat.dazel[0]){//current C/D/E
        //			if(90 <= pACU.acuStat.dazel[0]){//current C/D/E
        //uM2("Antenna is in zone C/D/E (%3.2lf deg) and move to zone D (%3.2lf deg).", pACU.acuStat.dazel[0], pACU.doazelC[0]); //20240208 コメントアウト（本多）
      }else{//current A/B
        pACU.doazelC[0] -= 360;
        //uM2("Antenna is in zone A/B (%3.2lf deg) and move to zone A (%3.2lf deg).", pACU.acuStat.dazel[0], pACU.doazelC[0]); //20240208 コメントアウト（本多）
      }
    }else{
      uM("FATAL LOGICAL ERROR in acuSetACU() #2 !!!");
    }
    //Antenna zone 処理後, -90 < Az < 450
  }

  int azPulse = (int)((pACU.doazelC[0] - pACU.azMotorB) * AZ_MOTOR_A);//090920 in
  //uM2("acuSetACU(); Az: %f [deg] is %d pulse.", pACU.doazelC[0], azPulse);
  const int elPulse = (int)((pACU.doazelC[1] - EL_MOTOR_B) * EL_MOTOR_A);
  //uM2("acuSetACU(); El: %f [deg] is %d pulse.", pACU.doazelC[1], elPulse);

#ifdef DUMMY_MOTOR
  _azcurrentPulse=azPulse;
  _elcurrentPulse=elPulse;
#else
  //penguin_motor_setVelocity(pACU.azMotor, 1);
  //printf("AZ setpulse --> (doaz %f - azoff %f) * azpulse %f =  %d \n",pACU.doazelC[0],pACU.azMotorB,AZ_MOTOR_A,azPulse); //20240208 コメントアウト（本多）
  penguin_motor_setPulse(pACU.azMotor, azPulse);
  //penguin_motor_setVelocity(pACU.elMotor, 1);
  //printf("EL setpulse --> (doel %f - eloff %f) * elpulse %f =  %d \n",pACU.doazelC[1],EL_MOTOR_B,EL_MOTOR_A,elPulse); //20240208 コメントアウト（本多）
  penguin_motor_setPulse(pACU.elMotor, elPulse);
#endif
  // 20240409 HONDA CO for speeding up
  // // 20240130 raster scanが動かないのでコメントアウト // 20240208 コメントアウト（本多）
  // //  int t = 0;
  // int curaz,curel, current_az, current_el, effective_load_az, effective_load_el, load_az, load_el;
  // //  while(penguin_motor_isReady(pACU.azMotor)==0 || penguin_motor_isReady(pACU.elMotor)== 0) {
  // curaz = penguin_motor_getPulse(pACU.azMotor);
  // curel = penguin_motor_getPulse(pACU.elMotor);
  // // 20240409 モーターの検出電流、実効負荷率、負荷率をpenguin_motorから取得してアンテナログと一緒に残す　
  // current_az = penguin_motor_getDetectedCurrent(pACU.azMotor);
  // current_el = penguin_motor_getDetectedCurrent(pACU.elMotor);

  // effective_load_az = penguin_motor_getEffectiveLoadFactor(pACU.azMotor);
  // effective_load_el = penguin_motor_getEffectiveLoadFactor(pACU.elMotor);

  // load_az = penguin_motor_getLoadFactor(pACU.azMotor);
  // load_el = penguin_motor_getLoadFactor(pACU.elMotor);

  //    uM4("pulse moving AZ: %d -> %d, EL: %d -> %d",curaz,azPulse,curel,elPulse);
  //    usleep(100*1000); // wait to finish moving
  //fprintf(fp, "[%s] AZ_real, EL_real, AZ_cmd, EL_cmd,current_az, current_el, e_load_az,e_load_el, load_az, load_el,  %f, %f, %f, %f, %d,%d, %d, %d, %d, %d\n",uGetDate(), (curaz/AZ_MOTOR_A)+AZ_MOTOR_B, 
  //(curel/EL_MOTOR_A)+EL_MOTOR_B,(azPulse/AZ_MOTOR_A)+AZ_MOTOR_B, (elPulse/EL_MOTOR_A)+EL_MOTOR_B, current_az, 
  //current_el, effective_load_az, effective_load_el, load_az, load_el); // 20240409 HONDA CO
  //20240126  
  //  t += 1;
  //    if(t>1000){
  //      uM("acuSetACU(); MOTOR TIMEOUT!!");
  //      break;
  //    }
  //  }
  return 0;
}

/*! ¥fn int acuSetAcuMood(int mood)
  ¥brief ACUのモード制御
  ¥param[in] mood 0:スタンバイ 2:プログラム追尾 3:StowLock
  ¥return 0:成功 -:失敗
*/
int acuSetAcuMood(int mood){
  //uM("acuSetAcuMood(); not implemented yet.");
  /* 090629 out to improve performace
     usleep(1000);
  */
  return 0;
}

/*! ¥fn int acuSetAcuBan(int ban)
  ¥brief 駆動禁止制御
  ¥param[in] ban 0:駆動可 1:駆動禁止
  ¥return 0:成功 -:失敗
*/
int acuSetAcuBan(int ban){
  uM("acuSetAcuBan(); not implemented yet.");
  /* 090629 out to improve performace
     usleep(1000);
  */
  return 0;
}

/*! ¥fn int acuSetAcuUnStow()
  ¥brief StowUnlock制御
  ¥retval 0 成功
  ¥retval 0以外 エラーコード
*/
int acuSetAcuUnStow(){
  //! STOW解除
  uM("acuSetAcuUnStow(); not implemented yet.");
  /* 090629 out to improve performace
     usleep(1000);
  */
  return 0;
}

int acuSetAcuVeloc(double* azel){ // 240202
#ifdef DUMMY_MOTOR
  return 0;
#endif
  // azel [deg/sec]
  int pulse = 0;
  pulse = (int)(azel[0] * 60 / 360 * 10); // [0.1r/min]
  penguin_motor_setVelocity(pACU.azMotor, pulse);
  pulse = (int)(azel[1] * 60 / 360* 10); // [0.1r/min]
  penguin_motor_setVelocity(pACU.elMotor, pulse);
  return 0;
}

/*! ¥fn int acuSafetyCheck()
  ¥brief ACUのステータスでエラーが無いかチェックする
  ¥return 0:成功, -1:失敗
*/
int acuSafetyCheck(){
  /*
    uM("acuSafetyCheck()");
    int res = penguin_motor_getAlermState(pACU.azMotor);
    if(res){
    uM1("Az motor Alerm: %d", res);
    }
    res = penguin_motor_getAlermState(pACU.elMotor);
    if(res){
    uM1("El motor Alerm: %d", res);
    }
    uM("acuSafetyCheck(); simple implementation.");
  */
  //uM("acuSafetyCheck(); not implemented yet.");
  return 0;
}


/*! ¥fn int acuSetDoAzC()
  ¥brief ACUに指令するAzを指定する
  \param[in] az [deg] 0 <= az < 360
*/
void acuSetDoAzC(double az){
  if(pACU.hemisphere){
    uM1("20231115 acu.cpp hemisphere = %d", pACU.hemisphere);//090920 in, in the Southern Hemisphere
    if(az >= 180){//090929 in
      /* 090929 out
         if(az > 180){
      */
      pACU.doazelC[0] = az-360;
    }else{//091003 in
      pACU.doazelC[0] = az;
    }
  }else{//In the Northern hemisphere
    pACU.doazelC[0] = az;
  }
}
/*! ¥fn int acuSetDoElC()
  ¥brief ACUに指令するElを指定する
  \param[in] el [deg]
*/
void acuSetDoElC(double el){
  pACU.doazelC[1] = el;
}

/*! ¥fn void acuGetRZONE(int* RZONE)
  ¥brief アンテナZoneの実際値
  ¥param[out] RZONE 0:最小駆動角方向 1:CW 2:CCW
*/
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

// 20240409 HONDA
void acuGetACULoadStatus(int* status){
#ifdef DUMMY_MOTOR
  return;
#endif

  int tmp_status[3];
  // tmp_status[0] = penguin_motor_getDetectedCurrent(pACU.azMotor);
  tmp_status[0] = penguin_motor_getDetectedCurrent(pACU.elMotor);
  // tmp_status[2] = penguin_motor_getEffectiveLoadFactor(pACU.azMotor);
  tmp_status[1] = penguin_motor_getEffectiveLoadFactor(pACU.elMotor);
  // tmp_status[4] = penguin_motor_getLoadFactor(pACU.azMotor);
  tmp_status[2] = penguin_motor_getLoadFactor(pACU.elMotor);
  memcpy(status, tmp_status, sizeof(int) * 3);
  return;
}

/*! ¥fn void trkGetRAZEL(double* RAZEL)
  ¥brief アンテナ実際角RAZELの取得
  ¥param[out] RAZEL [deg]; 0 <= Az < 360
*/
void acuGetRAZEL(double* RAZEL){
  //usleep(1000*600);
  /*
    int azRealPulse = penguin_motor_getPulseActual(pACU.azMotor);
    int elRealPulse = penguin_motor_getPulseActual(pACU.elMotor);
    pACU.acuStat.dazel[0] = azRealPulse/AZ_MOTOR_A + AZ_MOTOR_B;
    pACU.acuStat.dazel[1] = elRealPulse/EL_MOTOR_A + EL_MOTOR_B;
  */
  memcpy(RAZEL, pACU.acuStat.dazel, sizeof(double)*2);
  if(RAZEL[0] < 0){
    RAZEL[0] += 360;
  }
  if(RAZEL[0] >= 360){
    RAZEL[0] -= 360;
  }
  return;
}

#endif


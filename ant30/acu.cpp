/*!
  ¥file acu.cpp
  ¥author NAGAI Makoto
  ¥date 2008.12.8
  ¥brief ACU Program for 30-cm telescope
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>//usleep()

#include "configuration.h"
#include "acu.h"
#include "errno.h"

#define AZ_MOTOR_OFFSET 90.0//165.0//(190.7)
#define EL_MOTOR_OFFSET 75.0//-10.0//(64.9)

#define ANT_ZONE_EDGE_NEG_HI 0
#define ANT_ZONE_EDGE_NEG_LO 85
#define ANT_ZONE_EDGE_POS_LO 275
#define ANT_ZONE_EDGE_POS_HI 360

//! ACUの状態を格納しておく構造体
typedef struct sAcu{
  int zone;             // antenna zone = 1:CW 2:CCW
  double dazel[2];      // current azel angle list [deg]
  int s[5];
}tAcu;

typedef struct {
  penguin_motor_t* azMotor;  // az motor
  penguin_motor_t* elMotor;  // el motor
  tAcu acuStat;              // current acu status
  int acuStatSBefore[5];     // previous acu status
  double doazelC[2];         // target azel angle list [deg] before antenna zone proc.
                             // 0 <= Az < 360 in North.Hem. / -180 <= Az < 180 in South.Hem.
  int hemisphere;            // 0: Northern, 1: Southern
  double antzoneEdge[4];
  double azMotorOffset;      // offset [deg]
  double elMotorOffset;      // offset [deg]
}tParamACU;

static int isInitialized=0;
static tParamACU pACU;
static int comAct=0;

static int _acuInitRS232c();

int acuInit(){
  if(isInitialized == 1){
    uM("WARNING acuInit(): RpcomFit already initialized.");
    return 0;
  }
  comAct = 0;
  isInitialized = 1;

  int res = _acuInitRpcomFit();
  if(res){
    uM("ERROR acuInit(): RpcomFit initialization failed.");
    return res;
  }

  memset(&pACU.acuStat,       0, sizeof(pACU.acuStat));
  memset(pACU.acuStatSBefore, 0, sizeof(pACU.acuStatSBefore[0])*5);
  memset(pACU.doazelC,        0, sizeof(pACU.doazelC[0])*2);

  if(pACU.hemisphere){ // In Southern Hemisphere
    uM("acuInit(): SOUTHERN hemisphere mode");
    pACU.azMotorOffset = AZ_MOTOR_OFFSET -180;
    pACU.elMotorOffset = EL_MOTOR_OFFSET;
    pACU.antzoneEdge[0] = ANT_ZONE_EDGE_NEG_HI -180;
    pACU.antzoneEdge[1] = ANT_ZONE_EDGE_NEG_LO -180;
    pACU.antzoneEdge[2] = ANT_ZONE_EDGE_POS_LO -180;
    pACU.antzoneEdge[3] = ANT_ZONE_EDGE_POS_HI -180;
  }
  else{ // In Northern Hemisphere
    uM("acuInit(): NORTHERN hemisphere mode");
    pACU.azMotorOffset = AZ_MOTOR_OFFSET;
    pACU.elMotorOffset = EL_MOTOR_OFFSET;
    pACU.antzoneEdge[0] = ANT_ZONE_EDGE_NEG_HI;
    pACU.antzoneEdge[1] = ANT_ZONE_EDGE_NEG_LO;
    pACU.antzoneEdge[2] = ANT_ZONE_EDGE_POS_LO;
    pACU.antzoneEdge[3] = ANT_ZONE_EDGE_POS_HI;
  }

  return 0;
}

int acuEnd(){
  if(comAct == 0) return 0;
  int res = 0;
  res = penguin_motor_end(pACU.azMotor);
  if(res){
    uM("ERROR acuEnd(): failed for AZ motor");
  }
  res = penguin_motor_end(pACU.elMotor);
  if(res){
    uM("ERROR acuEnd(): failed for EL motor");
  }
  comAct = 0;
  return res;
}

int acuGet(){
  int azRealPulse = penguin_motor_getPulse(pACU.azMotor);
  int elRealPulse = penguin_motor_getPulse(pACU.elMotor);
  pACU.acuStat.dazel[0] = penguin_motor_calcAngle(penguin_motor_getPulse(pACU.azMotor)) + pACU.azMotorOffset
  pACU.acuStat.dazel[1] = penguin_motor_calcAngle(penguin_motor_getPulse(pACU.elMotor)) + pACU.elMotorOffset

  pACU.acuStat.zone = 0;
  pACU.acuStat.s[0] = 0;
  pACU.acuStat.s[1] = 0;
  pACU.acuStat.s[2] = 0;
  pACU.acuStat.s[3] = 0;
  pACU.acuStat.s[4] = 0;

  return 0;
}

int _acuInitRpcomFit(){
  char* azip, elip;
  int azch=0;
  int elch=0;
  int azlim_pos=0;
  int azlim_neg=0;
  int ellim_pos=0;
  int ellim_neg=0;

  if(confSetKey("AzRpcomFitIp"))
    azip = confGetVal();
  if(strlen(azip)==0){
    uM1("ERROR:: Configuration parameter \"AzRpcomFitIp\" is empty. ¥n", azip);
    return -1;
  }
  if(confSetKey("AzRpcomFitCh"))
    azCom = atoi(confGetVal());
  if(azCom == 0){
    uM1("ERROR:: Configuration parameter \"AzRpcomFit\" = %d is invalid. ¥n", azCom);
    return -1;
  }
  if(confSetKey("AzPositiveLimit"))
    azlim_pos = atoi(confGetVal());
  if(azlim_pos == 0){
    uM1("ERROR:: Configuration parameter \"AzPositiveLimit\" = %d is invalid. ¥n", azlim_pos);
    return -1;
  }
  if(confSetKey("AzNegativeLimit"))
    azlim_pos = atoi(confGetVal());
  if(azlim_pos == 0){
    uM1("ERROR:: Configuration parameter \"AzNegativeLimit\" = %d is invalid. ¥n", azlim_neg);
    return -1;
  }

  if(confSetKey("ElRpcomFitIp"))
    elip = confGetVal();
  if(strlen(elip)==0){
    uM1("ERROR:: Configuration parameter \"ElRpcomFitIp\" is empty. ¥n", elip);
    return -1;
  }
  if(confSetKey("ElRpcomFitCh"))
    elCom = atoi(confGetVal());
  if(elCom == 0){
    uM1("ERROR:: Configuration parameter \"ElRpcomFit\" = %d is invalid. ¥n", elCom);
    return -1;
  }
  if(confSetKey("ElPositiveLimit"))
    ellim_pos = atoi(confGetVal());
  if(ellim_pos == 0){
    uM1("ERROR:: Configuration parameter \"ElPositiveLimit\" = %d is invalid. ¥n", ellim_pos);
    return -1;
  }
  if(confSetKey("ElNegativeLimit"))
    ellim_pos = atoi(confGetVal());
  if(ellim_pos == 0){
    uM1("ERROR:: Configuration parameter \"ElNegativeLimit\" = %d is invalid. ¥n", ellim_neg);
    return -1;
  }

  if(confSetKey("Hemisphere"))
    pACU.hemisphere = atoi(confGetVal());

  pACU.azMotor = penguin_motor_init(azip, azch, azlim_pos, azlim_neg);
  if(!pACU.azMotor){
    uM1("AZ motor (IP=%s, channel=%d) cannot initialized",azip,azch);
    return -1;
  }
  if(penguin_motor_isOriginDetected(pACU.azMotor)==0){
    uM1("AZ motor not detect origin point");
    int res = penguin_motor_goOrigin(pACU.azMotor);
    if(res<0){
      uM1("AZ motor communication error");
      return -1;
    }
    unsigned int len = 0;
    while(len<1000){ // wait to survo-on for <100sec
      if((penguin_motor_isOriginDetected(pACU.azMotor)==1) && (penguin_motor_isReady(pACU.azMotor)==1)) break;
      if(penguin_motor_isLimit(pACU.azMotor)){
        uM1("AZ motor limit switch enabled");
        return -1;
      }
      usleep(100*1000);
      len++;
    }
    if(len>=1000) return -1;
  }

  pACU.elMotor = penguin_motor_init(elip, elch, ellim_pos, ellim_neg);
  if(!pACU.elMotor){
    uM1("EL motor (IP=%s, channel=%d) cannot initialized",elip,elch);
    return -1;
  }
  if(penguin_motor_isOriginDetected(pACU.elMotor)==0){
    uM1("EL motor not detect origin point");
    int res = penguin_motor_goOrigin(pACU.elMotor);
    if(res<0){
      uM1("EL motor communication error");
      return -1;
    }
    unsigned int len = 0;
    while(len<1000){ // wait to survo-on for <100sec
      if((penguin_motor_isOriginDetected(pACU.elMotor)==1) && (penguin_motor_isReady(pACU.elMotor)==1)) break;
      if(penguin_motor_isLimit(pACU.elMotor)){
        uM1("EL motor limit switch enabled");
        return -1;
      }
      usleep(100*1000);
      len++;
    }
    if(len>=1000) return -1;
  }

  penguin_motor_servOn(pACU.azMotor);
  penguin_motor_servOn(pACU.elMotor);

  penguin_motor_setVelocity(pACU.azMotor,1);
  penguin_motor_setVelocity(pACU.elMotor,1);

  comAct = 1;

  return 0;
}

int acuSet(int antZone){

  // AZ
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
    uM("WARNING acuSet() invalid antZone = %d",antZone);
  }
  // Antenna zone 処理前, -180 <= Az < 180 (South.Hem.) / 0 <= Az < 360 (North.Hem.)
  if(pACU.antzoneEdge[0] <= pACU.doazelC[0] && pACU.doazelC[0] < pACU.antzoneEdge[1]){ //command B/E
    if(pACU.acuStat.dazel[0] <= pACU.antzoneEdge[2]){ //current A/B/C
      uM2("Antenna is in zone A/B/C (%3.2lf deg) and move to zone B (%3.2lf deg).", pACU.acuStat.dazel[0], pACU.doazelC[0]);
    }else{
      pACU.doazelC[0] += 360;
      uM2("Antenna is in zone D/E (%3.2lf deg) and move to zone E (%3.2lf deg).", pACU.acuStat.dazel[0], pACU.doazelC[0]);
    }
  }else if(pACU.antzoneEdge[1] <= pACU.doazelC[0] && pACU.doazelC[0] <= pACU.antzoneEdge[2]){ //command C
    uM2("Antenna is in zone A/B/C/D/E (%3.2lf deg) and move to zone C (%3.2lf deg).", pACU.acuStat.dazel[0], pACU.doazelC[0]);
  }else if(pACU.antzoneEdge[2] < pACU.doazelC[0] && pACU.doazelC[0] < pACU.antzoneEdge[3]){ //command A/D
    if(pACU.antzoneEdge[1] <= pACU.acuStat.dazel[0]){ //current C/D/E
      uM2("Antenna is in zone C/D/E (%3.2lf deg) and move to zone D (%3.2lf deg).", pACU.acuStat.dazel[0], pACU.doazelC[0]);
    }else{//current A/B
      pACU.doazelC[0] -= 360;
      uM2("Antenna is in zone A/B (%3.2lf deg) and move to zone A (%3.2lf deg).", pACU.acuStat.dazel[0], pACU.doazelC[0]);
    }
  }
  //Antenna zone 処理後, -270 < Az < 270 / -90 < Az < 45

  int azPulse = penguin_motor_calcPulse(pACU.doazelC[0] - pACU.azMotorOffset);
  int elPulse = penguin_motor_calcPulse(pACU.doazelC[0] - pACU.elMotorOffset);

  //penguin_motor_setVelocity(pACU.azMotor, 1);
  printf("AZ setpulse --> (doaz %f - azoff %f) * azpulse %f =  %d",pACU.doazelC[0],pACU.azMotorB,AZ_MOTOR_A,azPulse);
  penguin_motor_setPulse(pACU.azMotor, azPulse);

  /* El */
  //penguin_motor_setVelocity(pACU.elMotor, 1);
  printf("EL setpulse --> (doel %f - eloff %f) * elpulse %f =  %d",pACU.doazelC[1],EL_MOTOR_B,EL_MOTOR_A,elPulse);
  penguin_motor_setPulse(pACU.elMotor, elPulse);

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
  return 0;
}

/*! ¥fn int acuSafetyCheck()
  ¥brief ACUのステータスでエラーが無いかチェックする
  ¥return 0:成功, -1:失敗
*/
int acuSafetyCheck(){
  uM("acuSafetyCheck()");
  int res = penguin_motor_getAlermState(pACU.azMotor);
  if(res){
    uM1("Az motor Alerm: %d", res);
  }
  res = penguin_motor_getAlermState(pACU.elMotor);
  if(res){
    uM1("El motor Alerm: %d", res);
  }
  return 0;
}


/*! ¥fn int acuSetDoAzC()
  ¥brief ACUに指令するAzを指定する
  \param[in] az [deg] 0 <= az < 360
*/
void acuSetDoAzC(double az){
  if(pACU.hemisphere && az >= 180) pACU.doazelC[0] = az-360;
  else pACU.doazelC[0] = az;
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
void acuGetStatus(int* status){
  memcpy(status, pACU.acuStat.s, sizeof(int) * 5);
  return;
}

/*! ¥fn void trkGetRAZEL(double* RAZEL)
  ¥brief アンテナ実際角RAZELの取得
  ¥param[out] RAZEL [deg]; 0 <= Az < 360
*/
void acuGetRAZEL(double* RAZEL){
  memcpy(RAZEL, pACU.acuStat.dazel, sizeof(double)*2);
  if(RAZEL[0] <    0) RAZEL[0] += 360;
  if(RAZEL[0] >= 360) RAZEL[0] -= 360;
  return;
}


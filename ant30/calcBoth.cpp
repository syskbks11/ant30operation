/*!
  ¥file calcBoth.cpp
  ¥author NAGAI Makoto
  ¥date 2008.11.14
  ¥brief Tracking Calculation Program for 32-m telescope & 30-cm telescope
  * to gether calc.cpp and calc2.cpp
  */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>
//#include <sys/types.h>
//#include <pthread.h>

#include "configuration.h"
#include "celestialObject.h"
#include "scanTable.h"
#include "trk45Sub.h"
#include "errno.h"
#include "trkCalc.h"
#include "calcBoth.h"
#include "ifCalcBoth.h"
#include "instrument.h"

//#define PI  3.14159265358979323846
#define PI M_PI


typedef struct {
  double dweath[3];     //!< 気温 [℃], 気圧 [hPa] 水蒸気圧 [hPa]
  /* 090604 out
     int isFinishedTrk_00;	//!< trk_00()を呼び出すタイミング用。UTにおける日付が変わったとき呼ぶ。
     XTRKI xin;            //!< 45m追尾ライブラリ入力パラメーター構造体
     XTRKO xout;           //!< 45m追尾ライブラリ計算結果出力用構造体
  */
  /* 090206 out
     int scanOffsetCoord;	//!< 現在のスキャンオフセットの座標系 1:RADEC 2:LB 3:AZEL
  */
  /* 090206 moved to instrument.cpp
     double P[30];         //!< アンテナ器差補正パラメータ used only in trkCalInst()
  */
  double doazel[2];     //!< 計算結果AZEL [rad]
  double doazel2[2];    //!< 計算結果に器差補正を加えた値 [deg]
  double dgtdat[8][2];  //!< MapCenter(RADEC)(LB) Obs(RADEC)(LB) [rad] 
  double TrkTimeOffset; //!< 追尾計算を行うときに何秒後の計算を行うかセットする 単位sec
  unsigned int lst;     //!< LST時分秒のみを秒換算したもの
  /* 090604 out
     char cstart[24];      //!< 観測時刻の設定 YYYYMMDDhhmmss.0 (JST)
     tmClass_t* vtm;		//!< trk_00()更新時間管理用
  */
  glacier_ScanOffsetClass_t* scanOffset;//!< 現在のスキャンオフセット
  glacier_ScanOffsetClass_t* pointingOffset;//!< 現在のポインティングオフセット

  int sourceDefinition;
  int sourceFrame;
  double trackingFrequency;	//!< [Hz]
  double dvrad; 	//!< [m/s]
  double dopplerShiftedFrequency;
}tParamCalc;


//static tParamTrk *p;
static tParamCalc pCalc;

static const double ABS_ZERO = 273.15;
//const int mapNumMax = 20;
static const double sec2rad = PI / (180.0 * 3600.0);
static const double rad2sec = 180.0 * 3600.0 / PI;

/*enum eAcuCmd {STX=0x02, ETX=0x03};
  enum eAcuS1 {EL_DRIVE_DISABLE=0x38, AZ_DRIVE_DISABLE=0x34, EL_DRIVE_ENABLE=0x32, AZ_DRIVE_ENABLE=0x31};
  enum eAcuS2 {TRACK_MODE_FAULT=0x38, STANDBY=0x34, PROGRAM_TRACK_MODE=0x32};
  enum eAcuS3 {REMOTE_CONTROL_MODE=0x38, LOCAL_CONTROL_MODE=0x34, STOW=0x32};
  enum eAcuS4 {TOTAL_ALARM=0x34};
  enum eAcuS5 {ANGLE_FAULT=0x34, AZ_DCPA_FAULT=0x32, EL_DCPA_FAULT=0x31};
*/

/* 0090206 moved to instrument.cpp
   static void _calcCalInst(double X, double Y, double* dX, double* dY);
*/
//static void _calcCalZoneType(double az);//081117 moved to trk.cpp

/*
  static void _calcSetXinDscn(int coord, double dscnst0, double dscnst1, double dscned0, double dscned1);//090206 in

  static void _calcSetXinIFlags(int irpflg, int iapflg, int irsflg, int iasflg);
  static void _calcSetXinIScn(int itnscn, int inoscn);
*/

static void setParam();
static int checkParam();
/*
  Invoked by trkInit()
  Thus, this function is excuted once for each tkb32 run. 
  * thread 1, phase A0
  */
int calcInit(){
  //p = param;
  /* 090604 out
     pCalc.vtm = tmInit();//081117 in
     pCalc.isFinishedTrk_00 = 0;
  */


  // 090529 in, Use penguin_tracking
  int ret = penguin_tracking_Init();
  if(ret){
    printf("penguin_tracking_Init() failed, exit.\n");
    exit(1);
  }

  ret = penguin_tracking_loadConfigWithConfInit();
  if(ret){
    printf("penguin_tracking_loadConfigWithConfInit() failed, exit.\n");
    exit(1);
  }

  pCalc.scanOffset = glacier_ScanOffsetInit();
  pCalc.pointingOffset = glacier_ScanOffsetInit();

  // default values
  glacier_ScanOffsetSetInAzEl(pCalc.scanOffset, 0, 0);//090605 in

  //	glacier_ScanOffsetSetInLB(pCalc.pointingOffset, 0, 0);
  //	glacier_ScanOffsetSetXY(pCalc.pointingOffset, CELESTIAL_SCAN_COORDINATE_AZEL, 0, 0);
  glacier_ScanOffsetSetInAzEl(pCalc.pointingOffset, 0, 0);//090605 in

  return 0;
}

/*
  Invoked by trkEnd()
  Thus, this function is excuted once for each tkb32 run. 
*/
void calcEnd(){
  /* 090604 out
     if(pCalc.vtm){
     tmEnd(pCalc.vtm);
     pCalc.vtm = NULL;
     }
  */
  // 090529 in, Use penguin_tracking
  penguin_tracking_end();

  glacier_ScanOffsetEnd(pCalc.scanOffset);//090603 in
  glacier_ScanOffsetEnd(pCalc.pointingOffset);//090603 in
}

/*
 * Invoked by trkInitParam()
 * Thus, this function is excuted once for each initialization of observation tables. 
 * thread 1, phase C0
 */
int calcInitParam(){

  /* 0090206 moved to instrument.cpp
     memset(pCalc.P, 0, sizeof(pCalc.P[0]) * 30);
  */
  memset(pCalc.doazel, 0, sizeof(pCalc.doazel[0]) * 2);
  memset(pCalc.doazel2, 0, sizeof(pCalc.doazel2[0]) * 2);
  /* 090604 out
     memset(pCalc.cstart, 0, sizeof(pCalc.cstart[0]) * 24);
     memset(&pCalc.xout, 0, sizeof(pCalc.xout));
     memset(&pCalc.xin, 0, sizeof(pCalc.xin));
  */
  for(int i = 0; i < 8; i++){
    memset(pCalc.dgtdat[i], 0, sizeof(pCalc.dgtdat[0][0]) * 2);
  }
	
  /* 090206 out
     pCalc.scanOffsetCoord = 0;
  */
  pCalc.TrkTimeOffset = 0;

  setParam();
  instrumentInitParam();//090206 in

  /* 090529 out
  //! Initialize trk45 library
  tmReset(pCalc.vtm);
  strcpy(pCalc.cstart, tmGetTimeStr(pCalc.vtm, pCalc.TrkTimeOffset));
  char cerr[256];
  if(trk_00(pCalc.cstart, "", cerr) != 0) {
  uM2("trkInitParam(); trk_00(%s) %s", pCalc.cstart, cerr);
  return TRK_TRK00_ERR;
  }
  pCalc.lst = 0;
  */
  //	pCalc.isFinishedTrk_00 = 0;//081117 in force update pCalc.cstart if JST become ****/**/** 09:**
  /*
  //pCalc.isFinishedTrk_00 = 1;//081117 out
  */

  return 0;
}

/*! ¥fn int trkCal()
  ¥brief 追尾計算部
  ¥retval 0 成功
  ¥retval マイナス値 失敗
*/
int calcTrkCal(int currOffFlag, int currOffNo){
  /* 090604 out
     char cerr[256];

     //! trk_20用変数
     double dtime;
  */
  //! trk_30用変数
  //tmClass_t* timeStart = tmInit();//090619 in to measure performance. 
  double diazel[2], dorade[2];
  static double dgtloc;        //!< 瞬間の視恒星時 [rad]

  //! 作業用変数
  int ival;
  //double daz, del;
  //double timeElapsed = tmGetLag(timeStart)*1000;//[ms]
  //uM1("before penguin_tracking_setCurrentTime() used %lf ms.", timeElapsed);
  //tmReset(timeStart);//090619 in to measure performance. 

  int res = penguin_tracking_setCurrentTime(pCalc.TrkTimeOffset);
  //timeElapsed = tmGetLag(timeStart)*1000;//[ms]
  //uM1("penguin_tracking_setCurrentTime() used %lf ms.", timeElapsed);
  if(res){
    uM1("penguin_tracking_setCurrentTime() returns %d.", res);
  }
  /* 090529 out
  //! UTにおいて日にちが変わっていたらtrk_00を実行する
  tmReset(pCalc.vtm);
  strcpy(pCalc.cstart, tmGetTimeStr(pCalc.vtm, pCalc.TrkTimeOffset));
  if(pCalc.cstart[8] == '0' && pCalc.cstart[9] == '9'){         //!< JST時刻との比較 JSTが09時の意
  if(pCalc.isFinishedTrk_00 == 0){
  uM1("trkCal(); trk_00(%s)", pCalc.cstart);
  if(trk_00(pCalc.cstart, "", cerr) != 0) {
  uM1("trkCal(); %s",cerr);
  return TRK_TRK00_ERR;
  }
  */
  //      pCalc.isFinishedTrk_00 = 1;//do not update pCalc.cstart even if JST is ****/**/** 09:**
  /*    }
        }
        else{
  */
  //    pCalc.isFinishedTrk_00 = 0;//force update pCalc.cstart if JST become ****/**/** 09:**
  /*
    }
  */
  //tmReset(timeStart);//090619 in to measure performance. 
  glacier_CelestialObjectClass_t* celestialObject = glacier_CelestialObjectInit();


  //! trk_10
  //! 第２引数は処理フラグ 0:AZEL
  //! uM2("debug trkCal(); sourc=(%lf,%lf)", p->celObj.dsourc[0], p->celObj.dsourc[1]);
  if(scanTableGetOffMode() == 2 && currOffFlag == 1){//081117 in
    /* 090608 This is equivalent part. 
    //! trk_10用変数
    double offRad[4];            //!< 絶対座標指定のOFF点の位置 offX1 offY1 offX2 offY2 [rad] 090421 in
    for(int i = 0; i < 4; i++){// 090421 in // 090608 in for 30 cm
    offRad[i] = p.off[i] * sec2rad;
    }
    ival = trk_10(0, 0, p.OffCoord, p.ibjflg, &offRad[p.offNo*2], 0);// 090421 in
    */
    // 090529 in, Use penguin tracking
    glacier_CelestialObjectSetSource(celestialObject, 0);
    const double* xy = scanTableGetOff(currOffNo*2);
    //		double y = scanTableGetOff(currOffNo*2+1);
    /*
     */
    glacier_CelestialObjectSetCoordinate(celestialObject, scanTableGetOffCoord(), xy[0] * sec2rad, xy[1] * sec2rad);// 090608 in

    /* 090608 out
       glacier_CelestialObjectSetCoordinate(celestialObject, scanTableGetOffCoord(), xy[0], xy[1]);
    */
    glacier_CelestialObjectSetEpoch(celestialObject, celestialObjectGetEpoch());

    /* 090529 out
    //  if(scanTableGetOffMode() == 2 && p->curr.offFlg == 1){
    //! 絶対座標指定のOFF点を観測中の場合。0:太陽系外 0:AZEL :OffCoord ibjflg:On点と同じ元期 :座標 0:速度 
    ival = trk_10(0, 0, scanTableGetOffCoord(), celestialObjectGetEpoch(), scanTableGetOff(currOffNo*2), 0);//081117 in
    //    ival = trk_10(0, 0, scanTableGetOffCoord(), celestialObjectGetEpoch(), &p->seq.off[p->curr.offNo*2], 0);//081115 in
    //    ival = trk_10(0, 0, scanTableGetOffCoord(), p->celObj.ibjflg, &p->seq.off[p->curr.offNo*2], 0);//081115 out
    */
  }
  else{
    // 090529 in, Use penguin tracking
    glacier_CelestialObjectSetSource(celestialObject, celestialObjectGetSourceFlag());
    const double* xy = celestialObjectGetXY();
    glacier_CelestialObjectSetCoordinate(celestialObject, celestialObjectGetCoordinate(), xy[0], xy[1]);
    glacier_CelestialObjectSetEpoch(celestialObject, celestialObjectGetEpoch());

    /* 090529 out
    //! 通常の天体を観測中の場合
    //uM2("calcTrkCalc(); %d %d", celestialObjectGetSourceFlag(), celestialObjectGetCoordinate());
    //uM3("calcTrkCalc(); %d %d %d", celestialObjectGetEpoch(), celestialObjectGetXY(), celestialObjectGetVelocity());
    ival = trk_10(celestialObjectGetSourceFlag(), 0, celestialObjectGetCoordinate(), celestialObjectGetEpoch(), celestialObjectGetXY(), celestialObjectGetVelocity());//081115 in
    //    ival = trk_10(p->celObj.ioflg, 0, p->celObj.iaoflg, p->celObj.ibjflg, p->celObj.dsourc, p->celObj.drdlv);//081115 out
    */
  }

  /* 090529 out
     if (ival == 1) {
     //uM("trk_10: Argument check error");
     return TRK_TRK10_ARG_ERR;
     }
     else if (ival == 2){
     //uM("trk_10: Common area initial error");
     return TRK_TRK10_COMAREA_ERR;
     }
  */


  // 090529 in, Use penguin tracking
  penguin_tracking_setObject(celestialObject);

  penguin_tracking_setScanOffset(pCalc.scanOffset);
  penguin_tracking_setPointingOffset(pCalc.pointingOffset);//090604 in

  // 090529 in, Use penguin tracking
  glacier_TrackingInformationClass_t* information = penguin_tracking_work();

  glacier_CelestialObjectEnd(celestialObject);//090604in
  /* 090529 out
  //! trk_20 (観測該当時刻の設定 YYYYMMDDhhmmss.0 (JST))
  if(tjcd2m(pCalc.cstart, &dtime) == 1){  //!< JST時系の暦日付からMJDへの変換
  //uM("tjcd2m: Error");
  return TRK_TJCD2M_ERR;
  }
  ival = trk_20(dtime, pCalc.xin, &pCalc.xout);
  trk_ant_(pCalc.dgtdat[0], pCalc.dgtdat[1], pCalc.dgtdat[2], pCalc.dgtdat[3], pCalc.dgtdat[4], pCalc.dgtdat[5], pCalc.dgtdat[6], pCalc.dgtdat[7], &dgtloc);
  */


  //! 天体座標、LSTを取得 時分秒を秒換算したもの[sec]
  //	pCalc.lst = dgtloc * (24.0 * 60.0 * 60.0 / (2.0 * PI));//090529 out
  if(information == NULL){
    return TRK_TRK20_LOCAL_ERR;
  }
  /* 090529 out
     if(ival == -1){
     //uM("trk_20: Argument check error");
     pCalc.doazel2[0] = pCalc.xout.dazel[0] * 180.0 / PI;
     pCalc.doazel2[1] = pCalc.xout.dazel[1] * 180.0 / PI;
     return TRK_TRK20_ARG_ERR;
     }
     else if (ival == -2) {
     //uM("trk_20: Local error end");
     pCalc.doazel2[0] = pCalc.xout.dazel[0] * 180.0 / PI;
     pCalc.doazel2[1] = pCalc.xout.dazel[1] * 180.0 / PI;
     return TRK_TRK20_LOCAL_ERR;
     }
  */
  dgtloc = glacier_TrackingInformationGetLocalSideralTime(information);
  pCalc.lst = dgtloc * (24.0 * 60.0 * 60.0 / (2.0 * PI));//090529 in

  const double* azel = glacier_TrackingInformationGetAzEl(information);
  //! Elevationが地平線以下になってしまっている場合は追尾不能 
  //uM1("trkCal(); xout.dazel[1] (%lf deg)", pCalc.xout.dazel[1]*180.0/PI);
  if(azel[1]<=0){
    pCalc.doazel2[0] = azel[0] * 180.0 / PI;
    pCalc.doazel2[1] = azel[1] * 180.0 / PI;
    /*
      pCalc.doazel2[0] = pCalc.xout.dazel[0] * 180.0 / PI;
      pCalc.doazel2[1] = pCalc.xout.dazel[1] * 180.0 / PI;
    */
    return TRK_TRK20_EL_ERR;
  }

  diazel[0] = azel[0];
  diazel[1] = azel[1];

  //! trk_30 (大気差補正)
  memcpy(pCalc.dgtdat[0], glacier_TrackingInformationGet0(information), sizeof(double)*2);//090529 in
  memcpy(pCalc.dgtdat[1], glacier_TrackingInformationGet1(information), sizeof(double)*2);//090529 in
  memcpy(pCalc.dgtdat[2], glacier_TrackingInformationGet2(information), sizeof(double)*2);//090529 in
  memcpy(pCalc.dgtdat[3], glacier_TrackingInformationGet3(information), sizeof(double)*2);//090529 in
  memcpy(pCalc.dgtdat[4], glacier_TrackingInformationGet4(information), sizeof(double)*2);//090529 in
  memcpy(pCalc.dgtdat[5], glacier_TrackingInformationGet5(information), sizeof(double)*2);//090529 in
  memcpy(pCalc.dgtdat[6], glacier_TrackingInformationGet6(information), sizeof(double)*2);//090529 in
  memcpy(pCalc.dgtdat[7], glacier_TrackingInformationGet7(information), sizeof(double)*2);//090529 in

  glacier_TrackingInformationEnd(information);//090604 in

  //for debug 100929 in
  //uM3("debug!; pCalc.dweath[0]=%f, pCalc.dweath[1]=%f, pCalc.dweath[2]=%f", pCalc.dweath[0], pCalc.dweath[1], pCalc.dweath[2]);

  ival = trk_30(diazel, pCalc.dweath, pCalc.dgtdat[0], dgtloc, pCalc.doazel, dorade);
  if(ival != 0){
    uM("trk_30: Local error end");
    uM2("(%3.2lf, %3.2lf)", diazel[0], diazel[1]);
    uM3("(%2.2lf, %3.2lf, %3.2lf), %3.2lf, %3.2lf", pCalc.dweath[0], pCalc.dweath[1], pCalc.dweath[2]);
    pCalc.doazel2[0] = azel[0] * 180.0 / PI;
    pCalc.doazel2[1] = azel[1] * 180.0 / PI;
    /*
      pCalc.doazel2[0] = pCalc.xout.dazel[0] * 180.0 / PI;
      pCalc.doazel2[1] = pCalc.xout.dazel[1] * 180.0 / PI;
    */
    return TRK_TRK30_LOCAL_ERR;
  }
  //uM1("trkCal(); doazel[1] %lf", pCalc.doazel[1] * 180.0 / PI);

  //! rad(pCalc.doazel) -> deg(pCalc.doazel2)
  pCalc.doazel2[0] = pCalc.doazel[0] * 180.0/PI;
  pCalc.doazel2[1] = pCalc.doazel[1] * 180.0/PI;

	
  if(pCalc.doazel2[1] < 0){
    return TRK_LIMIT_EL_ERR;
  }


  //! 器差補正
  double daz, del;
  //! optical
  instrumentCalInst(pCalc.doazel2[0], pCalc.doazel2[1], &daz, &del);
  //instrumentCalInst(pCalc.doazel[0], pCalc.doazel[1], &daz, &del);//*** 2011/01/25 in
  /* 0090206 out
     _calcCalInst(pCalc.doazel2[0], pCalc.doazel2[1], &daz, &del);
  */
  //uM2("daz = %f, del = %f", daz, del);
  pCalc.doazel2[0] += daz;
  pCalc.doazel2[1] += del;

  //debug 101001 in
  /*
    pCalc.doazel2[0] = 71.61;
    pCalc.doazel2[1] = 46.92;
    uM2("[before]AZ=%f, EL=%f", pCalc.doazel2[0], pCalc.doazel2[1]);
  */

  //! radio
  instrumentRadioModel(pCalc.doazel2[0], pCalc.doazel2[1], &daz, &del);
  pCalc.doazel2[0] += daz;
  pCalc.doazel2[1] += del;
	


  //debug 101001 in
  //uM4("[rP]AZ=%f, EL=%f, daz=%f, del=%f", pCalc.doazel2[0], pCalc.doazel2[1], daz, del);


  while(pCalc.doazel2[0]>=360.0){
    pCalc.doazel2[0]-=360.0;
  }
  while(pCalc.doazel2[0]<0.0){
    pCalc.doazel2[0]+=360.0;
  }

  /*** 2011/01/26 out ***/
  // 20121023 in
  // 20120928 out
  uM4("(Az, El) = (%lf, %lf), scan offset = (%lf, %lf)", pCalc.doazel2[0], pCalc.doazel2[1], glacier_ScanOffsetGetX(pCalc.scanOffset) * rad2sec, glacier_ScanOffsetGetY(pCalc.scanOffset) * rad2sec);

  //timeElapsed = tmGetLag(timeStart)*1000;//[ms]
  //uM1("after penguin_tracking_setCurrentTime() used %lf ms.", timeElapsed);

  //_calcCalZoneType(pCalc.doazel2[0]);//081117 out
  return 0;
}

/*! ¥fn void calcGetRADEC(double* RADEC)
  ¥brief 現在の観測位置RADECの取得
  ¥param[out] RADEC [rad]
*/
void calcGetRADEC(double* RADEC){
  memcpy(RADEC, pCalc.dgtdat[4], sizeof(double) * 2);
  return;
}

/*! ¥fn void calcGetLB(double* LB)
  ¥brief 現在の観測位置LBの取得
  ¥param[out] LB [rad]
*/
void calcGetLB(double* LB){
  memcpy(LB, pCalc.dgtdat[5] , sizeof(double) * 2);
  return;
}



/*! ¥fn void trkGetAZEL(double* AZEL)
  ¥brief 現在の観測位置AZEL(器差補正無し位置)の取得
  ¥param[out] AZEL [rad]
*/
void calcGetAZEL(double* AZEL){
  memcpy(AZEL, pCalc.doazel, sizeof(double) * 2);
  return;
}



/*! ¥fn void trkGetPAZEL(double* PAZEL)
  ¥brief アンテナ指示角PAZELの取得
  ¥param[out] PAZEL [rad]
*/
void calcGetPAZEL(double* PAZEL){
  memcpy(PAZEL, pCalc.doazel2, sizeof(double)*2);
  return;
}


/*! ¥fn double calcGetDoAz2()
  ¥brief _trkThrdMain threadからのみ呼び出すこと
*/
double calcGetDoAz2(){//thread unsafe ?
  return pCalc.doazel2[0];
}

/*! ¥fn double calcGetDoAz2()
  ¥brief _trkThrdMain threadからのみ呼び出すこと
*/
double calcGetDoEl2(){//thread unsafe ?
  return pCalc.doazel2[1];
}

double calcGetMapCenterDec(){
  return pCalc.dgtdat[1][1];
}
double calcGetAntennaLat(){
  return pCalc.dgtdat[0][1];
}


/*! ¥fn void calcGetDRADEC(double* DRADEC)
  ¥brief スキャンオフセットDRADECの取得
  ¥param[out] DRADEC [rad]
*/
void calcGetDRADEC(double* DRADEC){
  if(glacier_ScanOffsetGetCoordinate(pCalc.scanOffset) == 1){
    DRADEC[0] = glacier_ScanOffsetGetX(pCalc.scanOffset);
    DRADEC[1] = glacier_ScanOffsetGetY(pCalc.scanOffset);
  }else{
    memset(DRADEC, 0, sizeof(double)*2);
  }
  /* 090604 out
     if(pCalc.xin.iasflg==1){
     memcpy(DRADEC, pCalc.xin.dscnst, sizeof(double)*2);
     }
     else{
     memset(DRADEC, 0, sizeof(double)*2);
     }
  */
  return;
}

/*! ¥fn void calcGetDLB(double* DLB)
  ¥brief スキャンオフセットDLBの取得
  ¥param[out] DLB [rad]
*/
void calcGetDLB(double* DLB){
  if(glacier_ScanOffsetGetCoordinate(pCalc.scanOffset) == 2){
    DLB[0] = glacier_ScanOffsetGetX(pCalc.scanOffset);
    DLB[1] = glacier_ScanOffsetGetY(pCalc.scanOffset);
  }else{
    memset(DLB, 0, sizeof(double)*2);
  }
  /*
    if(pCalc.xin.iasflg==2){
    memcpy(DLB, pCalc.xin.dscnst, sizeof(double)*2);
    }
    else{
    memset(DLB, 0, sizeof(double)*2);
    }
  */
  return;
}

/*! ¥fn void calcGetDAZEL(double* DAZEL)
  ¥brief スキャンオフセットDAZELの取得
  ¥param[out] DAZEL [rad]
*/
void calcGetDAZEL(double* DAZEL){
  if(glacier_ScanOffsetGetCoordinate(pCalc.scanOffset) == 3){
    DAZEL[0] = glacier_ScanOffsetGetX(pCalc.scanOffset);
    DAZEL[1] = glacier_ScanOffsetGetY(pCalc.scanOffset);
  }else{
    memset(DAZEL, 0, sizeof(double)*2);
  }
  /*
    if(pCalc.xin.iasflg==3){
    memcpy(DAZEL, pCalc.xin.dscnst, sizeof(double) * 2);
    }
    else{
    memset(DAZEL, 0, sizeof(double)*2);
    }
  */
  return;
}

/*! ¥fn void calcGetScanOff(int* coord, double* sof)
  ¥brief スキャンオフセットの現在値取得
  ¥param[out] coord 1:RADEC 2:LB 3:AZEL
  ¥param[out] sof スキャンオフセットの値 [sec]
*/
void calcGetScanOff(int* coord, double* sof){
  *coord = glacier_ScanOffsetGetCoordinate(pCalc.scanOffset);//090529 in
  /* 090529 out
   *coord = pCalc.xin.iasflg;//090206 in
   */
  /* 090206 out
   *coord = pCalc.scanOffsetCoord;
   */
  sof[0] = glacier_ScanOffsetGetX(pCalc.scanOffset) * rad2sec;
  sof[1] = glacier_ScanOffsetGetY(pCalc.scanOffset) * rad2sec;
  /* 090529 out
     memcpy(sof, pCalc.xin.dscnst, sizeof(double) * 2);
     //memcpy(sof, p.sof, sizeof(double) * 2);
     sof[0] *= rad2sec;
     sof[1] *= rad2sec;
  */
  return;
}

/*! ¥fn void _trkSetSof(double* sof, int coord, double posAngleRad)
  ¥brief スキャンオフセットの修正
  ¥param[in] sof スキャンオフセットX,Y 2次元配列 [sec]
  ¥param[in] coord スキャンオフセット座標の定義 1:RADEC 2:LB 3:AZEL
*/
void calcSetSof(const double* sof, int coord, double posAngleRad){

  uM4("_trkSetSof(); Set scan offset(%.2lf\",%.2lf\") coord=%d(%s)", sof[0], sof[1], coord, errnoCoord(coord));
  //uLock();
  if(1 <= coord && coord <= 3){
    /* 090206 out
       pCalc.scanOffsetCoord = coord;
    */
    //p.xin.iasflg = coord;
  }else{
    uM1("CAUTION! _trkSetSof(); Invalid coord: %d\n", coord);
  }
  const double xSoff = sof[0] * sec2rad;
  const double ySoff = sof[1] * sec2rad;
  /* 081117 after the invocation of this method, these values are never read, thus, should not modify them. 
     sof[0] *= sec2rad;
     sof[1] *= sec2rad;
  */
  //memcpy(p.sof, sof, sizeof(p.sof[0]) * 2);//081115 out

  const double cosAng = cos(posAngleRad);//081117 in
  const double sinAng = sin(posAngleRad);
  const double x =   xSoff * cosAng + ySoff * sinAng;
  const double y = - xSoff * sinAng + ySoff * cosAng;



  /*
    x =   sof[0] * cos(posAngleRad) + sof[1] * sin(posAngleRad);
    y = - sof[0] * sin(posAngleRad) + sof[1] * cos(posAngleRad);
  */
  glacier_ScanOffsetSetXY(pCalc.scanOffset, coord, x, y);//090529 in
  /* 090529 out
     calcSetXinDscn(coord, x, y);//090207 in
  */
  /* 090207 out
     calcSetXinDscn(coord, x, y, x, y);//081115 in
  */
  /* 081115 out
     p.xin.dscnst[0] = x;
     p.xin.dscnst[1] = y;
     p.xin.dscned[0] = x;
     p.xin.dscned[1] = y;
  */
  //uUnLock();
  return;
}

/*! ¥fn void calcGetPointOff(int* coord, double* pof)
  ¥brief ポインティングオフセットの現在値取得
  ¥param[out] coord 1:RADEC 2:LB 3:AZEL
  ¥param[out] pof ポインティングオフセットの値 [sec]
*/
void calcGetPointOff(int* coord, double* pof){
  *coord = glacier_ScanOffsetGetCoordinate(pCalc.pointingOffset);//090603 in
  /* 090603 out
   *coord = pCalc.xin.iapflg;
   */

  pof[0] = glacier_ScanOffsetGetX(pCalc.pointingOffset) * rad2sec;//090603 in
  pof[1] = glacier_ScanOffsetGetY(pCalc.pointingOffset) * rad2sec;
  /* 090603 out
     if(pCalc.xin.iapflg == 1 || pCalc.xin.iapflg == 2){
     memcpy(pof, pCalc.xin.dpofst, sizeof(double) * 2);
     }
     else{
     memcpy(pof, pCalc.xin.dpofaz, sizeof(double) * 2);
     }
     pof[0] *= rad2sec;
     pof[1] *= rad2sec;
  */
  return;
}

void calcSetPointOff(int coord, double ddX, double ddY){
  glacier_ScanOffsetSetXY(pCalc.pointingOffset, coord, ddX, ddY);//091110 in
  //	glacier_ScanOffsetSetXY(pCalc.pointingOffset, coord, ddX * sec2rad, ddY * sec2rad);//090604 in
}


/*! ¥fn unsigned int calcGetLST()
  ¥brief LSTの取得
  ¥retval lst 時分秒のみを秒換算したLST [sec]
*/
unsigned int calcGetLST(){
  return pCalc.lst;
  //void trkGetLST(unsigned int* lst){
  //  *lst = p.lst;
  //  return;
}

/*! ¥fn void calcSetXinDscn(int coord, double dscn0, double dscn1)
  ¥brief xin 構造体のsetter. 
  ¥param[in] coord スキャンオフセット座標の定義 1:RADEC 2:LB 3:AZEL
  ¥param[in] dscn0 [rad]
  ¥param[in] dscn1 [rad]
*/

void calcSetXinDscn(int coord, double dscn0, double dscn1){
  //	uM2("Scan offset (%.2lf\",%.2lf\")", dscn0*rad2sec, dscn1*rad2sec);
  glacier_ScanOffsetSetXY(pCalc.scanOffset, coord, dscn0, dscn1);//090604 in
  /* 090604 out
     _calcSetXinDscn(coord, dscn0, dscn1, dscn0, dscn1);
  */
}

/*! ¥fn void _calcSetXinDscn(int coord, double dscnst0, double dscnst1, double dscned0, double dscned1)
  ¥brief xin 構造体のsetter. 
  ¥param[in] coord スキャンオフセット座標の定義 1:RADEC 2:LB 3:AZEL
  ¥param[in] dscnst0 [rad]
  ¥param[in] dscnst1 [rad]
  ¥param[in] dscned0 [rad]
  ¥param[in] dscned1 [rad]
*/
/* 090604 out
   void _calcSetXinDscn(int coord, double dscnst0, double dscnst1, double dscned0, double dscned1){
   pCalc.xin.iasflg = coord;
   pCalc.xin.dscnst[0] = dscnst0;
   pCalc.xin.dscnst[1] = dscnst1;
   pCalc.xin.dscned[0] = dscned0;
   pCalc.xin.dscned[1] = dscned1;
   }
*/

/*! ¥fn void calcSetXinDpof(int coord, double dpofst0, double dpofst1, double dpofaz0, double dpofaz1)
  ¥brief xin 構造体のsetter. 
  ¥param[in] coord ポインティングオフセット座標の定義 1:RADEC 2:LB 3:AZEL
  ¥param[in] dscnst0 [rad]
  ¥param[in] dscnst1 [rad]
  ¥param[in] dscned0 [rad]
  ¥param[in] dscned1 [rad]
*/
/* 090604 out
   void calcSetXinDpof(int coord, double dpofst0, double dpofst1, double dpofaz0, double dpofaz1){
   pCalc.xin.iapflg = coord;
   pCalc.xin.dpofst[0] = dpofst0;
   pCalc.xin.dpofst[1] = dpofst1;
   pCalc.xin.dpofaz[0] = dpofaz0;
   pCalc.xin.dpofaz[1] = dpofaz1;
   }
*/

/*! ¥fn void _calcSetXinIFlags(int irpflg, int iapflg, int irsflg, int iasflg)
  ¥brief xin 構造体のsetter. 
  ¥param[in] irpflg ポインティング 0:虚角 1:実角
  ¥param[in] iapflg ポインティング座標定義初期値 1:RADEC 2:LB 3:AZEL
  ¥param[in] irsflg スキャンニング 0:虚角 1:実角
  ¥param[in] iasflg スキャンニング座標定義初期値 1:RADEC 2:LB 3:AZEL
*/
/* 090604 out
   void _calcSetXinIFlags(int irpflg, int iapflg, int irsflg, int iasflg){
   pCalc.xin.irpflg = irpflg;
   pCalc.xin.iapflg = iapflg;
   pCalc.xin.irsflg = irsflg;
   pCalc.xin.iasflg = iasflg;
   }
*/

/*! ¥fn void _calcSetXinIScn(int itnscn, int inoscn)
  ¥brief xin 構造体のsetter. 
  ¥param[in] itnscn スキャンニング点数
  ¥param[in] inoscn スキャンニング点
*/
/* 090604 out
   void _calcSetXinIScn(int itnscn, int inoscn){
   pCalc.xin.itnscn = itnscn;
   pCalc.xin.inoscn = inoscn;
   }
*/

/*! ¥fn void trkCalInst(double X, double Y, double* dx, double* dy)
  ¥brief 器差補正を行うための関数。
  ¥brief 一時的につくば32m鏡のS/X帯でのパラメーターを使用
  ¥param[in] X  raw Az deg
  ¥param[in] Y  raw El deg
  ¥param[out] dX (add to raw Az) deg
  ¥param[out] dY (add to raw El) deg
*/
/* 0090206 moved to instrument.cpp
   void _calcCalInst(double X, double Y, double* dX, double* dY){
   double Phi = 90.0 * PI / 180.0;
   //double P[31] = {0};

   //memset(P, 0, 31*sizeof(P[0]));
   //! deg -> rad
   X *= PI / 180.0;
   Y *= PI / 180.0;

   const double cosX = cos(X);
   const double sinX = sin(X);
   const double cos2X = cos(2*X);
   const double sin2X = sin(2*X);
   const double tanY = tan(Y);
   const double cosY = cos(Y);
   const double sinY = sin(Y);
   *dX=pCalc.P[0] - pCalc.P[1]*cos(Phi)*tanY + pCalc.P[2]*tanY - pCalc.P[3]/cosY
   + pCalc.P[4]*sinX*tanY - pCalc.P[5]*cosX*tanY
   + pCalc.P[11]*X + pCalc.P[12]*cosX + pCalc.P[13]*sinX + pCalc.P[16]*cos2X + pCalc.P[17]*sin2X;
   *dY=  pCalc.P[4]*cosX + pCalc.P[5]*sinX
   + pCalc.P[6] - pCalc.P[7]*(cos(Phi)*sinY*cosX-sin(Phi)*cosY) + pCalc.P[8]*Y
   + pCalc.P[9]*cosY + pCalc.P[10]*sinY + pCalc.P[14]*cos2X + pCalc.P[15]*sin2X
   + pCalc.P[18]*cos(8*Y) + pCalc.P[19]*sin(8*Y) + pCalc.P[20]*cosX + pCalc.P[21]*sinX;
*/
/*
// *dX=P[1] + P[3]*tan(Y) - P[4]/cos(Y) + P[5]*sin(X)*tan(Y) - P[6]*cos(X)*tan(Y);
*dX=pCalc.P[0] - pCalc.P[1]*cos(Phi)*tan(Y) + pCalc.P[2]*tan(Y) - pCalc.P[3]/cos(Y)
+ pCalc.P[4]*sin(X)*tan(Y) - pCalc.P[5]*cos(X)*tan(Y)
+ pCalc.P[11]*X + pCalc.P[12]*cos(X) + pCalc.P[13]*sin(X) + pCalc.P[16]*cos(2*X) + pCalc.P[17]*sin(2*X);
  
// *dY=P[5]*cos(X) + P[6]*sin(X) + P[7] - P[8]*cos(Y);
*dY=  pCalc.P[4]*cos(X) + pCalc.P[5]*sin(X)
+ pCalc.P[6] - pCalc.P[7]*(cos(Phi)*sin(Y)*cos(X)-sin(Phi)*cos(Y)) + pCalc.P[8]*Y
+ pCalc.P[9]*cos(Y) + pCalc.P[10]*sin(Y) + pCalc.P[14]*cos(2*X) + pCalc.P[15]*sin(2*X)
+ pCalc.P[18]*cos(8*Y) + pCalc.P[19]*sin(8*Y) + pCalc.P[20]*cos(X) + pCalc.P[21]*sin(X);
*/
//! debug
//double dx = *dX * 3600.0;
//double dy = *dY * 3600.0;
//printf("debug trkCalInst(); dAz dEl = %lf %lf¥n", *dX, *dY);
/* 0090206 moved to instrument.cpp
   return;
   }

*/


/*! ¥fn void calcSetWeath(double* weath)
  ¥brief 天候情報の設定
*/
void calcSetWeath(const double* weath){ 
  pCalc.dweath[0] = weath[0];    //!< 気温 [K]
  pCalc.dweath[1] = weath[1];    //!< 気圧 [hPa]
  pCalc.dweath[2] = weath[2];    //!< 水蒸気圧 [hPa]

  pCalc.dweath[0] -= ABS_ZERO;   //!< 気温[℃]
  return;
}


/*! ¥fn void setParam()
  ¥brief パラメーター情報を取得する
  * thread 1, phase C0
  */
void setParam(){

  if(confSetKey("TrkTimeOffset"))
    pCalc.TrkTimeOffset = atof(confGetVal());

  /* 0090206 moved to instrument.cpp
     char tmp[16];
     const char *antInst[7];

     memset(antInst, 0, sizeof(antInst[0]) * 7);
     for(int i = 0; i < 7; i++){
     sprintf(tmp, "AntInst%d", i);
     if(confSetKey(tmp))
     antInst[i] = confGetVal();
     }
     for(int i = 0; i < 6; i++){
     int j = i * 5;
     if(sscanf(antInst[i+1], "%lf %lf %lf %lf %lf",
     &pCalc.P[j], &pCalc.P[j+1], &pCalc.P[j+2], &pCalc.P[j+3], &pCalc.P[j+4]) != 5){
     uM2("setParam(); antInst[%d] (%s) error!!", i+1, antInst[i+1]);
     }
     }
  */
  /* 090604 out
     _calcSetXinIFlags(1, 3, 1, 1);//081115 in
  */
  /* 081115 out
     if(1)
     p.xin.irpflg = 1;              //!< ポインティング 0:虚角 1:実角
     if(1)
     p.xin.iapflg = 3;              //!< ポインティング座標定義初期値 1:RADEC 2:LB 3:AZEL
     if(1)
     p.xin.irsflg = 1;              //!< スキャンニング 0:虚角 1:実角
     if(1)
     p.xin.iasflg = 1;              //!< スキャンニング座標定義初期値 1:RADEC 2:LB 3:AZEL
  */
  /* 090604 out
     _calcSetXinIScn(1 ,1);//081115 in
  */
  /* 081115 out
     if(1)
     p.xin.itnscn = 1;              //!< スキャンニング点数
     if(1)
     p.xin.inoscn = 1;              //!< スキャンニング点
  */


  //////////// parameters from calc2.cpp ///////////////////
  if(confSetKey("Definition"))
    pCalc.sourceDefinition = atoi(confGetVal());
  if(confSetKey("Frame"))
    pCalc.sourceFrame = atoi(confGetVal());
  if(confSetKey("Ftrack"))
    pCalc.trackingFrequency = atof(confGetVal()) * 1.0e9; //!< 静止トラッキング周波数 [Hz]

}

/*! ¥fn int checkParam()
  ¥brief パラメーターのチェック
  ¥retval 0 成功
  ¥retval -1 失敗
*/
int checkParam(){
  return 0;
}



//////////////////////////////////////////// from calc2.cpp ////////////////////////////////////////////////////

/*! ¥fn void calc2GetVRAD(double* VRAD)
  ¥brief VRADを取得する
  ¥param[out] VRAD [km/s]
*/
double calc2GetVRAD(){
  return pCalc.dvrad / 1.0e3; //!< km/s
}

/*! ¥fn int _ifCal()
  ¥brief ドップラー追尾の周波数計算を行う
  ¥retval 0 成功
  ¥retval 0以外 エラーコード
*/
int calc2IfCal(){
  int res = penguin_tracking_setCurrentTime(pCalc.TrkTimeOffset);
  if(res){
    uM1("penguin_tracking_setCurrentTime() returns %d.", res);
  }

  glacier_CelestialObjectClass_t* celestialObject = glacier_CelestialObjectInit();
  glacier_CelestialObjectSetSource(celestialObject, celestialObjectGetSourceFlag());
  const double* xy = celestialObjectGetXY();
  glacier_CelestialObjectSetCoordinate(celestialObject, celestialObjectGetCoordinate(), xy[0], xy[1]);
  glacier_CelestialObjectSetEpoch(celestialObject, celestialObjectGetEpoch());

  glacier_CelestialObjectSetVelocity(celestialObject, celestialObjectGetVelocity());
  glacier_CelestialObjectSetFrame(celestialObject, pCalc.sourceFrame);
  glacier_CelestialObjectSetDefinition(celestialObject, pCalc.sourceDefinition);

  penguin_tracking_setObjectForVelocity(celestialObject);
  penguin_tracking_setRestFrequencyForVelocity(pCalc.trackingFrequency);

  /* no need.
     penguin_tracking_setScanOffset(pCalc.scanOffset);
     penguin_tracking_setPointingOffset(pCalc.pointingOffset);
  */

  pCalc.dvrad = penguin_tracking_workForVelocity();
  pCalc.dopplerShiftedFrequency = penguin_tracking_getDopplerFrequency();

  glacier_CelestialObjectEnd(celestialObject);//090604in
  /*
  //! DOPPLER SHIFTの取得
  //! trk_20用変数
  double dtime;

  //! 作業用変数
  char cerr[256];
  int  ival;


  //! UTにおいて日にちが変わっていたらtrk_00を実行する
  tmReset(pCalc.vtm);
  strcpy(pCalc.cstart, tmGetTimeStr(pCalc.vtm, pCalc.trkTimeOffset));
  if(pCalc.cstart[8] == '0' && pCalc.cstart[9] == '9'){           //!< JST時刻との比較
  if(pCalc.dayFlg == 0){
  uM1("_ifCal(); trk_00(\"%s\")", pCalc.cstart);
  if(trk_00(pCalc.cstart, "", cerr) != 0) {
  uM1("_ifCal(); %s",cerr);
  return IF_TRK00_ERR;
  }
  pCalc.dayFlg = 1;
  }
  }
  else{
  pCalc.dayFlg = 0;
  }

  //! trk_10
  //! 第２引数は処理フラグ 3:DOPPLER
  ival = trk_10(celestialObjectGetSourceFlag(), 3, celestialObjectGetCoordinate(), celestialObjectGetEpoch(), celestialObjectGetXY(), celestialObjectGetVelocity());
  if (ival == 1) {
  uE("trk_10: Argument check error");
  return IF_TRK10_ARG_ERR;
  } else if (ival == 2) {
  uE("trk_10: Common area initial error");
  return IF_TRK10_COMAREA_ERR;
  }

  if (tjcd2m(pCalc.cstart, &dtime) == 1) {  //!< JST時系の暦日付からMJDへの変換
  uM("tjcd2m: Error");
  return IF_TJCD2M_ERR;
  }
  ival = trk_20(dtime, pCalc.xin, &pCalc.xout);
  if (ival == -1) {
  uM("trk_20: Argument check error");
  return IF_TRK20_ARG_ERR;
  } else if (ival == -2) {
  uM("trk_20: Local error end");
  return IF_TRK20_LOCAL_ERR;
  }

  //! VRAD (アンテナから見た視線速度) の算出
  trk_velo_(&pCalc.dvrad);

  */
  return 0;
}

void calc2PrintResults(){
  uM3("ifUpdate(); Fobs %+10.9e Hz,  Fdop %+10.9e Hz,  Vrad %+10.9lf m/s", pCalc.trackingFrequency, pCalc.dopplerShiftedFrequency, pCalc.dvrad);
  /*
    uM3("ifUpdate(); Fobs %+10.9e Hz,  Fdop %+10.9e Hz,  Vrad %+10.9lf m/s", pCalc.xin.dobsfq, pCalc.xout.dflkfq, pCalc.dvrad);
    uM1("ifUpdate(); DateTime %s", pCalc.cstart);
  */
}

double calc2GetXoutDflkfq(){
  return pCalc.dopplerShiftedFrequency;
  //	return pCalc.xout.dflkfq;
}


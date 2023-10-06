/*!
¥file acu.cpp
¥author NAGAI Makoto
¥date 2008.11.14
¥brief Tracking Calculation Program for 32-m telescope & 30-cm telescope
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
#include "calc.h"

//#define PI  3.14159265358979323846
#define PI M_PI


typedef struct {
	double dweath[3];     //!< 気温 [℃], 気圧 [hPa] 水蒸気圧 [hPa]
	int isFinishedTrk_00;	//!< trk_00()を呼び出すタイミング用。UTにおける日付が変わったとき呼ぶ。
	XTRKI xin;            //!< 45m追尾ライブラリ入力パラメーター構造体
	XTRKO xout;           //!< 45m追尾ライブラリ計算結果出力用構造体
	int scanOffsetCoord;	//!< 現在のスキャンオフセットの座標系 1:RADEC 2:LB 3:AZEL
	double P[30];         //!< アンテナ器差補正パラメータ used only in trkCalInst()
	double doazel[2];     //!< 計算結果AZEL [rad]
	double doazel2[2];    //!< 計算結果に器差補正を加えた値 [deg]
	double dgtdat[8][2];  //!< MapCenter(RADEC)(LB) Obs(RADEC)(LB) [rad] 
	char cstart[24];      //!< 観測時刻の設定 YYYYMMDDhhmmss.0 (JST)
	double TrkTimeOffset; //!< 追尾計算を行うときに何秒後の計算を行うかセットする 単位sec
	unsigned int lst;     //!< LST時分秒のみを秒換算したもの
	tmClass_t* vtm;		//!< trk_00()更新時間管理用
}tParamCalc;


//static tParamTrk *p;
static tParamCalc pCalc;

const double ABS_ZERO = 273.15;
//const int mapNumMax = 20;
const double sec2rad = PI / (180.0 * 3600.0);
const double rad2sec = 180.0 * 3600.0 / PI;

/*enum eAcuCmd {STX=0x02, ETX=0x03};
enum eAcuS1 {EL_DRIVE_DISABLE=0x38, AZ_DRIVE_DISABLE=0x34, EL_DRIVE_ENABLE=0x32, AZ_DRIVE_ENABLE=0x31};
enum eAcuS2 {TRACK_MODE_FAULT=0x38, STANDBY=0x34, PROGRAM_TRACK_MODE=0x32};
enum eAcuS3 {REMOTE_CONTROL_MODE=0x38, LOCAL_CONTROL_MODE=0x34, STOW=0x32};
enum eAcuS4 {TOTAL_ALARM=0x34};
enum eAcuS5 {ANGLE_FAULT=0x34, AZ_DCPA_FAULT=0x32, EL_DCPA_FAULT=0x31};
*/


static void _calcCalInst(double X, double Y, double* dX, double* dY);
//static void _calcCalZoneType(double az);//081117 moved to trk.cpp

static void _calcSetXinIFlags(int irpflg, int iapflg, int irsflg, int iasflg);
static void _calcSetXinIScn(int itnscn, int inoscn);


static void setParam();

/*
Invoked by trkInit()
Thus, this function is excuted once for each tkb32 run. 
*/
int calcInit(){
	//p = param;

	pCalc.vtm = tmInit();//081117 in
	pCalc.isFinishedTrk_00 = 0;

	return 0;
}

/*
Invoked by trkEnd()
Thus, this function is excuted once for each tkb32 run. 
*/
void calcEnd(){
	if(pCalc.vtm){
		tmEnd(pCalc.vtm);
		pCalc.vtm = NULL;
	}
}

/*
* Invoked by trkInitParam()
* Thus, this function is excuted once for each initialization of observation tables. 
*/
int calcInitParam(){

	memset(&pCalc.xout, 0, sizeof(pCalc.xout));
	memset(pCalc.P, 0, sizeof(pCalc.P[0]) * 30);
	memset(pCalc.doazel, 0, sizeof(pCalc.doazel[0]) * 2);
	memset(pCalc.doazel2, 0, sizeof(pCalc.doazel2[0]) * 2);
	memset(pCalc.cstart, 0, sizeof(pCalc.cstart[0]) * 24);
	memset(&pCalc.xin, 0, sizeof(pCalc.xin));
	for(int i = 0; i < 8; i++){
		memset(pCalc.dgtdat[i], 0, sizeof(pCalc.dgtdat[0][0]) * 2);
	}
	
	pCalc.scanOffsetCoord = 0;
	pCalc.TrkTimeOffset = 0;

	setParam();

	//! Initialize trk45 library
	tmReset(pCalc.vtm);
	strcpy(pCalc.cstart, tmGetTimeStr(pCalc.vtm, pCalc.TrkTimeOffset));
	char cerr[256];
	if(trk_00(pCalc.cstart, "", cerr) != 0) {
		uM2("trkInitParam(); trk_00(%s) %s", pCalc.cstart, cerr);
		return TRK_TRK00_ERR;
	}
	pCalc.lst = 0;
	pCalc.isFinishedTrk_00 = 0;//081117 in force update pCalc.cstart if JST become ****/**/** 09:**
	//pCalc.isFinishedTrk_00 = 1;//081117 out
	return 0;
}

/*! ¥fn int trkCal()
¥brief 追尾計算部
¥retval 0 成功
¥retval マイナス値 失敗
*/
int calcTrkCal(int currOffFlag, int currOffNo){
  char cerr[256];

  //! trk_20用変数
  double dtime;

  //! trk_30用変数
  double diazel[2], dorade[2];
  static double dgtloc;        //!< 瞬間の視恒星時 [rad]

  //! 作業用変数
  int ival;
  //double daz, del;

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
      pCalc.isFinishedTrk_00 = 1;//do not update pCalc.cstart even if JST is ****/**/** 09:**
    }
  }
  else{
    pCalc.isFinishedTrk_00 = 0;//force update pCalc.cstart if JST become ****/**/** 09:**
  }

  //! trk_10
  //! 第２引数は処理フラグ 0:AZEL
  //! uM2("debug trkCal(); sourc=(%lf,%lf)", p->celObj.dsourc[0], p->celObj.dsourc[1]);
	if(scanTableGetOffMode() == 2 && currOffFlag == 1){//081117 in
//  if(scanTableGetOffMode() == 2 && p->curr.offFlg == 1){
    //! 絶対座標指定のOFF点を観測中の場合。0:太陽系外 0:AZEL :OffCoord ibjflg:On点と同じ元期 :座標 0:速度 
    ival = trk_10(0, 0, scanTableGetOffCoord(), celestialObjectGetEpoch(), scanTableGetOff(currOffNo*2), 0);//081117 in
//    ival = trk_10(0, 0, scanTableGetOffCoord(), celestialObjectGetEpoch(), &p->seq.off[p->curr.offNo*2], 0);//081115 in
//    ival = trk_10(0, 0, scanTableGetOffCoord(), p->celObj.ibjflg, &p->seq.off[p->curr.offNo*2], 0);//081115 out
  }
  else{
    //! 通常の天体を観測中の場合
    ival = trk_10(celestialObjectGetSourceFlag(), 0, celestialObjectGetCoordinate(), celestialObjectGetEpoch(), celestialObjectGetXY(), celestialObjectGetVelocity());//081115 in
//    ival = trk_10(p->celObj.ioflg, 0, p->celObj.iaoflg, p->celObj.ibjflg, p->celObj.dsourc, p->celObj.drdlv);//081115 out
  }

  if (ival == 1) {
    //uM("trk_10: Argument check error");
    return TRK_TRK10_ARG_ERR;
  }
  else if (ival == 2){
    //uM("trk_10: Common area initial error");
    return TRK_TRK10_COMAREA_ERR;
  }
  
  //! trk_20 (観測該当時刻の設定 YYYYMMDDhhmmss.0 (JST))
  if(tjcd2m(pCalc.cstart, &dtime) == 1){  //!< JST時系の暦日付からMJDへの変換
    //uM("tjcd2m: Error");
    return TRK_TJCD2M_ERR;
  }
  ival = trk_20(dtime, pCalc.xin, &pCalc.xout);
  trk_ant_(pCalc.dgtdat[0], pCalc.dgtdat[1], pCalc.dgtdat[2], pCalc.dgtdat[3], pCalc.dgtdat[4],
	   pCalc.dgtdat[5], pCalc.dgtdat[6], pCalc.dgtdat[7], &dgtloc);
  //! 天体座標、LSTを取得 時分秒を秒換算したもの[sec]
  pCalc.lst = dgtloc * (24.0 * 60.0 * 60.0 / (2.0 * PI));

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
  //! Elevationが地平線以下になってしまっている場合は追尾不能 
  //uM1("trkCal(); xout.dazel[1] (%lf deg)", pCalc.xout.dazel[1]*180.0/PI);
  if(pCalc.xout.dazel[1]<=0){
    pCalc.doazel2[0] = pCalc.xout.dazel[0] * 180.0 / PI;
    pCalc.doazel2[1] = pCalc.xout.dazel[1] * 180.0 / PI;
    return TRK_TRK20_EL_ERR;
  }
    
  diazel[0] = pCalc.xout.dazel[0];
  diazel[1] = pCalc.xout.dazel[1];

  //! trk_30 (大気差補正)
  ival = trk_30(diazel, pCalc.dweath, pCalc.dgtdat[0], dgtloc, pCalc.doazel, dorade);
  if(ival != 0){
    //! uM("trk_30: Local error end");
    pCalc.doazel2[0] = pCalc.xout.dazel[0] * 180.0 / PI;
    pCalc.doazel2[1] = pCalc.xout.dazel[1] * 180.0 / PI;
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
  _calcCalInst(pCalc.doazel2[0], pCalc.doazel2[1], &daz, &del);
  pCalc.doazel2[0] += daz;
  pCalc.doazel2[1] += del;

  while(pCalc.doazel2[0]>=360.0){
    pCalc.doazel2[0]-=360.0;
  }
  while(pCalc.doazel2[0]<0.0){
    pCalc.doazel2[0]+=360.0;
  }
  
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
  if(pCalc.xin.iasflg==1){
    memcpy(DRADEC, pCalc.xin.dscnst, sizeof(double)*2);
  }
  else{
    memset(DRADEC, 0, sizeof(double)*2);
  }
  return;
}

/*! ¥fn void calcGetDLB(double* DLB)
¥brief スキャンオフセットDLBの取得
¥param[out] DLB [rad]
*/
void calcGetDLB(double* DLB){
  if(pCalc.xin.iasflg==2){
    memcpy(DLB, pCalc.xin.dscnst, sizeof(double)*2);
  }
  else{
    memset(DLB, 0, sizeof(double)*2);
  }
  return;
}

/*! ¥fn void calcGetDAZEL(double* DAZEL)
¥brief スキャンオフセットDAZELの取得
¥param[out] DAZEL [rad]
*/
void calcGetDAZEL(double* DAZEL){
  if(pCalc.xin.iasflg==3){
    memcpy(DAZEL, pCalc.xin.dscnst, sizeof(double) * 2);
  }
  else{
    memset(DAZEL, 0, sizeof(double)*2);
  }
  return;
}

/*! ¥fn void calcGetScanOff(int* coord, double* sof)
¥brief スキャンオフセットの現在値取得
¥param[out] coord 1:RADEC 2:LB 3:AZEL
¥param[out] sof スキャンオフセットの値 [sec]
*/
void calcGetScanOff(int* coord, double* sof){
  *coord = pCalc.scanOffsetCoord;
  memcpy(sof, pCalc.xin.dscnst, sizeof(double) * 2);
  //memcpy(sof, p.sof, sizeof(double) * 2);
  sof[0] *= rad2sec;
  sof[1] *= rad2sec;
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
		pCalc.scanOffsetCoord = coord;
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
	calcSetXinDscn(coord, x, y, x, y);//081115 in
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
  *coord = pCalc.xin.iapflg;
  if(pCalc.xin.iapflg == 1 || pCalc.xin.iapflg == 2){
    memcpy(pof, pCalc.xin.dpofst, sizeof(double) * 2);
  }
  else{
    memcpy(pof, pCalc.xin.dpofaz, sizeof(double) * 2);
  }
  pof[0] *= rad2sec;
  pof[1] *= rad2sec;
  return;
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

/*! ¥fn void calcSetXinDscn(int coord, double dscnst0, double dscnst1, double dscned0, double dscned1)
¥brief xin 構造体のsetter. 
¥param[in] coord スキャンオフセット座標の定義 1:RADEC 2:LB 3:AZEL
¥param[in] dscnst0 [rad]
¥param[in] dscnst1 [rad]
¥param[in] dscned0 [rad]
¥param[in] dscned1 [rad]
*/
void calcSetXinDscn(int coord, double dscnst0, double dscnst1, double dscned0, double dscned1){
	pCalc.xin.iasflg = coord;
	pCalc.xin.dscnst[0] = dscnst0;
	pCalc.xin.dscnst[1] = dscnst1;
	pCalc.xin.dscned[0] = dscned0;
	pCalc.xin.dscned[1] = dscned1;
}

/*! ¥fn void calcSetXinDpof(int coord, double dpofst0, double dpofst1, double dpofaz0, double dpofaz1)
¥brief xin 構造体のsetter. 
¥param[in] coord ポインティングオフセット座標の定義 1:RADEC 2:LB 3:AZEL
¥param[in] dscnst0 [rad]
¥param[in] dscnst1 [rad]
¥param[in] dscned0 [rad]
¥param[in] dscned1 [rad]
*/
void calcSetXinDpof(int coord, double dpofst0, double dpofst1, double dpofaz0, double dpofaz1){
	pCalc.xin.iapflg = coord;
	pCalc.xin.dpofst[0] = dpofst0;
	pCalc.xin.dpofst[1] = dpofst1;
	pCalc.xin.dpofaz[0] = dpofaz0;
	pCalc.xin.dpofaz[1] = dpofaz1;
}

/*! ¥fn void _calcSetXinIFlags(int irpflg, int iapflg, int irsflg, int iasflg)
¥brief xin 構造体のsetter. 
¥param[in] irpflg ポインティング 0:虚角 1:実角
¥param[in] iapflg ポインティング座標定義初期値 1:RADEC 2:LB 3:AZEL
¥param[in] irsflg スキャンニング 0:虚角 1:実角
¥param[in] iasflg スキャンニング座標定義初期値 1:RADEC 2:LB 3:AZEL
*/
void _calcSetXinIFlags(int irpflg, int iapflg, int irsflg, int iasflg){
	pCalc.xin.irpflg = irpflg;
	pCalc.xin.iapflg = iapflg;
	pCalc.xin.irsflg = irsflg;
	pCalc.xin.iasflg = iasflg;
}

/*! ¥fn void _calcSetXinIScn(int itnscn, int inoscn)
¥brief xin 構造体のsetter. 
¥param[in] itnscn スキャンニング点数
¥param[in] inoscn スキャンニング点
*/
void _calcSetXinIScn(int itnscn, int inoscn){
	pCalc.xin.itnscn = itnscn;
	pCalc.xin.inoscn = inoscn;
}


/*! ¥fn void trkCalInst(double X, double Y, double* dx, double* dy)
¥brief 器差補正を行うための関数。
¥brief 一時的につくば32m鏡のS/X帯でのパラメーターを使用
¥param[in] X  raw Az deg
¥param[in] Y  raw El deg
¥param[out] dX (add to raw Az) deg
¥param[out] dY (add to raw El) deg
*/
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
  return;
}




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
*/
void setParam(){

	if(confSetKey("TrkTimeOffset"))
	pCalc.TrkTimeOffset = atof(confGetVal());

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

	_calcSetXinIFlags(1, 3, 1, 1);//081115 in
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
	_calcSetXinIScn(1 ,1);//081115 in
/* 081115 out
  if(1)
    p.xin.itnscn = 1;              //!< スキャンニング点数
  if(1)
    p.xin.inoscn = 1;              //!< スキャンニング点
*/

}

/*! ¥fn int checkParam()
¥brief パラメーターのチェック
¥retval 0 成功
¥retval -1 失敗
*/
int checkParam(){
  return 0;
}

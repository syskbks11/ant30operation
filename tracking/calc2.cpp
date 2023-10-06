/*!
\file calc2.cpp
\author NAGAI Makoto
\date 2008.11.15
\brief Manages calculation of frequency tracking for 32-m telescope & 30-cm telescope
*/

#include <stdlib.h>
#include <string.h>

#include "configuration.h"
#include "celestialObject.h"
#include "trk45Sub.h"
#include "errno.h"
#include "ifCalc2.h"
#include "calc2.h"

typedef struct {
	tmClass_t* vtm;            //!< 更新時間の管理
	double trkTimeOffset; //!< 何秒後の追尾計算を行うか [sec]
	XTRKI xin;            //!< オフセット設定
	XTRKO xout;           //!< アンテナ速度などDAZEL
	char cstart[24];      //!< 観測時刻の設定 YYYYMMDDhhmmss.0 (JST)
	int dayFlg;           //!< trk_00()を呼び出すタイミング用。UTにおける日付が変わったとき呼ぶ。
	double dvrad;         //!< trk_velo用変数
}calc2_t;

static calc2_t pCalc;
//static tParamIf *p;


/*! ¥fn void calc2GetVRAD(double* VRAD)
¥brief VRADを取得する
¥param[out] VRAD [km/s]
*/
double calc2GetVRAD(){
	return pCalc.dvrad / 1.0e3; //!< km/s
}

/**
 * thread 1, phase A0
 */
int calc2Init(){
	//p = param;
	pCalc.vtm = tmInit();

	return 0;
}

/**
 * thread 1, phase C0
 */
int calc2Start(){

	// zero fill
	pCalc.trkTimeOffset = 0;
	memset(pCalc.cstart, 0, sizeof(pCalc.cstart[0]) * 24);
	memset(&pCalc.xin, 0, sizeof(pCalc.xin));
	memset(&pCalc.xout, 0, sizeof(pCalc.xout));

	// settings
  if(1)
    pCalc.xin.irpflg = 1;                       //!< ポインティング 0:虚角 1:実角
  if(1)
    pCalc.xin.iapflg = 3;                       //!< ポインティング座標定義初期値 1:RADEC 2:LB 3:AZEL
  if(1)
    pCalc.xin.irsflg = 1;                       //!< スキャンニング 0:虚角 1:実角
  if(1){
    /*! スキャンニングの座標定義の初期値はRADECを使用する
     *  if.cではスキャニングの座標定義は初期値をずっと用いる
     */
    pCalc.xin.iasflg = 1;
  }
  if(1)
    pCalc.xin.itnscn = 1;                       //!< スキャンニング点数
  if(1)
    pCalc.xin.inoscn = 1;                       //!< スキャンニング点
  if(confSetKey("Definition"))
    pCalc.xin.ivdef = atoi(confGetVal());
  if(confSetKey("Frame"))
    pCalc.xin.ivref = atoi(confGetVal());
  if(confSetKey("Ftrack"))
    pCalc.xin.dobsfq = atof(confGetVal()) * 1.0e9; //!< 静止トラッキング周波数 [Hz]
  if(confSetKey("trkTimeOffset"))
    pCalc.trkTimeOffset = atof(confGetVal());

	//check
    if(pCalc.xin.ivdef < 1 || pCalc.xin.ivdef > 2){
      uM1("calc2Init(); Definition(%d) error!!¥n", pCalc.xin.ivdef);
/*
      uM2("IfUse1(%d) Definition(%d) error!!¥n", p->use1, pCalc.xin.ivdef);
*/
      return -1;
    }
    if(pCalc.xin.ivref < 1 || pCalc.xin.ivref > 2){
      uM1("calc2Init(); Frame(%d) error!!¥n", pCalc.xin.ivref);
/*
      uM2("IfUse1(%d) Frame(%d) error!!¥n", p->use1, pCalc.xin.ivref);
*/
      return -1;
    }
    //if(pCalc.xin.dobsfq < || pCalc.xin.dobsfq > )
    //  return -1;

	char cerr[256];

	tmReset(pCalc.vtm);
	strcpy(pCalc.cstart, tmGetTimeStr(pCalc.vtm, pCalc.trkTimeOffset));
	if(trk_00(pCalc.cstart, "", cerr) != 0) {
		uM1("ifStart(); %s",cerr);
		return IF_TRK00_ERR;
	}
	pCalc.dayFlg = 0;
/* 081121 out
	pCalc.dayFlg = 1;
*/
	return 0;
}

/**
 * thread 1, phase A2
 */
void calc2End(){
  if(pCalc.vtm){
    tmEnd(pCalc.vtm);
    pCalc.vtm = NULL;
  }
}

/*! ¥fn int _ifCal()
¥brief ドップラー追尾の周波数計算を行う
¥retval 0 成功
¥retval 0以外 エラーコード
*/
int calc2IfCal(){
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
/* 081121 out
  ival = trk_10(p->ioflg, 3, p->iaoflg, p->ibjflg, p->dsourc, p->drdlv);
*/
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


  return 0;
}

void calc2PrintResults(){
	uM3("ifUpdate(); Fobs %+10.9e Hz,  Fdop %+10.9e Hz,  Vrad %+10.9lf m/s", pCalc.xin.dobsfq, pCalc.xout.dflkfq, pCalc.dvrad);
	uM1("ifUpdate(); DateTime %s", pCalc.cstart);
}

double calc2GetXoutDflkfq(){
	return pCalc.xout.dflkfq;
}


/*!
¥file trk.cpp
¥author Y.Koide
¥date 2006.11.20
¥brief Tracking Program
*
* Changed by NAGAI Makoto,
* For Tsukuba 32-m telescope & Antarctic 30-cm telescope. 
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>
#include <sys/types.h>
#include <pthread.h>

#include"thread.h"


#include "configuration.h"
#include "celestialObject.h"
#include "scanTable.h"
#include "trk45Sub.h"
#include "errno.h"
#include "acu.h"
#include "calcBoth.h"
#include "trkCalc.h"
#include "trk.h"
#include "controlBoard.h"

//#define PI  3.14159265358979323846
#define PI M_PI

typedef struct {
  int sofNo;            //!< scan offsetの何番目を処理しているか
  int offNo;            //!< 何番のOff点を処理しているか 0 or 1
  int offFlg;           //!< 直前にOff点観測をしているか 0:No 1:Yes
}scanCurrenctStatus_t;

//! trkのパラメーター構造体
typedef struct sParamTrk{
  //! 接続パラメータ
/* 090611 out, since we use controlBoard.cpp
  int TrkUse;           //!< trkを使用するか 0:使用しない, 1:使用, 2:通信以外使用
*/
  //int TrkCom;           //!< RS-232Cポート 081113 moved to tParamACU in acu.cpp
  //! 変数
  tmClass_t* vtm;            //!< 更新時間管理用
	
  //! 観測パラメータ
  double TrkInterval;   //!< 追尾制御を処理する間隔 sec
	//scanTable_t scan;
  double AntAzElMarg[2];//!< アンテナの許容誤差[deg]
  int AntAzElMargThrsd; //!< この回数、AZELが許容誤差内に連続で収まると追尾と判定する
  //const char* AntInst[7];//!< アンテナ器差補正パラメータ文字列
//  double TrkTimeOffset; //!< 追尾計算を行うときに何秒後の計算を行うかセットする 単位sec
  
  //! 追尾ライブラリ用パラメータ
	//celestialObject_t celObj;
//  int ioflg;            //!< 天体フラグ 0:太陽系外 1-10:惑星 11:太陽 12:COMET 99:AZEL
//  int iaoflg;           //!< 位置座標系 0:惑星 1:RADEC 2:LB 3:AZEL
//  int ibjflg;           //!< 分点フラグ 1:B1950 2:J2000
//  double drdlv;         //!< 天体の視線速度 [m/s]
//  double dsourc[2];     //!< 天体位置 [rad]
  double dazelmin[2];   //!< アンテナの最小駆動限界 [deg]
  double dazelmax[2];   //!< アンテナの最大駆動限界 [deg]

  //! 各種変数
//  XTRKI xin;            //!< 45m追尾ライブラリ入力パラメーター構造体
  //double sof[2];        //!< 現在のスキャンオフセット値(ポジションアングルによる補正無しの値) [rad]
  //int sofCoord;         //!< 現在のスキャンオフセットの座標系 1:RADEC 2:LB 3:AZEL
	//scanSequence_t seq;

  //char cstart[24];      //!< 観測時刻の設定 YYYYMMDDhhmmss.0 (JST)

  //int pofFlg;           //!< Position offset 座標定義 0:虚角 1:実角
  int pofCoord;         //!< Position offset coord 一時格納用。5点法、9点法で使用
  double pof[2];        //!< Position offset 一時格納用。5点法、9点法で使用

  int rastStart;        //!< ラスター開始フラグ 0:none 1:RasterStart
  time_t rastStartTime;//!< ラスター開始時刻
  double* rastSof;      //!< ラスターの1スキャンのTrkIntervalごとのスキャンオフセット格納用 [rad]
  int rastSofSize;      //!< trkSofに登録されているスキャン位置の数
  int rastSofNo;        //!< 今何番目のラスタースキャン位置を見ているか
//  double P[30];         //!< アンテナ器差補正パラメータ

//  XTRKO xout;           //!< 45m追尾ライブラリ計算結果出力用構造体　081114 calc.cpp
  //double doazel[2];     //!< 計算結果AZEL [rad]
  //double doazel2[2];    //!< 計算結果に器差補正を加えた値 [deg]
//  double doazelC[2];    //!< ACUへ出力用のAZEL [deg]
  double deazel[2];     //!< dazel-doazel2 [deg]
//  double dgtdat[8][2];  //!< MapCenter(RADEC)(LB) Obs(RADEC)(LB) [rad] 
//  tAcu acuStat;         //!< ACU現在のステータス
//  int acuStatSBefore[5]; //!< 一つ前に取得したACUのステータスコード

  pthread_t thrdStatusID;//!< ステータス取得スレッドID
  pthread_t thrdID;     //!< スレッドID
  int thrdRet;          //!< スレッドの戻り値
  int thrdRun;          //!< スレッド実行状況 1:実行 0:停止
  int thrdStatusRun;
  int antZoneType;      //!< アンテナのZoneのタイプ1‾5まで定義
  int antZone;          //!< Zone(CW,CCWのどちらの領域で走らせるか) 0:移動距離が短い方 1:CW 2:CCW
  int trackStatCnt;     //!< アンテナ追尾状態のカウント用
  int trackStat;        //!< アンテナ追尾状態。ビットフラグによって表現 0:Tracking -1:az角がSwing -2:el角がSwing -4:Zone移動を含むSwing
  FILE* fp;             //!< 器差補正データ収集用
  //int dayFlg;           //!< trk_00()を呼び出すタイミング用。UTにおける日付が変わったとき呼ぶ。
	scanCurrenctStatus_t curr;
//  int sofNo;            //!< scan offsetの何番目を処理しているか
//  int offNo;            //!< 何番のOff点を処理しているか 0 or 1
//  int offFlg;           //!< 直前にOff点観測をしているか 0:No 1:Yes
//  unsigned int lst;     //!< LST時分秒のみを秒換算したもの
}tParamTrk;



//const double ABS_ZERO = 273.15;
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


static int init=0;
static tParamTrk p;
//static int comAct=0;    //!< RS-232Cの状態. 1:使用可, 0:使用不可 081113 moved to acu.cpp
//static double sps;     //!< Second Per Step 1Step処理するのにかかる時間 [sec]

static void* _trkThrdStatus(void* _p);
static void* _trkThrdMain(void* _p);

//static void _trkSetSof(double* sof, int coord, double posAngleRad);//081115 moved to trkCalc.h
static void _trkSetOff();

//static int trkCal();//081114 moved to calc.cpp
//static void trkCalInst(double X, double Y, double* dX, double* dY);//081114 moved to calc.cpp
static void _trkCalTrackStatus(const int reset);
static void _trkCalTrackStatusReset();//081114 in
//static int trkSetACU();//081113 moved to acu.cpp
//static int trkSafetyCheck();//081113 moved to acu.cpp
static void _trkCalZoneType(double az);//081114 moved to calc.cpp; 081117 came back
static void _trkCalZoneTypeInit();
static void setParam();
static int checkParam();

/*! ¥fn int trkInit()
¥brief 初期化プロセスでこのモジュールを使用する際に必ず最初に呼ぶ。
¥brief ステータスの取得のみの場合は最低限のパラメータがあればよい。
¥brief また、パラメーターを設定しなおしたいときは一度trkEnd()を行ってからtrkInit()する
¥return 0 成功
* thread 1, phase A0
*/
int trkInit(){
  int ret;

  if(init){
    trkEnd();
    init=0;
  }
  memset(&p, 0, sizeof(p));
  //comAct = 0;
  p.pofCoord = 3; //!< ポインティングオフセット座標系の初期値AZEL
/* 090611 out, since we use controlBoard.cpp
  //! 接続パラメータ取得  
  if(confSetKey("TrkUse"))
    p.TrkUse = atoi(confGetVal());
 // if(confSetKey("TrkCom"))//081113 out
 //   p.TrkCom = atoi(confGetVal());
  //! 接続パラメータチェック
  if(p.TrkUse < 0 || p.TrkUse > 2){
    uM1("TrkUse(%d) error!!¥n", p.TrkUse);
    return -1;
  }
*/
//  if(p.TrkUse == 1){//081113 out
//    if(p.TrkCom == 0){ //!< プラス値はttyUSBを使用することを意味する。マイナス値はttySを使用
//      uM2("TrkUse(%d) TrkCom(%d) error!!¥n", p.TrkUse, p.TrkCom);
//      return -1;
//    }
//  }

  init = 1;
	if(controlBoardGetAntenna() == CONTROL_BOARD_USE_NO){//090611 in
/* 090611 out
  if(p.TrkUse==0)
*/
		return TRK_NOT_USE;
	}

  //! Initialize timer module
  p.vtm=tmInit();

  //! Initialize ACU
	if(controlBoardGetAntenna() == CONTROL_BOARD_USE_FULL){//090611 in
/*
  if(p.TrkUse==1){
*/
	acuInit();
    ret = acuGetACU();
    if(ret){
      uM1("trkInit(); acuGetACU(); ret = %d error!!",ret);
      return ret;
    }
    ret = acuSafetyCheck();
    if(ret){
      uM("trkInit(); acuSafetyCheck(); error");
      //return ret;//!< トータルアラームに引っかるため一時的にコメントアウト
    }

  }
	ret = calcInit();//090610 in
	if(ret){//090610 in
		uM1("trkInit(); calcInit(); ret = %d error!!", ret);
	}
/* 090610 out
	calcInit();//081117 in
*/
//	calcInit(&p);
  //if((p.fp=fopen("kisa.cvs","a")) == NULL){
  //  uM("fileopen error");
  //}
  //fprintf(p.fp, "PAZ,PEL,PAZ+,PEL+,RAZ,REL");

  //! ステータス取得開始
  int status = pthread_create(&p.thrdStatusID, NULL, _trkThrdStatus, NULL);
  if(status != 0){
    uM1("trkInit(); pthread_create(trkTrhdStatus); status=%d error", status);
    return TRK_THRD_ERR;
  }

  return 0;
}

/*! ¥fn int trkInitParam()
¥brief 観測パラメータをセットし、天体の初期位置を算出する。
¥return 0:Success other:error
* thread 1, phase C0
*/
int trkInitParam(){
  //char cerr[256];
  int ret;

	if(controlBoardGetAntenna() == CONTROL_BOARD_USE_NO){//090611 in
/* 090611 out
  if(p.TrkUse==0)
*/
		return TRK_NOT_USE;
	}

  if(p.thrdRun){
    uM("trkInitParam(); Tracking already started");
    return 0;
  }

  //! 観測パラメータ初期化
  p.TrkInterval = 0;
  //memset(p.antPosLaLo, 0, sizeof(p.antPosLaLo[0]) * 2);
/*
  p.scan.OnCoord = 0;
  p.scan.OffCoord = 0;
  p.scan.OffNumber = 0;
  p.scan.ScanFlag = 0;
  p.scan.OffInterval = 0;
  p.scan.OffMode = 0;
  if(p.scan.SetPattern){
    free(p.scan.SetPattern);
    p.scan.SetPattern = NULL;
  }
  p.scan.OnNumber = 0;
  p.scan.posAngleRad = 0;
  p.scan.LineTime = 0;
*/
  //p.LinePathX_Rad = 0;
  //p.LinePathY_Rad = 0;
  memset(p.AntAzElMarg, 0, sizeof(p.AntAzElMarg[0]) * 2);
  p.AntAzElMargThrsd = 0;
//  memset(p.AntInst, 0, sizeof(p.AntInst[0]) * 7);
//  p.TrkTimeOffset = 0;
  //p.RSkyInterval = 0;

/* 081115  moved to celectialObject.cpp
  p.celObj.ioflg = 0;
  p.celObj.iaoflg = 0;
  p.celObj.ibjflg = 0;
  p.celObj.drdlv = 0;
  memset(p.celObj.dsourc, 0, sizeof(p.celObj.dsourc[0]) * 2);
*/
  memset(p.dazelmin, 0, sizeof(p.dazelmin[0]) * 2);
  memset(p.dazelmax, 0, sizeof(p.dazelmax[0]) * 2);
  
//  memset(&p.xin, 0, sizeof(p.xin));
  //memset(p.sof, 0, sizeof(p.sof[0]) * 2);
//  p.sofCoord = 0;

/* 081117  moved to scanTablle.cpp
  if(p.seq.mapPattern){
    free(p.seq.mapPattern);
    p.seq.mapPattern = NULL;
  }
  if(p.seq.off){
    free(p.seq.off);
    p.seq.off = NULL;
  }
  if(p.seq.on){
    free(p.seq.on);
    p.seq.on = NULL;
  }
  if(p.seq.linePath){
    free(p.seq.linePath);
    p.seq.linePath = NULL;
  }
*/
  //memset(p.cstart, 0, sizeof(p.cstart[0]) * 24);
  //memset(p.dweath, 0, sizeof(p.dweath[0]) * 3);
  //! ポインティングオフセットは初期化しない。
  //p.pofFlg = 0;
  //p.pofCoord = 0;
  //memset(p.pof, 0, sizeof(p.pof[0]) * 2);
  p.rastStart = 0;
  p.rastStartTime = 0;
  if(p.rastSof){
    free(p.rastSof);
    p.rastSof = NULL;
  }
  p.rastSofSize = 0;
  p.rastSofNo = 0;
//  memset(p.P, 0, sizeof(p.P[0]) * 30);

//  memset(&p.xout, 0, sizeof(p.xout));
//  memset(p.doazel, 0, sizeof(p.doazel[0]) * 2);
//  memset(p.doazel2, 0, sizeof(p.doazel2[0]) * 2);
//  memset(p.doazelC, 0, sizeof(p.doazelC[0]) * 2);//081113 out
  memset(p.deazel, 0, sizeof(p.deazel[0]) * 2);
/*
  for(int i = 0; i < 8; i++){
    memset(p.dgtdat[i], 0, sizeof(p.dgtdat[0][0]) * 2);
  }
*/
//  memset(&p.acuStat, 0, sizeof(p.acuStat));//081113 out
//  memset(p.acuStatSBefore, 0, sizeof(p.acuStatSBefore[0]) * 5);//081113 out
  if(p.fp){
    fclose(p.fp);
    p.fp = NULL;
  }
  p.thrdRet = 0;
  //p.dayFlg = 0;
  p.curr.sofNo = 0;
  p.curr.offNo = 0;
  p.curr.offFlg = 0;
//  p.lst = 0;

  //! 前回のポインティングOffSetを設定
  double pofSec[2];
  pofSec[0] = p.pof[0] * rad2sec;
  pofSec[1] = p.pof[1] * rad2sec;
  trkSetPof(pofSec, p.pofCoord);

  //! 観測パラメータ取得
  setParam();
	scanTableInit();
  if(scanTableGetScanFlag() == SCANTABLE_FLAG_LEE){//090611 in
/* 090611 out
  if(scanTableGetScanFlag() == 1002){
*/
    //! Lee Tracking
	celestialObjectInitLeeTracking();//081115 in
/*
    p.celObj.ioflg = 99;                  //!< 99:AZEL
    p.celObj.iaoflg = 3;                  //!< 3:AZEL
    p.celObj.ibjflg = 2;                  //!< J2000.0
    p.celObj.dsourc[0] = 0 * PI / 180.0;  //!< 適当な値 AZ
    p.celObj.dsourc[1] = 10 * PI / 180.0; //!< 適当な値 EL
*/
  }else{//081115 in
		if(celestialObjectInit()){//081115 in
			uM("trkInitParam(); celestialObjectInit(); error");
			p.thrdRet = TRK_PARAM_ERR;
			return p.thrdRet;
		}
	}//081115 in

  if(checkParam()){
    uM("trkInitParam(); checkParam(); error");
    p.thrdRet = TRK_PARAM_ERR;
    return p.thrdRet;
  }

/* 081117 out
  //! Initialize map pattern
  if(p.scan.ScanFlag >= 0 && p.scan.ScanFlag <= 6){
    //! On,5Point,Grid,Random,9Point,Raster
    int len = strlen(p.scan.SetPattern);
	uM1("trkInitParam(); p.scan.SetPattern length=%d", len)
    int i, j, k;
    
    uM1("trkInitParam(); debug SetPattern %s", p.scan.SetPattern);
    for(i = 0, j = 0, k = 0; i < len; i++){
      if(p.scan.SetPattern[i] == ' ' || p.scan.SetPattern[i] == '\t'){
	continue;
      }
      else if((p.scan.SetPattern[i] >= 'A' && p.scan.SetPattern[i] <= 'Z')
	      || (p.scan.SetPattern[i] >= '0' && p.scan.SetPattern[i] <= '9')){
	if(j == 0){
	  k++;
	}
	j++;
      }
      else if(p.scan.SetPattern[i] == ','){
	j = 0;
      }
    }

    //! マップパターンの領域を確保する。
    p.seq.mapNum = k;
    uM1("trkInitParam(); debug p.seq.mapNum=%d", p.seq.mapNum);
    //if(k != p.scan.OnNumber){
    //  uM3("trkStart(); SetPattern Num[%d] != OnNumber[%d] and use first [%d] pieces", k, p.scan.OnNumber, p.seq.mapNum);
    //}
    p.seq.mapPattern = (int*) malloc(sizeof(p.seq.mapPattern[0]) * p.seq.mapNum);
    memset(p.seq.mapPattern, 0, sizeof(p.seq.mapPattern[0]) * p.seq.mapNum);
    //! マップパターンのID記録 0:R +:On -:Off
    for(i = 0, k = 0; k < p.seq.mapNum;){
      while((p.scan.SetPattern[i] == ' ' || p.scan.SetPattern[i] == '\t') && p.scan.SetPattern[i] != '\0'){
	i++;
      }
      
      if(p.scan.SetPattern[i] == 'R'){
		//! R
		p.seq.mapPattern[k] = 0;
		k++;
      }
      else if(p.scan.SetPattern[i] >= 'A' && p.scan.SetPattern[i] <= 'Z'){//081114 in
//      else if(p.scan.SetPattern[i] >= 'A' && p.scan.SetPattern[0] <= 'Z'){//081114 out
		//! Off-Point
		p.seq.mapPattern[k] = -(p.scan.SetPattern[i] - 'A' + 1);
		//! 不正データの場合は無視
		if(p.seq.mapPattern[k] < - p.scan.OffNumber){
			uM3("trkInitParam(); invalid SetPattern[%d]=%c(%d): too much off points!", i, p.scan.SetPattern[i], p.seq.mapPattern[k]);
//		  char tmp = - p.seq.mapPattern[k] - 1 + 'A';
//		  uM2("trkInitParam(); invalid SetPattern[%d(%c)]: too much off points!", i, tmp);
		  p.seq.mapPattern[k] = 0;
		}
		else{
		  k++;
		}
      }
      else if(p.scan.SetPattern[i] >= '0' && p.scan.SetPattern[i] <= '9'){
		//! On-Point
		while(p.scan.SetPattern[i] >= '0' && p.scan.SetPattern[i] <= '9' && p.scan.SetPattern[i] != '\0'){//more than two digits
		  p.seq.mapPattern[k] = (p.scan.SetPattern[i] - '0') + p.seq.mapPattern[k] * 10;
		  i++;
		}
		i--;//081114 in
		//! 不正データの場合は無視
		if(p.seq.mapPattern[k] > p.scan.OnNumber){
			uM3("trkInitParam(); invalid SetPattern[%d]=%c(%d): too much on points!", i, p.scan.SetPattern[i], p.seq.mapPattern[k]);
		  //uM1("trkInitParam(); invalid SetPattern[%d]: too much on points!", p.scan.SetPattern[k]);
		  p.seq.mapPattern[k] = 0;
		}
		else{
		  k++;
		}
      }
     else if(p.scan.SetPattern[i] == ','){
	}
      else{
//     else if(p.scan.SetPattern[i] != ','){
		uM1("trkInitParam(); invalid SetPattern data [%c]", p.scan.SetPattern[i]);
      }

      while(p.scan.SetPattern[i] != ',' && p.scan.SetPattern[i] != '\0'){
		i++;
      }
	if(p.scan.SetPattern[i] == '\0'){//081114 in
		break;
	}

      i++;
    }
	if(p.seq.mapNum != k){
		uM("CAUTION!! This scan table seems inconsistent. Please check it.");
	}
    p.seq.mapNum = k; //!< 不正データを除去後の観測点数
    //uM1("trkInitParam(); debug p.seq.mapNum=%d", p.seq.mapNum);
    //uM1("trkInitParam(); debug p.scan.OnNumber=%d", p.scan.OnNumber);
    //uM1("trkInitParam(); debug p.scan.OffNumber=%d", p.scan.OffNumber);

    //! 確認
//     for(k = 0; k < p.seq.mapNum; k++){
//       if(p.seq.mapPattern[k] < - p.scan.OffNumber){
// 	char tmp = - p.seq.mapPattern[k] - 1 + 'A';
// 	uM1("trkInitParam(); invalid SetPattern[%c] ans set 'A'", tmp);
// 	p.seq.mapPattern[k] = -1;
//       }
//       else if(p.seq.mapPattern[k] > p.scan.OnNumber){
// 	uM1("trkInitParam(); invalid SetPattern[%d] ans set '1'", p.scan.SetPattern[k]);
// 	p.seq.mapPattern[k] = 1;
//       }
//     }
    //! debug
    //printf("trkInitParam(); debug mapPattern = [");
    for(k = 0; k < p.seq.mapNum; k++){
	uM1("map:%d", p.seq.mapPattern[k]);
    //  printf("%d,", p.seq.mapPattern[k]);
    }
    //printf("]¥n");
  }
*/
  if(scanTableGetScanFlag() == SCANTABLE_FLAG_RASTER || scanTableGetScanFlag() == SCANTABLE_FLAG_CROSS){//090611 in
/* 090611 out
  if(scanTableGetScanFlag() == 6){
    //! Raster
*/
    p.rastSofSize = scanTableGetLineTime() / p.TrkInterval + 2; //!< 最後の1回分と端数が出た場合の分を足しておく
    p.rastSof = (double*)malloc(sizeof(p.rastSof[0]) * p.rastSofSize * 2);
    memset(p.rastSof, 0, sizeof(p.rastSof[0]) * p.rastSofSize * 2);
  }

  //! Initialize trk45 library
	ret=calcInitParam();//081114 in
	if(ret){
		p.thrdRet = ret;
		return ret;
	}
//  strcpy(p.cstart, tmGetTimeStr(p.vtm, p.TrkTimeOffset));//081114 out
//  if(trk_00(p.cstart, "", cerr) != 0) {
//    uM2("trkInitParam(); trk_00(%s) %s", p.cstart, cerr);
//    p.thrdRet =  TRK_TRK00_ERR;
//    return p.thrdRet;
//  }
  //p.dayFlg = 1;//081114 out

	uM("trkInitParam() #1");	
  //! Get Zone Type

	// Initialize時はOn点のoffsetを加味した値を渡す
	double iniOffset[2];// 090430 in // 090608 in for 30 cm
	iniOffset[0] = scanTableGetOnValue(0);// 090611 in
	iniOffset[1] = scanTableGetOnValue(1);// 090611 in
//	memcpy(iniOffset, scanTableGetOn(0), sizeof(double) * 2);// 094030 in
	calcSetSof(iniOffset, scanTableGetOnCoord(), scanTableGetPosAngleRad());// 090608 in

/* 090608 out
	memcpy(iniOffset, p.on, sizeof(double) * 2);// 094030 in
	trkSetSof(iniOffset, p.OnCoord);// 090430 in Initialize時はOn点のoffsetを加味した値を渡す
*/
/* 090605 out, since penguin_tracking is now thread-safe.
  uLock();
*/
  ret = calcTrkCal(p.curr.offFlg, p.curr.offNo);
/* 090605 out
  uUnLock();
*/
	  _trkCalZoneType(calcGetDoAz2());//081117 in
  if(ret){
    uM2("trkInitParam(); calcTrkCal(); [%d(%s)] error!!", ret, errnoStr(ret));
    p.thrdRet = ret;
    return p.thrdRet;
  }
  _trkCalZoneTypeInit();

  return 0;
}

/*! ¥fn int trkStart()
¥brief 観測テーブルに基づいた追尾の実行する。
¥return 0:Success other:error
*/
int trkStart(){
  int status;

  //! Thread start
  status = pthread_create(&p.thrdID, NULL, _trkThrdMain, NULL);
  if(status != 0){
    uM1("trkStart(); pthread_create(); status=%d error!!", status);
    p.thrdRet = TRK_THRD_ERR;
    return p.thrdRet;
  }
  p.thrdRet = 0;
  return 0;
}

/*! ¥fn int trkStop()
¥brief 追尾の停止
¥retval 0 成功
* thread 1, phase C1, C2
*/
int trkStop(){
	if(controlBoardGetAntenna() == CONTROL_BOARD_USE_NO){//090611 in
/* 090611 out
  if(p.TrkUse==0){
*/
		return TRK_NOT_USE;
	}
  if(p.thrdRun){
    p.thrdRun=0;
    pthread_join(p.thrdID, NULL);
  }
  else{
    return TRK_THRD_STOPPED_ERR;
  }
  return 0;
}

/*! ¥fn int trkRepeat
¥brief 追尾実行状態の取得
*/
int trkRepeat(){
	if(controlBoardGetAntenna() == CONTROL_BOARD_USE_NO)//090611 in
/* 090611 out
  if(p.TrkUse==0)
*/
    return TRK_NOT_USE;

  if(p.thrdRun == 0 && p.thrdStatusRun == 0){
    return TRK_THRD_STOPPED_ERR;
  }
  return p.thrdRet;
}

/*! ¥fn void* _trkThrdMain(void* _p)
¥brief 自動追尾スレッド
¥param[in] _p パラメータ配列(未使用)
¥return NULL(0):成功
*/
void* _trkThrdMain(void* _p){
  //char tmp[4096];
  int ret;
  int err;

  //! ステータス取得スレッドが走っていたら停止する。
  if(p.thrdStatusRun){
    p.thrdStatusRun=0;
    pthread_join(p.thrdStatusID, NULL);
    p.thrdStatusID = NULL;
  }

  //! Main Loop
  p.thrdRun = 1;
  while(p.thrdRun){

    //! タイミング調整
    if(!p.rastStart){
      //! Not Raster
      if(tmGetLag(p.vtm) < p.TrkInterval){
		tmSleepMSec(p.TrkInterval * 10.0);
		continue;
      }
    }

    //! RastStartフラグが立っていたらRaster処理を行う。
    //double a = tmGetDiff(p.rastStartTime, 0);
    //printf("_trkThrdMain(); debug tmGetDiff(); diff=%lf sec¥n", a);
    int rastFlg = 0;
    if(p.rastStart){
      if(p.rastSofNo > 0){
		rastFlg = 1;
      }
      else if(p.rastSofNo == 0 && tmGetDiff(p.rastStartTime, 0) < p.TrkInterval){
		rastFlg = 1;
      }
      else{
		//! スタート時刻に近づくまで通常処理
		if(tmGetLag(p.vtm) < p.TrkInterval){
		tmSleepMSec(p.TrkInterval * 10.0);
		continue;
		}
      }
    }
    if(rastFlg){
      //! ラスター中
      g_rastEndFlag = RUN_RASTENDF;
      double sof[2];
      if(p.rastStart == 1){//update080529*1
		if(p.rastSofNo < p.rastSofSize){
			sof[0] = p.rastSof[p.rastSofNo*2];
			sof[1] = p.rastSof[p.rastSofNo*2+1];
			calcSetXinDscn(scanTableGetOnCoord(), sof[0], sof[1]);//090207 in
/* 090207 out
			calcSetXinDscn(scanTableGetOnCoord(), sof[0], sof[1], sof[0], sof[1]);//081115 in
*/
/* 081115 out
			p.xin.iasflg = p.scan.OnCoord;
			p.xin.dscnst[0] = sof[0];
			p.xin.dscnst[1] = sof[1];
			p.xin.dscned[0] = sof[0];
			p.xin.dscned[1] = sof[1];
*/
			//! 1msec以内になるまで待つ。
			double offsetMSec = p.rastSofNo * p.TrkInterval * 1000.0;
			while(tmGetDiff(p.rastStartTime, offsetMSec) > 0.001){
			}
			p.rastSofNo++;
			//uM3("_trkThrdMain(); rastSofNo[%03d] sof(%.15lf,%.15lf)¥"",
			//    p.rastSofNo, sof[0], sof[1]);
	      }
	      else{
			//! ラスター終了
			p.rastStart = 0;
			p.rastSofNo = 0;
			g_rastEndFlag = END_RASTENDF;
	      }
	}//*1
    }
    //printf("_trkThrdMain(); debug sof(%.15lf,%.15lf)¥" rastStart=%d rastSofNo=%d rastSofSize=%d¥n",
    //p.xin.dscnst[0], p.xin.dscned[1], p.rastStart, p.rastSofNo, p.rastSofSize);

	tmReset(p.vtm);//081117 in; another tmClass_t instance is introduced in calc.cpp, and then, we need reset this instance. 
    //tmReset(p.vtm);//081117 out never read before the invocation of calcTrkCal()
    //strcpy(p.cstart, tmGetTimeStr(p.vtm, p.TrkTimeOffset));//081114 out
    
    ret = 0;
	if(controlBoardGetAntenna() == CONTROL_BOARD_USE_FULL && p.rastStart == 0){//090611 in
/*
    if(p.TrkUse == 1 && p.rastStart == 0){//32 mの場合
*/
//    if(p.TrkUse == 1){
      ret = acuGetACU();
      if(ret){
		uM1("_trkThrdMain(); acuGetACU(); ret = %d", ret);
		p.thrdRet = ret;
      }
    }
    
    //! 追尾状態の算出
    _trkCalTrackStatus(ret);    

    //! 追尾計算
/* 090605 out, since penguin_tracking is now thread-safe.
    uLock();
*/
    ret = calcTrkCal(p.curr.offFlg, p.curr.offNo);
/* 090605 out
    uUnLock();
*/
	_trkCalZoneType(calcGetDoAz2());//081117 in


	const double doaz2=calcGetDoAz2();
	const double doel2=calcGetDoEl2();
    //printf("debug dazel  [rad]    : %.9lf,%.9lf,¥n", p.xout.dazel[0], p.xout.dazel[1]);
    //printf("debug %s doazel2 : %.9lf,%.9lf,¥n", p.cstart, doaz2, doel2);
    //printf("debug OBS    [rad]    : %+10.9lf %+10.9lf (RADEC)¥n", p.dgtdat[4][0], p.dgtdat[4][1]);
    if(ret < 0){
      p.thrdRet = ret;
      continue;
    }

    //! アンテナの駆動限界を超える場合は追尾しない
    err=0;
	acuSetDoAzC(doaz2);
	acuSetDoElC(doel2);
    //p.doazelC[0] = doaz2;//081113 out
    //p.doazelC[1] = doel2;//081113 out
    if(p.antZone == 2 && doaz2 <= p.dazelmin[0]){
      //uM3("Antenna Az(%lf) <= %lf (zone=%d)",
      //doaz2, p.dazelmin[0], p.antZone);
      err = TRK_LIMIT_AZ_ERR;
    } 
    else if(p.antZone == 1 && doaz2 >= p.dazelmax[0]){
      //uM3("Antenna Az(%lf) >= %lf (zone=%d)",
      //doaz2, p.dazelmax[0], p.antZone);
      err = TRK_LIMIT_AZ_ERR;
    }
    if(doel2 < p.dazelmin[1] || doel2 > p.dazelmax[1]){
      //uM3("Antenna El(%lf) over limit(%lf - %lf)",
      //doel2, p.dazelmin[1], p.dazelmax[1]);
      if(err){
		err = TRK_LIMIT_AZ_EL_ERR;
      }
      else{
		//! El上限リミットのみの場合はElを臨界角から約50[sec]離れた所までもって行き、Azは動かす
		double marg = 50.0 / 3600.0;
		if(doel2 > p.dazelmax[1]){
			acuSetDoElC(p.dazelmax[1] - marg);
			//p.doazelC[1] = p.dazelmax[1] - marg;//081113 out
		}
		err = TRK_LIMIT_EL_ERR;
      }
    }
    if(err){
      p.thrdRet = err;
      //tmSleepMSec(p.TrkInterval * 1000.0);
      //! El上限リミットのみの場合はElを臨界角から約50[sec]離れた所までもって行き、Azは動かす
      if(!(doel2 > p.dazelmax[1]  && err == TRK_LIMIT_EL_ERR)){
		continue;
      }
    }

    if(0){
      //! 結果出力
      //sprintf(tmp,"START => %s¥n", p.cstart);
      //sprintf(tmp + strlen(tmp),"AZEL [deg]   : %+10.9lf %+10.9lf¥n",doaz2, doel2);
      //sprintf(tmp + strlen(tmp),"DAZ [rad/sec]: %+10.9lf¥n", p.xout.ddazel[0]);
      //sprintf(tmp + strlen(tmp),"DEL [rad/sec]: %+10.9lf¥n", p.xout.ddazel[1]);
      //! マップセンター座標
      //sprintf(tmp + strlen(tmp),"MAP [rad]    : %+10.9lf %+10.9lf (RADEC)¥n", p.dgtdat[1][0], p.dgtdat[1][1]);
      //sprintf(tmp + strlen(tmp),"MAP [rad]    : %+10.9lf %+10.9lf (LB)¥n",    p.dgtdat[2][0], p.dgtdat[2][1]);
      //! 観測位置座標
      //sprintf(tmp + strlen(tmp),"OBS [rad]    : %+10.9lf %+10.9lf (RADEC)¥n", p.dgtdat[4][0], p.dgtdat[4][1]);
      //sprintf(tmp + strlen(tmp),"OBS [rad]    : %+10.9lf %+10.9lf (LB)¥n",    p.dgtdat[5][0], p.dgtdat[5][1]);
      //uM1("%s",tmp);
    }
	if(controlBoardGetAntenna() == CONTROL_BOARD_USE_FULL){//090611 in
/* 090611 out
    if(p.TrkUse == 1){
*/
      ret = acuSafetyCheck();
      //! debug
      //! ACUへ送信
      if(ret){
		p.thrdRet = ret;
      }
      else{
		ret = acuSetACU(p.antZone);
		if(ret){
		uM1("_trkThrdMain(); acuSetACU(); return %d", ret);
		p.thrdRet = ret;
		}
      }
    }//!< if(p.TrkUse==1)

    if(err == 0){
      p.thrdRet = 0;
    }
    //tmSleepMSec(p.TrkInterval * 1000.0);
  }//!< while(1)
  p.thrdRun = 0;
  //p.thrdRet = 0;

  //! 現在のRealAZELで指令をかける。停止させる。
	if(controlBoardGetAntenna() == CONTROL_BOARD_USE_FULL){//090611 in
/* 090611 out
  if(p.TrkUse == 1){
*/
    ret = acuSafetyCheck();
    if(!ret){
      p.antZone = 0; //! 0:Auto
	double acuStatDazel[2];
	acuGetRAZEL(acuStatDazel);
	acuSetDoAzC(acuStatDazel[0]);//081113 in
	acuSetDoElC(acuStatDazel[1]);//081113 in
//	acuSetDoAzC(p.acuStat.dazel[0]);//081113 out
//	acuSetDoElC(p.acuStat.dazel[1]);//081113 out
//      p.doazelC[0] = p.acuStat.dazel[0];//081113 out
//      p.doazelC[1] = p.acuStat.dazel[1];//081113 out
      ret = acuSetACU(p.antZone);
      if(ret){
	uM1("_trkThrdMain(); acuSetACU(); return %d", ret);
	p.thrdRet = ret;
      }
    }
    else{
      p.thrdRet = ret;
    }
  }

  int status = pthread_create(&p.thrdStatusID, NULL, _trkThrdStatus, NULL);
  if(status != 0){
    uM1("_trkThrdMain(); pthread_create(_trkThrdStatus); status=%d error", status);
    return NULL;
  }

  return NULL;
}

/*! ¥fn void* _trkThrdStatus(void* _p)
¥brief スケジュール観測していないときのステータス取得スレッド
¥param[in] _p パラメータ配列(未使用)
¥return NULL(0):成功
* thread 2, phase *
*/
void* _trkThrdStatus(void* _p){
  int ret;

  p.thrdStatusRun = 1;
  while(p.thrdStatusRun){
    if(tmGetLag(p.vtm) < p.TrkInterval){
      tmSleepMSec(p.TrkInterval * 10.0);
      continue;
    }
    tmReset(p.vtm);
    //p.thrdRet = 0;
	if(controlBoardGetAntenna() == CONTROL_BOARD_USE_FULL){//090611 in
/* 090611 out
    if(p.TrkUse == 1){
*/
      ret = acuGetACU();
      if(ret){
	uM2("_trkThrdStatus(); acuGetACU(); [%d(%s)]", ret, errnoStr(ret));
	p.thrdRet = ret;
      }
      ret = acuSafetyCheck();
    }
  }
  p.thrdStatusRun = 0;
  return NULL;
}


/*! ¥fn int trkEnd()
¥brief 終了時に呼ぶ
¥retval 0 成功
* thread 1, phase A2
*/
int trkEnd(){
  void* ret;

  init = 0;
  if(p.thrdRun){
    p.thrdRun=0;
    pthread_join(p.thrdID, &ret);
    p.thrdID = NULL;
  }
  if(p.thrdStatusRun){
    p.thrdStatusRun=0;
    pthread_join(p.thrdStatusID, &ret);
    p.thrdStatusID = NULL;
  }
	if(controlBoardGetAntenna() == CONTROL_BOARD_USE_FULL){//090611 in
/* 090611 out
	if(p.TrkUse == 1){
*/
		if(acuEnd()){
			return -1;
		}
	}
//  if(comAct){
//    rs232cEnd(p.TrkCom);
//    comAct = 0;
//  }
  if(p.vtm){
    tmEnd(p.vtm);
    p.vtm = NULL;
  }
  if(p.fp){
    fclose(p.fp);
    p.fp = NULL;
  }
	scanTableEnd();
	calcEnd();
/*
  if(p.scan.SetPattern){
    free(p.scan.SetPattern);
    p.scan.SetPattern = NULL;
  }
*/
/* 081117 moved to scanTable.cpp
  if(p.seq.mapPattern){
    free(p.seq.mapPattern);
    p.seq.mapPattern = NULL;
  }
  if(p.seq.on){
    free(p.seq.on);
    p.seq.on = NULL;
  }
  if(p.seq.linePath){
    free(p.seq.linePath);
    p.seq.linePath = NULL;
  }
  if(p.seq.off){
    free(p.seq.off);
    p.seq.off = NULL;
  }
*/
  if(p.rastSof){
    free(p.rastSof);
    p.rastSof = 0;
  }
  p.rastSofSize = 0;

  return 0;
}


/*! ¥fn void _trkCalTrackStatus()
¥brief 追尾状態を算出する
¥brief trackStatを設定する 0:Tracking 1:Counting -:Swing(ビットフラグによって表現 1:az角がSwing 2:el角がSwing 4:Zone移動を含むSwing 8:リセット)
¥param[in] reset 0:None other:Reset Tracking Status and return
*/
void _trkCalTrackStatus(const int reset){
  double dAz;
  double dEl;
  int stat;   //!< 0:Tracking 1:Counting -:ビットフラグ(1:Az 2:El 4:Zone 8:StateCalFault)

  //! Reset Tracking Status
  //! etc)RealAzElが正常に取得されていない場合、Update Sof
  if(reset){
	_trkCalTrackStatusReset();//081114 in
    //p.trackStatCnt = 0;//081114 out
    //p.trackStat = -8;//081114 out
    return;
  }
	/* The following are run only on _trkThrdMain thread. */

  //! ACUと通信して無い場合は常にTracking
	if(controlBoardGetAntenna() != CONTROL_BOARD_USE_FULL){//090611 in
/* 090611 out
  if(p.TrkUse != 1){
*/
    p.trackStatCnt = p.AntAzElMargThrsd;
    p.trackStat = 0;
    return;
  }

  stat = 0;
	//uM2("p.antZone = %d, acuGetRZONE() = %d", p.antZone, acuGetRZONE());
// 081210 for 30-cm telescope, ignore because I cannot guess why. 
	if(p.antZone != 0 && p.antZone != acuGetRZONE()){
//  if(p.antZone != 0 && p.antZone != p.acuStat.zone){
		stat = -4;
	}

	double acuStatDazel[2];
	acuGetRAZEL(acuStatDazel);
	const double doaz2=calcGetDoAz2();
	const double doel2=calcGetDoEl2();
	p.deazel[0] = acuStatDazel[0] - doaz2;
	p.deazel[1] = acuStatDazel[1] - doel2;
//  p.deazel[0] = p.acuStat.dazel[0] - doaz2;//081113 out
//  p.deazel[1] = p.acuStat.dazel[1] - doel2;//081113 out
	//uM2("doaz2 = %f, acuStatDazel[0] = %f", doaz2, acuStatDazel[0]);
	//uM2("doel2 = %f, acuStatDazel[1] = %f", doel2, acuStatDazel[1]);
  
  //! 見かけのErrAzからそのときのElにおけるErrAzへ変換する。
  //p.deazel[0] = 2.0 * asin(cos( doel2 * PI / 180.0) * sin(p.deazel[0] * PI / 180.0)); 
  //p.deazel[0] *= 180.0 / PI;
  p.deazel[0] = p.deazel[0] * cos(doel2 * PI / 180.0);

  dAz = fabs(p.deazel[0]);
  dEl = fabs(p.deazel[1]);
  if(dAz > p.AntAzElMarg[0]){
    stat += -1;
  }
  if(dEl > p.AntAzElMarg[1]){
    stat += -2;
  }
	//uM2("dAz = %f, p.AntAzElMarg[0] = %f", dAz, p.AntAzElMarg[0]);
	//uM2("dEl = %f, p.AntAzElMarg[1] = %f", dEl, p.AntAzElMarg[1]);

	//uM1("stat = %d", stat);

  if(stat == 0){
    //! Now tracking
    if(p.trackStatCnt > p.AntAzElMargThrsd){
      //! Tracking (count over threshold)
      p.trackStat = 0;
    }
    else{
      //! Counting
      p.trackStatCnt++;
      p.trackStat = 1;
    }
  }
  else{
    //! Swing
    p.trackStatCnt = 0;
    p.trackStat = stat;
  }
	//uM1("p.trackStat = %d", p.trackStat);

  return;
}

void _trkCalTrackStatusReset(){//_trkCalTrackStatus(1)
	p.trackStatCnt = 0;
	p.trackStat = -8;
	return;
}



/*! ¥fn void trkCalZoneType(double az)
¥brief アンテナ局の北緯位置からアンテナの駆動Zoneを算出する。
¥param[in] az 天体を見るためのアンテナAz角 [deg]
*/
void _trkCalZoneType(double az){
  switch(p.antZoneType){
  case 0:
    p.antZone = 0; //!< Auto 最短距離
  case 1:
  case 2:
    p.antZone = 2; //!< CCW
    break;
  case 4:
  case 5:
    p.antZone = 1; //!< CW
    break;
  case 3:
    //if(az >= 270.0 + p.antPosLaLo[0] && az < 360.0){
    if(az >= 270.0 && az < 360.0){
      p.antZone = 2;//!< CCW
    }
    //else if(az >= 0 && az <= 90.0 - p.antPosLaLo[0]){
    else if(az >= 0 && az <= 90.0){
      p.antZone = 1;//!< CW
    }
    else{
      //! 範囲外の場合はantZoneType=0で処理
      p.antZoneType = 0;
      p.antZone = 0;
    }
    break;
  default:
    p.antZone = 0;
    break;
  }
  return;
}



/*! ¥fn void trkSetPof(double* pof, int coord)
¥brief ポインティングオフセットの修正
¥param[in] pof ポインティングオフセットX,Y 2次元配列 [sec]
¥param[in] coord ポインティングオフセット座標の定義 1:RADEC 2:LB 3:AZEL
*/
void trkSetPof(const double* pof, int coord){
  double azel[2];
  //uLock();

  //!< オフセット定義が3(AzEl)かつ天体座標系が3(AzEl)以外ならポインティングオフセットの座標定義は実角
  //p.pofFlg = 1;

  p.pofCoord = coord;
  p.pof[0] = pof[0] * sec2rad;
  p.pof[1] = pof[1] * sec2rad;
  //p.xin.iapflg = p.pofCoord;
  
	const int scanFlag = scanTableGetScanFlag();
	if(p.thrdStatusRun || ( scanFlag != SCANTABLE_FLAG_5 && scanFlag != SCANTABLE_FLAG_9)){
/* 090611
  if(p.thrdStatusRun || ( scanFlag != 2 && scanFlag != 5)){
*/
	//!< ステータス取得モードもしくは、5-Point,9-Point 以外の場合は即オフセットを実行
	calcGetPAZEL(azel);
	uM2("trkSetPof(); PAZEL = %lf %lf [deg]", azel[0], azel[1]);
	uM4("trkSetPof(); Set Pointing Offset=(%.2lf\",%.2lf\") coord=%d(%s)", pof[0], pof[1], coord, errnoCoord(coord));

	calcSetPointOff(coord, p.pof[0], p.pof[1]);//090604 in
/* 090604 out
	//p.xin.iapflg = coord;//081115 out
	if(coord == 1 || coord == 2){
		calcSetXinDpof(coord, p.pof[0], p.pof[1], 0, 0);//081115 in 
	}
	else if(coord == 3){
		calcSetXinDpof(coord, 0, 0, p.pof[0], p.pof[1]);//081115 in 
	}else{//081115 in
		uM1("CAUTION! trkSetPof(); Invalid coord: %d\n", coord);
	}
*/
  }
  else{
    uM4("trkSetPof(); Next SetPattern Set Pointing Offset=(%.2lf\",%.2lf\") coord=%d(%s)",
	pof[0], pof[1], coord, errnoCoord(coord));
  }
  //uM("debug trkSetPof(); Complete");
  //uUnLock();
  return;
}


/*! ¥fn void _trkSetOff()
¥brief 次のオフセット位置を設定する
¥brief オフセットが2点以上ある場合に必要
*/
void _trkSetOff(){
  if(p.curr.offNo < 0 || p.curr.offNo >= scanTableGetOffNumber()){
    p.curr.offNo = 0;
  }
	calcSetSof(scanTableGetOff(p.curr.offNo*2), scanTableGetOffCoord(), scanTableGetPosAngleRad());
//  calcSetSof(&p.seq.off[p.curr.offNo*2], scanTableGetOffCoord(), scanTableGetPosAngleRad());
  p.curr.offNo++;
  return;
}

/*! ¥fn void trkSetSofNext()
¥brief 観測の種類を判別して次の観測点を設定し、観測点の種類を返す。
¥retval 1 End scan set
¥retval 0 Set Next Sof
*/
int trkSetSofNext(int* id){
  double sof[2];
  int mapNo;

  //! OnPoint,5Point,Grid,Random,9Point,rasterは次のスキャン位置の管理をRandomで行う。

	if(p.curr.sofNo >= scanTableGetNumberOfPoints()){//081117 in
//  if(p.curr.sofNo >= p.seq.mapNum){
    //! 1Set完了
    p.curr.sofNo = 0;
    p.curr.offFlg = 0;
    *id = 0;
    return 1;
  }
  else{
		mapNo = scanTableGetPointNumber(p.curr.sofNo);
//    mapNo = p.seq.mapPattern[p.curr.sofNo];
    if(mapNo == 0){
      //! R
      p.curr.sofNo++;
      p.curr.offFlg = 0;
    }
    else if(mapNo > 0){
      //! On-Point
      int onNo = mapNo - 1;
      memcpy(sof, scanTableGetOn(onNo*2), sizeof(double) * 2);//081117 in
//      memcpy(sof, &p.seq.on[onNo*2], sizeof(double) * 2);
      calcSetSof(sof, scanTableGetOnCoord(), scanTableGetPosAngleRad());
      p.curr.sofNo++;
      p.curr.offFlg = 0;

	if(scanTableGetScanFlag() == SCANTABLE_FLAG_RASTER || scanTableGetScanFlag() == SCANTABLE_FLAG_CROSS){//090611 in
/* 090611 out
      if(scanTableGetScanFlag() == 6){
*/
	//! RasterのOn点なら開始点(Approach点の開始点)と終了点を取得し
	//! スキャンオフセットをTrkInterval間隔で計算し全てメモリに格納
	const double a = cos(scanTableGetPosAngleRad());
	const double b = sin(scanTableGetPosAngleRad());
	double sx, sy; //!< rad
	double dx, dy; //!< rad

	//! スタート座標、観測区間をPosAngleを考慮して取得する
	const double onX = scanTableGetOnValue(onNo*2);//081117 in
	const double onY = scanTableGetOnValue(onNo*2+1);
	sx =(  onX * a + onY * b) * sec2rad;
	sy =(- onX * b + onY * a) * sec2rad;
/* 081117 out
	sx =(  p.seq.on[onNo*2] * a + p.seq.on[onNo*2+1] * b) * sec2rad;
	sy =(- p.seq.on[onNo*2] * b + p.seq.on[onNo*2+1] * a) * sec2rad;
*/
	const double onLPX = scanTableGetLinePath(onNo*2);//081117 in
	const double onLPY = scanTableGetLinePath(onNo*2+1);
	dx =   onLPX * a + onLPY * b;
	dy = - onLPX * b + onLPY * a;
	  
	for(int i = 0; i < p.rastSofSize; i++){
	  p.rastSof[i*2]   = sx + dx * (double)i / (double)(p.rastSofSize-1);
	  p.rastSof[i*2+1] = sy + dy * (double)i / (double)(p.rastSofSize-1);
//	  p.rastSof[i*2]   = sx + dx * (double)i / (double)p.rastSofSize;
//	  p.rastSof[i*2+1] = sy + dy * (double)i / (double)p.rastSofSize;
	  //uM3("trkSetSofNext(); rastSof[%d](%.15lf,%.15lf)¥"",
	  //    i, p.rastSof[i*2], p.rastSof[i*2+1]);
	}
 	if(p.rastStart == 0){//080530 in
		p.rastSofNo = 0;
	}//080530 in
      }
    }
    else if(mapNo < 0){
      //! OffPoint -1:Off1 -2:Off2 ....
		if(scanTableGetScanFlag() == SCANTABLE_FLAG_RASTER || scanTableGetScanFlag() == SCANTABLE_FLAG_CROSS){//090611 in
/* 090611 out
      if(scanTableGetScanFlag() == 6){
*/
	p.rastStart = 0; //!< もしラスター中でも終了させる
      }
      p.curr.offNo = - mapNo - 1;
      if(scanTableGetOffMode() != 2){
	//! 相対座標定義のOff点だったらスキャンオフセットに設定
	memcpy(sof, scanTableGetOff(p.curr.offNo*2), sizeof(double) * 2);//081117 in
//	memcpy(sof, &p.seq.off[p.curr.offNo*2], sizeof(double) * 2);
	calcSetSof(sof, scanTableGetOffCoord(), scanTableGetPosAngleRad());
      }
      else{
	memset(sof, 0, sizeof(double) * 2);
	calcSetSof(sof, scanTableGetOffCoord(), scanTableGetPosAngleRad());
      }
      p.curr.offFlg = 1;
      p.curr.sofNo++;
    }
    //uM3("debug trkSetSofNext(); sofNo[%d] mapNum[%d] mapNo[%d]", p.curr.sofNo, p.seq.mapNum, mapNo);
  }

  //! Reset Tracking Status
	_trkCalTrackStatusReset();//081114 in
  //_trkCalTrackStatus(1); //!< Reset Tracking Status

  *id = mapNo;
  return 0;
}

/*! ¥fn int trkSetRastStart(time_t startTime)
¥breif Rasterスキャンを開始する。
¥param[in] startTime 開始時刻を1970年からの経過UT1秒(time(&t)で取得できる値)で指定する。
¥return 0;Success othre:Error
*/
int trkSetRastStart(time_t startTime){
  time_t t;

  //! Tracking状態になっていない場合はエラー
  if(p.trackStat != 0){
    p.thrdRet = TRK_STANDBY_ERR;
    return p.thrdRet;
  }

  if(scanTableGetScanFlag() != SCANTABLE_FLAG_RASTER && scanTableGetScanFlag() != SCANTABLE_FLAG_CROSS){//090611 in
/* 090611 out
  if(scanTableGetScanFlag() != 6){
*/
    p.thrdRet = TRK_SET_ERR;
    return TRK_SET_ERR;
  }

  if(p.rastStart == 1){
    uM("trkSetRastStart(); Already set");
    p.thrdRet = TRK_SET_ERR;
    return p.thrdRet;
  }

  time(&t);
  if(t > startTime){
    uM("trkSetRastStart(); Already passed startTime error!!");
    p.thrdRet = TRK_SET_ERR;
    return p.thrdRet;
  }

  //! RastStartの設定
  p.rastStartTime = startTime;
  p.rastStart = 1;
//	uM("trkSetRastStart(); success. ");//090611 in for test.

  return 0;
}

/*! ¥fn int trkSetXY(int coord, double* xy)
¥brief Lee Trackingの時などにアンテナの座標を随時設定する。
¥param[in] coord 1:RADEC 2:LB 3:AZEL
¥param[in] xy 座標位置(X,Y) rad
¥reutrn 0:Success other:Error
*/
int trkSetXY(int coord, const double* xy){
  uM3("trkSetXY(); coord[%d] XY[%lf, %lf] rad", coord, xy[0], xy[1]);
	const int scanFlag=scanTableGetScanFlag();
	if(scanFlag != SCANTABLE_FLAG_LEE){//090611 in
/* 090611 ouot
  if(scanFlag != 1002){
*/
    uM1("trkSetXY(); ScanFlag[%d] error!!", scanFlag);
    return TRK_SET_ERR;
  }


	return celestialObjectSetXY(coord, xy);//081115 in
/* 081115 out
  if(coord < 1 || coord > 3){
    uM("trkSetXY(); invalid error!!");
    return TRK_SET_ERR;
  }
  else if(xy[0] < 0.0 || xy[0] > 2.0 * PI || xy[1] < - PI / 2.0 || xy[1] > PI / 2.0){
    uM("trkSetXY(); invalid xy error!!");
    return TRK_SET_ERR;
  }

  p.celObj.iaoflg = coord;
  memcpy(p.celObj.dsourc, xy, sizeof(*p.celObj.dsourc) * 2);

  return 0;
*/

}

/*! ¥fn void trkGetXY(int* coord, double* xy)
¥brief 現在の観測位置をcelestialObjectGetCoordinate()(天体定義時のCoordinate)に従って返す。
¥brief 惑星はAZELを返すこととする。
¥param[out] coord 定義座標系。 1:RADEC 2:LB 3:AZEL 0:Error
¥param[out] xy 座標double[2] [deg]
*/
void trkGetXY(int* coord, double* xy){
	const int flg = celestialObjectGetCoordinate();
	switch(flg) {//081115 in
	case 0:
		//! 惑星 AZEL
		calcGetAZEL(xy);
		*coord = 3;
		break;
	case 1:
		//! RADEC
		calcGetRADEC(xy);
		*coord = 1;
		break;
	case 2:
		//! LB
		calcGetLB(xy);
		*coord = 2;
		break;
	case 3:
		//! AZEL
		calcGetAZEL(xy);
		*coord = 3;
		break;
	default:
		//! Error
		uM1("CAUTION! Invalid flg: %d\n", flg);
		memset(xy, 0, sizeof(double) * 2);
		*coord = 0;
		return;//since xy[0] and xy[1] are 0.
	}

/* 081115 out
  if(p.celObj.iaoflg == 0){
    //! 惑星 AZEL
    calcGetAZEL(xy);
    *coord = 3;
  }
  else if(p.celObj.iaoflg == 1){
    //! RADEC
    trkGetRADEC(xy);
    *coord = 1;
  }
  else if(p.celObj.iaoflg == 2){
    //! LB
    trkGetLB(xy);
    *coord = 2;
  }
  else if(p.celObj.iaoflg == 3){
    //! AZEL
    calcGetAZEL(xy);
    *coord = 3;
  }
  else{
    //! Error
    memset(xy, 0, sizeof(double) * 2);
    *coord = 0;
  }
*/
  xy[0] *= 180.0 / PI;
  xy[1] *= 180.0 / PI;
  return;
}

/*! ¥fn void trkGetPZONE(int* PZONE)
¥brief アンテナZoneの指示角
¥param[out] PZONE 0:最小駆動角方向 1:CW 2:CCW
*/
void trkGetPZONE(int* PZONE){
  *PZONE = p.antZone;
  return;
}


/*! ¥fn void trkGetEAZEL(double* EAZEL)
¥brief アンテナ指示角と実際角の誤差EAZELの取得
¥param[out] EAZEL (=RAZEL-PAZEL) [rad]
*/
void trkGetEAZEL(double* EAZEL){
  memcpy(EAZEL, p.deazel, sizeof(double)*2);
  return;
}


/*! ¥fn void trkGetTrackStatus(int* status)
¥brief 追尾状態の取得
¥param[out] status ビットフラグによって表現 0:Tracking -1:az角がSwing -2:el角がSwing -4:Zone移動を含むSwing
*/
int trkGetTrackStatus(){
	return p.trackStat;
//void trkGetTrackStatus(int* status){
//  *status = p.trackStat;
//  return;
}


/*! ¥fn void _trkCalZoneTypeInit()
¥brief アンテナのZoneTypeを設定する。ZoneTypeは0〜5を下記のように定義する
¥brief Map Center Decが必要なため必ずtrkCal()を行った後に呼ぶこと。
¥brief [0] アンテナ移動角が最短距離になるようなZoneを使用
¥brief [1] 0<=antAz<=90-(antLatitude) (Zone = CCW)
¥brief [2] 90-(antLatitude)<antAz<270+(antLatitude) (Zone = CCW)
¥brief [3] 270+(antLatitude)<=antAz<360 (Zone = CCW)
¥brief     or 0<=antAz<=90-(antLatitude) (Zone = CW)
¥brief [4] 90-(antLatitude)<antAz<270+(Antenna Latitude) (Zone = CW)
¥brief [5] 270+(antLatitude)<=antAz<360 (Zone = CW)
*/
void _trkCalZoneTypeInit(){
  double dec;
  double la;
  //double az;

  p.antZoneType=0;
  p.antZone=0;
	const int flg = celestialObjectGetCoordinate();
  if(flg==1 || flg == 2){
    //! 天体の座標系にRADEC,LBを使用していた場合のみ適応
	dec = calcGetMapCenterDec();
    //dec=p.dgtdat[1][1]; //! MapCenter DEC rad
    //la=p.antPosLaLo[0];
	la = calcGetAntennaLat();
    //la=p.dgtdat[0][1]; //! Antenna Latitude rad
    if(la<=dec){
      p.antZoneType=3;
    }
    else{
      p.antZoneType = 0;
      //if(p.acuStat.zone == 2){ //!< CCW
      //p.antZoneType=2;
      //}
      //else{
      //p.antZoneType=4;
      //}
    }
  }
  uM1("_trkCalZoneTypeInit(); Set antZoneType=%d", p.antZoneType);
  return;
}

/*! ¥fn void setParam()
¥brief パラメーター情報を取得する
* thread 1, phase C0
*/
void setParam(){
  //int i;
  //char tmp[16];


  if(confSetKey("TrkInterval"))
    p.TrkInterval = atof(confGetVal());
  if(confSetKey("AntAzMinimum"))
    p.dazelmin[0] = atof(confGetVal());
  if(confSetKey("AntElMinimum"))
    p.dazelmin[1] = atof(confGetVal());
  if(confSetKey("AntAzMaximum"))
    p.dazelmax[0] = atof(confGetVal());
  if(confSetKey("AntElMaximum"))
    p.dazelmax[1] = atof(confGetVal());
  if(confSetKey("AntAzMarg"))
    p.AntAzElMarg[0] = atof(confGetVal());
  if(confSetKey("AntElMarg"))
    p.AntAzElMarg[1] = atof(confGetVal());
  if(1)
    p.AntAzElMargThrsd = 3; //!< アンテナがトラッキングしていると判断する許容誤差に入った回数の閾値


//  if(confSetKey("TrkTimeOffset"))
//    p.TrkTimeOffset = atof(confGetVal());
  
  //if(confSetKey("RSkyInterval"))
  //  p.RSkyInterval = atoi(confGetVal());
  //if(confSetKey("Interval"))
  //  p.RSkyInterval = atoi(confGetVal());//!< 互換性のため当面設定しておく。

/*
  //! On-Point
  if(confSetKey("ScanFlag"))
    p.scan.ScanFlag = atoi(confGetVal());
  if(confSetKey("OnCoord"))
    p.scan.OnCoord = atoi(confGetVal());
  if(confSetKey("PosAngle_Rad"))
    p.scan.posAngleRad = atof(confGetVal());
  if(confSetKey("LineTime"))
    p.scan.LineTime = atof(confGetVal());
  //if(confSetKey("LinePathX_Rad"))
  //  p.LinePathX_Rad = atof(confGetVal());
  //if(confSetKey("LinePathY_Rad"))
  //  p.LinePathY_Rad = atof(confGetVal());

  if(confSetKey("SeqPattern")){ //!< 互換性のため残しておく
    if(p.scan.SetPattern){
      free(p.scan.SetPattern);
    }
    p.scan.SetPattern = (char*) malloc(strlen(confGetVal()) + 1);
    strcpy(p.scan.SetPattern, confGetVal());
  }
  if(confSetKey("SetPattern")){
    if(p.scan.SetPattern){
      free(p.scan.SetPattern);
    }
    p.scan.SetPattern = (char*) malloc(strlen(confGetVal()) + 1);
    strcpy(p.scan.SetPattern, confGetVal());
  }
  if(confSetKey("OnNumber")){
    p.scan.OnNumber = atoi(confGetVal());
    if(p.scan.OnNumber < 0){
      uM1("trk setParam(); invalid  p.scan.OnNumber[%d] < 0 ans set 0", p.scan.OnNumber);
      p.scan.OnNumber = 0;
    }
    else{
      if(p.seq.on){
	free(p.seq.on);
      }
      if(p.seq.linePath){
	free(p.seq.linePath);
      }
      p.seq.on = (double*)malloc(sizeof(p.seq.on[0]) * p.scan.OnNumber * 2);
      p.seq.linePath = (double*)malloc(sizeof(p.seq.linePath[0]) * p.scan.OnNumber * 2);
      memset(p.seq.on, 0, sizeof(p.seq.on[0]) * p.scan.OnNumber * 2);
      memset(p.seq.linePath, 0, sizeof(p.seq.linePath[0]) * p.scan.OnNumber * 2);
      for(i = 0; i < p.scan.OnNumber; i++){
	sprintf(tmp, "OnX%d_Rad", i+1);
	if(confSetKey(tmp)){
	  p.seq.on[i*2] = atof(confGetVal()) * rad2sec;
	}
	sprintf(tmp, "OnY%d_Rad", i+1);
	if(confSetKey(tmp)){
	  p.seq.on[i*2+1] = atof(confGetVal()) * rad2sec;
	}
	sprintf(tmp, "LinePathX%d_Rad", i+1);
	if(confSetKey(tmp)){
	  p.seq.linePath[i*2] = atof(confGetVal());
	}
	sprintf(tmp, "LinePathY%d_Rad", i+1);
	if(confSetKey(tmp)){
	  p.seq.linePath[i*2+1] = atof(confGetVal());
	}
      }
    }
  }

  //! Off-Point
  if(confSetKey("OffCoord"))
    p.scan.OffCoord = atoi(confGetVal());
  if(confSetKey("OffNumber")){
    p.scan.OffNumber = atoi(confGetVal());
    if(p.scan.OffNumber < 0){
      uM1("trk setParam(); invalid p.scan.OffNumber[%d] < 0 ans set 0", p.scan.OffNumber);
      p.scan.OffNumber = 0;
    }
    else{
      if(p.seq.off){
	free(p.seq.off);
      }
      p.seq.off = (double*)malloc(sizeof(p.seq.off[0]) * p.scan.OffNumber * 2);
      memset(p.seq.off, 0, sizeof(p.seq.off[0]) * p.scan.OffNumber * 2);
      for(i = 0; i < p.scan.OffNumber; i++){
	sprintf(tmp, "OffX%d_Rad", i+1);
	if(confSetKey(tmp)){
	  p.seq.off[i*2] = atof(confGetVal()) * rad2sec;
	}
	sprintf(tmp, "OffY%d_Rad", i+1);
	if(confSetKey(tmp)){
	  p.seq.off[i*2+1] = atof(confGetVal()) * rad2sec;
	}
      }
    }
  }
  if(confSetKey("OffInterval"))
    p.scan.OffInterval = atoi(confGetVal());
  if(confSetKey("OffMode"))
    p.scan.OffMode = atoi(confGetVal());
*/

  //! Source
/* 081115 moved to celectialObject.cpp
  if(confSetKey("SourceFlag"))
    p.celObj.ioflg = atoi(confGetVal());  //!< 天体フラグ 0:太陽系外 1-9:惑星 10:月 11:太陽 12:COMET 99:AZEL
  if(confSetKey("Coordinate"))
    p.celObj.iaoflg = atoi(confGetVal()); //!< 位置座標系 0:惑星 1:RADEC 2:LB 3:AZEL
  if(confSetKey("X_Rad"))
    p.celObj.dsourc[0] = atof(confGetVal());
  if(confSetKey("Y_Rad"))
    p.celObj.dsourc[1] = atof(confGetVal());
  if(confSetKey("Epoch"))
    p.celObj.ibjflg = atoi(confGetVal()); //!< 分点フラグ 1:B1950 2:J2000
  if(confSetKey("Velocity"))
    p.celObj.drdlv = atof(confGetVal());
*/

  //if(p.celObj.ioflg != 0 || p.celObj.ioflg != 99){                 //!< 位置座標系 0:惑星 1:RADEC 2:LB 3:AZEL
  //  p.celObj.iaoflg = 0;
  //}

//  for(int i = 0; i < 6; i++){//081114 moved to calc.cpp
//    int j = i * 5;
//    if(sscanf(antInst[i+1], "%lf %lf %lf %lf %lf",
//	      &p.P[j], &p.P[j+1], &p.P[j+2], &p.P[j+3], &p.P[j+4]) != 5){
//      uM2("setParam(); antInst[%d] (%s) error!!", i+1, antInst[i+1]);
//    }
//  }
}

/*! ¥fn int checkParam()
¥brief パラメーターのチェック
¥retval 0 成功
¥retval -1 失敗
* thread 1, phase C0
*/
int checkParam(){
	if(controlBoardGetAntenna() == CONTROL_BOARD_USE_NO){//090611 in
/* 090611 out
	if(p.TrkUse == 0)//081115 in
*/
		return 0;
	}
//  if(p.TrkUse != 0){//081115 out
    //! ACU関連
    if(p.TrkInterval < 0){
      uM1("TrkInterval(%lf) error!!", p.TrkInterval);
      return -1;
    }
/*
    if(p.dazelmin[0] < 0 || p.dazelmin[0] > 360){
      uM1("AntAzMinimum(%lf) error!!", p.dazelmin[0]);
      return -1;
    }
    if(p.dazelmin[1] < 0 || p.dazelmin[1] > 90){
      uM1("AntElMinimum(%lf) error!!", p.dazelmin[1]);
      return -1;
    }
    if(p.dazelmax[0] < p.dazelmin[0] || p.dazelmax[0] > 360){
      uM1("AntAzMaximum(%lf) error!!", p.dazelmax[0]);
      return -1;
    }
    if(p.dazelmax[1] < p.dazelmin[1] || p.dazelmax[1] > 90){
      uM1("AntElMaximum(%lf) error!!", p.dazelmax[1]);
      return -1;
    }
*/
    if(p.AntAzElMarg[0] < 0){
      uM1("AntAzMarg(%lf) error!!", p.AntAzElMarg[0]);
      return -1;
    }
    if(p.AntAzElMarg[1] < 0){
      uM1("AntElMarg(%lf) error!!", p.AntAzElMarg[1]);
      return -1;
    }

/* 081115 moved to celectialObject.cpp
    //! 観測関連
    if(p.celObj.ioflg < 0 || (p.celObj.ioflg > 12 && p.celObj.ioflg != 99)){
      uM1("SourceFlag(%d) error!!", p.celObj.ioflg);
      return -1;
    }
    if(p.celObj.iaoflg < 0 || p.celObj.iaoflg > 3){
      uM1("Coorinate(%d) error!!", p.celObj.iaoflg);
      return -1;
    }
    if(p.celObj.ibjflg < 1 || p.celObj.ibjflg > 2){
      uM1("Epoch(%d) error!!", p.celObj.ibjflg);
      return -1;
    }
    if(!((p.scan.ScanFlag >= 1 && p.scan.ScanFlag <= 6)
	 || (p.scan.ScanFlag >= 1001 && p.scan.ScanFlag <= 1002))){
      uM1("ScanFlag(%d) error!!", p.scan.ScanFlag);
      return -1;
    }
    if(p.celObj.dsourc[0] < 0.0 || p.celObj.dsourc[0] > 2.0*PI){
      uM1("X_Rad(%lf) error!!", p.celObj.dsourc[0]);
      return -1;
    }
    if(p.celObj.dsourc[1] < -PI/2.0 || p.celObj.dsourc[1] > PI/2.0){
      uM1("Y_Rad(%lf) error!!", p.celObj.dsourc[1]);
      return -1;
    }
*/
/*
    if(!((p.scan.ScanFlag >= 1 && p.scan.ScanFlag <= 6)
	 || (p.scan.ScanFlag >= 1001 && p.scan.ScanFlag <= 1002))){
      uM1("ScanFlag(%d) error!!", p.scan.ScanFlag);
      return -1;
    }

    if(p.scan.ScanFlag >= 1 && p.scan.ScanFlag <= 6){
      if(p.scan.OffNumber < 1 && p.scan.OffNumber > 2){
	uM1("checkParam(); OffNumber %d error!!.", p.scan.OffNumber);
	return -1;
      }
      if(p.scan.ScanFlag == 1 && p.scan.ScanFlag == 3 && p.scan.ScanFlag == 4){
	//! On,Grid,Random
	if(p.scan.OffInterval <= 0){
	  uM1("checkParam(); OffInterval[%d] error!!", p.scan.OffInterval);
	  return -1;
	}
      }
      if(!p.scan.SetPattern){
	uM1("ScanFlag(%d) SetPattern error!!", p.scan.ScanFlag);
	return -1;
      }
      if(p.scan.OnNumber <= 0){
	uM2("ScanFlag(%d) OnNumber(%d) error!!", p.scan.ScanFlag, p.scan.OnNumber);
	return -1;
      }
      if(p.scan.OffCoord < 1 || p.scan.OffCoord > 3){
	uM1("OffCoord(%d) error!!", p.scan.OffCoord);
	return -1;
      }
    }
    else if(p.scan.ScanFlag == 1001){
      //! Source Tracking
    }
    if(p.scan.LineTime < 0){
      uM1("LineTime(%lf) error!!", p.scan.LineTime);
      return -1;
    }
*/
//  }
  return 0;
}

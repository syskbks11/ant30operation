#ifndef INCLUDE_GUARD_UUID_18d0718c_4515_45eb_b87c_d703f9bb262e
#define INCLUDE_GUARD_UUID_18d0718c_4515_45eb_b87c_d703f9bb262e
/*!
¥file if.cpp
¥author Y.Koide
¥date 2006.11.28
¥brief IF制御用
* 
* Changed by NAGAI Makoto
* for 32-m telescope & 30-cm telescope
* 
* Changed by Enohi Rina
* 2輝線同時観測＋2偏波化
*
*/
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <trk.h>
#include "configuration.h"
#include "celestialObject.h"
//#include "trk45Sub.h"
#include "errno.h"
#include "if.h"
#include "ifCalcBoth.h"
#include "controlBoard.h"
#include "../signalG/penguin_signalG.h" //20250121

/* Constants used in ifRepeat() */
static const double _LO1_FREQ_THRESH = 1.0e-6; //!< 許容誤差[GHz]
static const double _LO1_AMP_THRESH = 0.01;    //!< 許容誤差[dB]
static const double _LO1_ATT_THRESH = 0.01;      //!< 許容誤差[dB]
static const double _LO1_ATT2_THRESH = 0.01;      //!< 許容誤差[dB]  //20240810
static const double _LO2_FREQ_THRESH = 1.0e-9; //!< 許容誤差[GHz]
static const double _LO2_AMP_THRESH = 0.01;    //!< 許容誤差[dB]

//! 1stIFへのリクエスト構造体
typedef struct sReqIf1{
	double lo1freq;    //!< 1st Local Frequency [GHz]
	double lo1amp;     //!< 1st Local Amplitude [dBm] ant30.confで設定。固定値
}tReqIf1;

//! 2ndIFへのリクエスト構造体 ant30.confで設定。固定値
typedef struct sReqIf2{
    double lo2freq;    //!< 2nd Local Frequency [GHz]
    double lo2amp;     //!< 2nd Local Amplitude [dBm]
    double att;       //!< CO 2nd IF Step Att [-dB] H_pol
    double att2;      //!< CI 2nd IF Step Att [-dB] H_pol //20240810
    double att3;      //!< CO 2nd IF Step Att [-dB] V_pol //20241126
    double att4;      //!< CI 2nd IF Step Att [-dB] V_pol //20241126
}tReqIf2;

//! 1stIFからの応答構造体
typedef struct sAnsIf1{
  double ansLo1Freq;    //!< [GHz]
  double ansLo1Amp;     //!< [dBm]
}tAnsIf1;


//! 2ndIFからの応答構造体 Lo2Freq, Lo2Ampは固定値なので制御しない。ここにステップアッテネーターを追加
typedef struct sAnsIf2{
    double ansLo2Freq;    //!< [GHz]
    double ansLo2Amp;     //!< [dBm]
    double ansAtt1;       //!< [-dB] CO H_pol
    double ansAtt2;       //!< [-dB] CI H_pol//20240810
    double ansAtt3;       //!< [-dB] CO V_pol//20241126
    double ansAtt4;       //!< [-dB] CI V_pol//20241126
}tAnsIf2;


typedef struct sParamIf{
  //! 変数
  tmClass_t* vtm;            //!< 更新時間の管理
  netClass_t* net1;
  netClass_t* net2;
  int OnOffR;           //!< 現在観測対象 0:On 1:Off 2:R
  //! 観測パラメータ
	tReqIf1 req1;//!< IF1への送信データ
	tReqIf2 req2;//!< IF2への送信データ

 // .device で設定
  double IfAtt_1;        //!< CO StepAtt at OnOff-Point [dB]  //20240810 H_pol
  double IfAttDecR_1;    //!< CO StepAtt at R [dB]            //20240810 H_pol
  double IfAtt_2;        //!< CI StepAtt at OnOff-Point [dB]  //20240810 H_pol
  double IfAttDecR_2;    //!< CI StepAtt at R [dB]            //20240810 H_pol
  double IfAtt_3;        //!< CO StepAtt at OnOff-Point [dB]  //20241126 V_pol
  double IfAttDecR_3;    //!< CO StepAtt at R [dB]            //20241126 V_pol
  double IfAtt_4;        //!< CI StepAtt at OnOff-Point [dB]  //20241126 V_pol
  double IfAttDecR_4;    //!< CI StepAtt at R [dB]            //20241126 V_pol
  double Fif_1;          //!< CO 第一IF(Left)出力の信号中心周波数 [GHz] //20240810
  double Fif_2;          //!< CI 第一IF(Left)出力の信号中心周波数 [GHz] //20240810
  double lo2_1;          //!< CO 2nd Local //20240810 H_pol 
  double lo2_2;          //!< CI 2nd Local //20240810 H_pol
  double lo2_3;          //!< CO 2nd Local //20241126 V_pol
  double lo2_4;          //!< CI 2nd Local //20241126 V_pol
  double fcenter;       //!< 観測する周波数Frf [Hz]
  //double safcenter;     //!< 分光計の中心周波数 [Hz]
  int  if1Interval;     //!< IF制御を更新する時間間隔 [sec]
//  double trkTimeOffset; //!< 何秒後の追尾計算を行うか [sec]

  //! 追尾ライブラリ用パラメータ
/* 081121 out
  int ioflg;            //!< 天体フラグ 0:太陽系外 1-11:惑星 12:COMET 99:AZEL
  int iaoflg;           //!< 位置座標系 0:惑星 1:RADEC 2:LB 3:AZEL
  int ibjflg;           //!< 分点フラグ 1:B1950 2:J2000
  double drdlv;         //!< 天体の視線速度 [m/s]
  double dsourc[2];     //!< 天体位置 [rad]
*/

  //! 各種変数
//  XTRKI xin;            //!< オフセット設定
  //double doff[2];       //!< OFF点用
//  char cstart[24];      //!< 観測時刻の設定 YYYYMMDDhhmmss.0 (JST)

//  XTRKO xout;           //!< アンテナ速度などDAZEL
//  double dvrad;         //!< trk_velo用変数
  tAnsIf1 aIf1;         //!< IF1からの応答データ
  tAnsIf2 aIf2;         //!< IF2からの応答データ

//  int dayFlg;           //!< trk_00()を呼び出すタイミング用。UTにおける日付が変わったとき呼ぶ。
}tParamIf;


static int start=0;
static tParamIf p;

/* moved to calc2.cpp
static int _ifCal();
*/
static int _ifNet();

static void setParam();
static int checkParam();

/*! ¥fn int ifInit()
¥retval 0 成功
¥retval 0以外 エラーコード
* thread 1, phase A0
*/
int ifInit(){
  //char cerr[256];
	char if1ip[256];      //!< 1stIF IP
	char if2ip[256];      //!< 2ndIF IP
	int  if1port, if2port;
  memset(&p, 0, sizeof(p));
  start = 0;

  //! 制御プログラムへの接続パラメータのみ取得
/* 090611 out
  if(confSetKey("IfUse01"))
    p.use1 = atoi(confGetVal());
  if(confSetKey("IfUse02"))
    p.use2 = atoi(confGetVal());
*/
  if(confSetKey("IfIp01"))
    strcpy(if1ip, confGetVal());
  if(confSetKey("IfIp02"))
    strcpy(if2ip, confGetVal());
  if(confSetKey("IfPort01"))
    if1port = atoi(confGetVal());
  if(confSetKey("IfPort02"))
    if2port = atoi(confGetVal());

	if(controlBoardGet1stIF() == CONTROL_BOARD_USE_FULL){//090611 in
		if(!confSetKey("IfIp01")){
			uM("ifInit(); ERROR: There is no IfIp01 key in conf.");//090611 in
/* 090611 out
      uM1("IfUse01(%d) IfIp01() error!!¥n", p.use1); 
*/
			return -1;
		}
		if(if1port <= 0){
			uM1("ifInit(); ERROR: Invalid IfPort01 value (%d).", if1port);//090611 in
/* 090611 out
      uM2("IfUse01(%d) IfPort01(%d) error!!¥n", p.use1, if1port);
*/
			return -1;
		}
	}
	if(controlBoardGet2ndIF() == CONTROL_BOARD_USE_FULL){
		if(!confSetKey("IfIp02")){
			uM("ifInit(); ERROR: There is no IfIp02 key in conf.");//090611 in
/* 090611 out
      uM1("IfUse02(%d) IfIp02() error!!¥n", p.use2);
*/
			return -1;
		}
		if(if2port <= 0){
			uM1("ifInit(); ERROR: Invalid IfPort02 value (%d).", if2port);//090611 in
/* 090611 out
      uM2("IfUse02(%d) IfPort02(%d) error!!¥n", p.use2, if2port);
*/
			return -1;
		}
	}
  
	if(controlBoardGet1stIF() == CONTROL_BOARD_USE_FULL){//090611 in
    p.net1 = netclInit(if1ip, if1port);
    if(p.net1 == NULL){
      uM("ifInit(); netclInit(); if2 error");
      return IF_NET_ERR;
    }
  }
	if(controlBoardGet2ndIF() == CONTROL_BOARD_USE_FULL){//090611 in
    p.net2 = netclInit(if2ip, if2port);
    if(p.net2 == NULL){
      uM("ifInit(); netclInit(); if2 error");
      return IF_NET_ERR;
    }
  }
  p.vtm = tmInit();

  return 0;
}

/*! ¥fn int ifStart()
¥brief 新しい観測を行う際に最初に呼び出す。
¥brief 観測パラメータの初期化を行う。
* thread 1, phase C0
*/
int ifStart(){

  //! 観測パラメータの初期化
  p.req1.lo1amp = 0;
  p.req2.lo2amp = 0;
//20241126
  p.IfAtt_1 = 0;
  p.IfAttDecR_1 = 0;
  p.IfAtt_2 = 0;
  p.IfAttDecR_2 = 0;
  p.IfAtt_3 = 0;
  p.IfAttDecR_3 = 0;
  p.IfAtt_4 = 0;
  p.IfAttDecR_4 = 0;
  p.Fif_1 = 0;
  p.Fif_2 = 0;
  p.lo2_1 = 0;
  p.lo2_2 = 0;
  p.lo2_3 = 0;
  p.lo2_4 = 0;

  p.fcenter = 0;
  p.if1Interval = 0;
/* 081121 out
  p.trkTimeOffset = 0;
  p.ioflg = 0;
  p.iaoflg = 0;
  p.ibjflg = 0;
  p.drdlv = 0;
  memset(p.dsourc, 0, sizeof(p.dsourc[0]) * 2);
  memset(&p.xin, 0, sizeof(p.xin));
*/
  
  //! 各種変数初期化
  start = 0;
/*
  memset(p.cstart, 0, sizeof(p.cstart[0]) * 24);
  memset(&p.xout, 0, sizeof(p.xout));
  p.dvrad = 0;
*/
  p.req1.lo1freq = 0;
  p.req2.lo2freq = 0;
  memset(&p.aIf1, 0, sizeof(p.aIf1));
  memset(&p.aIf2, 0, sizeof(p.aIf2));
  p.req2.att = 0;
  p.req2.att2 = 0; //20240810
  //! 観測パラメータをセットする
  setParam();
  if(checkParam()){
    uM("ifInit(); checkParam(); error");
    return IF_PARAM_ERR;
  }

  //! 追尾ライブラリの初期化
/* 090604
	calc2Start();
*/
/* 081121 out
  tmReset(p.vtm);
  strcpy(p.cstart, tmGetTimeStr(p.vtm, p.trkTimeOffset));
  if(trk_00(p.cstart, "", cerr) != 0) {
    uM1("ifStart(); %s",cerr);
    return IF_TRK00_ERR;
  }
  p.dayFlg = 1;
*/ 
  return 0;
}


/*! ¥fn int ifEnd()
¥brief 終了時に呼び出す
¥retval 0 成功
* thread 1, phase A2
*/
int ifEnd(){
  if(p.vtm){
    tmEnd(p.vtm);
    p.vtm = NULL;
  }
  if(p.net1){
    netclEnd(p.net1);
    p.net1 = NULL;
  }
  if(p.net2){
    netclEnd(p.net2);
    p.net2 = NULL;
  }
/* 090604 out
	calc2End();
*/
  return 0;
}

/*! ¥fn int ifRepeat()
¥brief IF制御を行う。ステップアッテネーターの設定のみ。
¥retval 0 成功
¥retval 0以外 エラーコード
*/
int ifRepeat(){
  //char tmp[1024];
  int ret;

	if(controlBoardGet2ndIF() == CONTROL_BOARD_USE_NO){//090611 in
		return IF_NOT_USE;
	}
  
  //! 制御プログラムへ送信
  ret = _ifNet();
  if(ret){
    uM1("ifRepeat(); _ifNet(); return [%d] error!!", ret);
    return ret;
  }

  //! 指令値どおりに設定されているか確認
//ローカル周波数の更新はifRepeat2()へ移動　20250204
/*	if(controlBoardGet1stIF() == CONTROL_BOARD_USE_FULL){//090611 in
    if(fabs(p.aIf1.ansLo1Freq - p.req1.lo1freq) >= _LO1_FREQ_THRESH){
      uM2("ifRepeat(); 1st Lo Freq req[%.10lf] ans[%.10lf] error!!",
	  p.req1.lo1freq, p.aIf1.ansLo1Freq);
      ret = IF_STATE_ERR;
    }
    if(fabs(p.aIf1.ansLo1Amp - p.req1.lo1amp) >= _LO1_AMP_THRESH){
      uM2("ifRepeat(); 1st Lo Amp req[%.2lf] ans[%.2lf] error!!",
	  p.req1.lo1amp, p.aIf1.ansLo1Amp);
      ret = IF_STATE_ERR;
    }
  }*/
if(controlBoardGet2ndIF() == CONTROL_BOARD_USE_FULL){
      if(fabs(p.aIf2.ansAtt1 - p.req2.att) >= _LO1_ATT_THRESH){
      uM2("ifRepeat(); StepAtt req[%.2lf] ans[%.2lf] error!!",
          p.aIf2.ansAtt1, p.req2.att);
      ret = IF_STATE_ERR;
    }
    if(fabs(p.aIf2.ansAtt2 - p.req2.att2) >= _LO1_ATT_THRESH){
      uM2("ifRepeat(); StepAtt2 req[%.2lf] ans[%.2lf] error!!",
          p.aIf2.ansAtt2, p.req2.att2);
      ret = IF_STATE_ERR;
    }
    if(fabs(p.aIf2.ansAtt3 - p.req2.att3) >= _LO1_ATT_THRESH){
      uM2("ifRepeat(); StepAtt3 req[%.2lf] ans[%.2lf] error!!",
          p.aIf2.ansAtt3, p.req2.att3);
      ret = IF_STATE_ERR;
    }
    if(fabs(p.aIf2.ansAtt4 - p.req2.att4) >= _LO1_ATT_THRESH){
      uM2("ifRepeat(); StepAtt4 req[%.2lf] ans[%.2lf] error!!",
          p.aIf2.ansAtt4, p.req2.att4);
      ret = IF_STATE_ERR;
    }
   // if(fabs(p.aIf2.ansLo2Freq - p.req2.lo2freq) >= _LO2_FREQ_THRESH){
   //   uM2("ifRepeat(); 2nd Lo Freq req[%.10lf] ans[%.10lf] error!!",
   //	  p.req2.lo2freq, p.aIf2.ansLo2Freq);
   //   ret = IF_STATE_ERR;
   // }
   // if(fabs(p.aIf2.ansLo2Amp - p.req2.lo2amp) >= _LO2_AMP_THRESH){
   //   uM2("ifRepeat(); 2nd Lo Amp req[%.2lf] ans[%.2lf] error!!",
   //	  p.req2.lo2amp, p.aIf2.ansLo2Amp);
   //   ret = IF_STATE_ERR;
   // }
  }

  return ret;
}


/*! ¥fn int ifRepeat2()
¥brief IF制御を行う。ローカル周波数の更新のみ。
¥retval 0 成功
¥retval 0以外 エラーコード
*/
/*int ifRepeat2(){
  //char tmp[1024];
  int ret;

        if(controlBoardGet1stIF() == CONTROL_BOARD_USE_NO){//090611 in
                return IF_NOT_USE;
        }

  //! 制御プログラムへ送信
  ret = _ifNet();
  if(ret){
    uM1("ifRepeat2(); _ifNet(); return [%d] error!!", ret);
    return ret;
  }

  //! 指令値どおりに設定されているか確認
        if(controlBoardGet1stIF() == CONTROL_BOARD_USE_FULL){//090611 in
    if(fabs(p.aIf1.ansLo1Freq - p.req1.lo1freq) >= _LO1_FREQ_THRESH){
      uM2("ifRepeat2(); 1st Lo Freq req[%.10lf] ans[%.10lf] error!!",
          p.req1.lo1freq, p.aIf1.ansLo1Freq);
      ret = IF_STATE_ERR;
    }
    if(fabs(p.aIf1.ansLo1Amp - p.req1.lo1amp) >= _LO1_AMP_THRESH){
      uM2("ifRepeat2(); 1st Lo Amp req[%.2lf] ans[%.2lf] error!!",
          p.req1.lo1amp, p.aIf1.ansLo1Amp);
      ret = IF_STATE_ERR;
    }
  }
  return ret;
}*/

//成功すまで繰り返す。30秒を超えるとタイムアウト　20250204
#include <time.h>  // clock(), CLOCKS_PER_SEC

int ifRepeat2() {
    int ret;
    const int sleepTime = 1000; // 100ms待機
    const double timeoutSec = 30.0; // タイムアウト30秒
    clock_t startTime = clock(); // 開始時刻記録

    if (controlBoardGet1stIF() == CONTROL_BOARD_USE_NO) { // 090611 in
        return IF_NOT_USE;
    }

    while (1) { // 無限ループで成功するまでリトライ
        //! 制御プログラムへ送信
        ret = _ifNet();
        if (ret) {
            uM1("ifRepeat2(); _ifNet(); return [%d] error!! Retrying...", ret);
        }

        //! 指令値どおりに設定されているか確認
        if (controlBoardGet1stIF() == CONTROL_BOARD_USE_FULL) { // 090611 in
            if (fabs(p.aIf1.ansLo1Freq - p.req1.lo1freq) >= _LO1_FREQ_THRESH) {
                //uM2("ifRepeat2(); 1st Lo Freq req[%.10lf] ans[%.10lf] error!! Retrying...",
                //    p.req1.lo1freq, p.aIf1.ansLo1Freq);
                ret = IF_STATE_ERR;
            }
            if (fabs(p.aIf1.ansLo1Amp - p.req1.lo1amp) >= _LO1_AMP_THRESH) {
                //uM2("ifRepeat2(); 1st Lo Amp req[%.2lf] ans[%.2lf] error!! Retrying...",
                //    p.req1.lo1amp, p.aIf1.ansLo1Amp);
                ret = IF_STATE_ERR;
            }
        }

        if (ret == 0) {
            return 0; // 成功
        }

        // タイムアウトチェック
        double elapsedSec = (double)(clock() - startTime) / CLOCKS_PER_SEC;
        if (elapsedSec >= timeoutSec) {
            uM("ifRepeat2(); Timeout after 30 seconds.");
            return ret; // タイムアウトエラー
        }

        tmSleepMSec(sleepTime); // 1000ms待機
    }
}


/*! ¥fn int ifUpdate(const int OnOffR)
¥breif 周波数追尾の値を更新する
¥return _ifCal()の戻り値
*/
int ifUpdate(){
  int ret;

  //! if1Interval秒以下だったら処理しない //20250130 コメントアウト。OFF点観測時のみifRepeat()する。
//  if(start != 0 && tmGetLag(p.vtm) < p.if1Interval){
//    ifRepeat();
//    return 0;
//  }

	ret = calc2IfCal();
	//30-cm telescope
//	p.req1.lo1freq      = (calc2GetXoutDflkfq() / 1.0e9 - p.FifL)/108.0; //!< [GHz]  //20240810 コメントアウト
        double Lo1 = 0.0;  
        if(confSetKey("Lo1"))
               Lo1 = atof(confGetVal()); //!< 1st Localの周波数 [GHz]
        p.req1.lo1freq      = (calc2GetXoutDflkfq() / 1.0e9) / 108.0;  //20250204 [GHz]
        
        //SG初期化コマンド送信
        penguin_signalG_init();
        //SGに値を指令する
        setLo1PFreqToSG(); //20250121
        setLo1PAmpToSG();  //20250121
//	p.req2.lo2freq      = p.FifL - p.fcenter; //!< [GHz]  //20240810 2nd Localは固定になったのでコメントアウト
        
        //SGから値を取得
        getLo1RFreqfromSG(); //20250128
        getLo1RAmpfromSG();  //20250128


  if(ret){
    uM1("ifUpdate(); _ifCal(); return [%d] error!!", ret);
    return ret;
  }
/*
  uM3("ifUpdate(); Fobs %+10.9e Hz,  Fdop %+10.9e Hz,  Vrad %+10.9lf m/s", p.xin.dobsfq, p.xout.dflkfq, p.dvrad);
*/
	calc2PrintResults();
  //! 送信
  ret = ifRepeat2();
/*
  uM1("ifUpdate(); DateTime %s", p.cstart);
*/
//Lo2は固定値になったのでコメントアウト 20240810 20250121
  uM2("Request 1st Local (%.9lf GHz, %.1lf dBm)", p.req1.lo1freq, p.req1.lo1amp);
  uM2("Request 2nd IF Att CO (H_pol %4.1lf dB, V_pol %4.1lf dB)", p.req2.att, p.req2.att3);
  uM2("Request 2nd IF Att CI (H_pol %4.1lf dB, V_pol %4.1lf dB)", p.req2.att2, p.req2.att4);
//  uM2("Request Lo2(%.9lf GHz, %.1lf dBm)", p.req2.lo2freq, p.req2.lo2amp);

  uM2("Answer 1st Local (%.9lf GHz, %.1lf dBm)", p.aIf1.ansLo1Freq, p.aIf1.ansLo1Amp);
  uM2("Answer 2nd IF Att CO (H_pol %4.1lf dB, V_pol %4.1lf dB)", p.aIf2.ansAtt1, p.aIf2.ansAtt3);
  uM2("Answer 2nd IF Att CI (H_pol %4.1lf dB, V_pol %4.1lf dB)", p.aIf2.ansAtt2, p.aIf2.ansAtt4);
//  uM2("Answer  Lo2(%.9lf GHz, %4.1lf dBm)", p.aIf2.ansLo2Freq, p.aIf2.ansLo2Amp);

  //! エラーがなければタイマーをリセットする。
  if(ret == 0){
    start = 1;
    tmReset(p.vtm);
  }
  return ret;
}


/*! ¥fn int _ifNet()
¥breif 制御プログラムと通信を行う
¥retval 0 成功
¥retval 0以外 エラーコード
*/
int _ifNet(){
  int ret;
  int err;

/*
  tReqIf1 rIf1;
  tReqIf2 rIf2;
*/
  tAnsIf1 aIf1;
  tAnsIf2 aIf2;

  err = 0;
//printf("Size of tReqIf1: %zu\n", sizeof(tReqIf1));
//printf("Size of tAnsIf1: %zu\n", sizeof(tAnsIf1));

//uM1("Debug: sizeof(p.req1) = %zu", sizeof(p.req1)); // データサイズ確認

  //! 第一中間周波数部制御プログラム
	if(controlBoardGet1stIF() == CONTROL_BOARD_USE_FULL){//090611 in
      usleep(100000);//100ms待機
      ret = netclWrite(p.net1, (const unsigned char*)&p.req1, sizeof(p.req1));
/*
    ret = netclWrite(p.net1, (const unsigned char*)&rIf1, sizeof(rIf1));
*/
    if(ret < 0){
      uM1("Debug: p.net1 = %d", p.net1); // ソケット状態の確認
      perror("_ifNet: netclWrite error");
      uM1("_ifNet(); netclWrite(); IF1 network error = %d", ret);
      err = IF_NET_ERR;
    }
    else if(ret != sizeof(p.req1)){
      uM1("_ifNet(); netclWrite(); IF1 send size error = %d", ret);
      err = IF_NET_DAT_ERR;
    }
    else{
      ret = netclRead(p.net1, (unsigned char*)&aIf1, sizeof(aIf1));
      if(ret < 0){
	uM1("_ifNet(); netclRead(); IF1 network error = %d", ret);
	err = IF_NET_ERR;
      }
      else if(ret != sizeof(aIf1)){
	uM1("_ifNet(); netclRead(); IF1 receive size error = %d", ret);
	err = IF_NET_DAT_ERR;
      }
      else{
	memcpy(&p.aIf1, &aIf1, sizeof(p.aIf1));
      }
    }
  }

  //! 第二中間周波数部制御プログラム
	if(controlBoardGet2ndIF() == CONTROL_BOARD_USE_FULL){//090611 in
    //netuCnvLong((unsigned char*)&rIf2.reqLo2Freq);
    //netuCnvLong((unsigned char*)&rIf2.reqLo2Amp);
	ret = netclWrite(p.net2, (const unsigned char*)&p.req2, sizeof(p.req2));
/*
    ret = netclWrite(p.net2, (const unsigned char*)&rIf2, sizeof(rIf2));
*/
    if(ret < 0){
      uM1("_ifNet(); netclWrite(); IF2 network error = %d", ret);
      err = IF_NET_ERR;
    }
    else if(ret != sizeof(p.req2)){
      uM1("_ifNet(); netclWrite(); IF2 send size error = %d", ret);
      err = IF_NET_DAT_ERR;
    }
    else{
      ret = netclRead(p.net2, (unsigned char*)&aIf2, sizeof(aIf2));
      if(ret < 0){
	uM1("_ifNet(); netclRead(); IF2 network error = %d", ret);
	err = IF_NET_ERR;
      }
      else if(ret != sizeof(aIf2)){
	uM1("_ifNet(); netclRead(); IF2 receive size error = %d", ret);
	err = IF_NET_DAT_ERR;
      }
      else{
	//netuCnvLong((unsigned char*)&aIf2.ansLo2Freq);
	//netuCnvLong((unsigned char*)&aIf2.ansLo2Amp);
	//aIf2.ansLo2Freq /= 1000 * 1000 * 1000;
	memcpy(&p.aIf2, &aIf2, sizeof(p.aIf2));
      }
    }
  }
  return err;
}

/*! ¥fn void ifSetLo1Freq(double Freq)
¥brief 1stIFのローカル周波数を設定する
¥param[in] Freq [GHz]
*/
void ifSetLo1Freq(double Freq){
  uM1("ifSetLo1Freq(); Set p.req1.lo1freq = %.10e [GHz]", Freq);
  p.req1.lo1freq = Freq;
  return;
}

/*! ¥fn void ifSetLo1Amp(double Amp)
¥brief 1stIFのローカルの出力値を設定する。
¥param[in] Amp [dBm]
*/
void ifSetLo1Amp(double Amp){
  uM1("ifSetLo1Amp(); Set p.req1.lo1amp = %lf [dBm]", Amp);
  p.req1.lo1amp = Amp;
  return;
}

/*! ¥fn void ifSetLo1Att(double Att)
¥brief 1stIFのアッテネーターを設定する
¥param[in] Att必ずマイナス値をセットすること [dB]
*/
void ifSetLo1Att(double Att_1){
  if(Att_1 > 0){
    uM1("ifSetLo1Att(); CO  Att[%d] > 0 error!!", Att_1);
    return;
  }
  uM2("ifSetLo1Att(); CO Set p.req1.att = %lf (OnOffR = %d)", Att_1, p.OnOffR);

  //20240810
  //! 該当するスキャンの減衰値を上書きする
  if(p.OnOffR == 0 || p.OnOffR == 1){
    p.IfAtt_1 = Att_1;
  }
  else{
    p.IfAttDecR_1 = Att_1;
  }

  return;
}

//20240810 CI用に新しく関数を追加した
/*! ¥fn void ifSetLo1Att2(double Att_2)
¥brief CI 1stIFのアッテネーターを設定する
¥param[in] Att必ずマイナス値をセットすること [dB]
*/
void ifSetLo1Att2(double Att_2){
  if(Att_2 > 0){
    uM1("ifSetLo1Att2(); CO  Att_2[%d] > 0 error!!", Att_2);
    return;
  }
  uM2("ifSetLo1Att2(); CI  Set p.req2.att2 = %lf (OnOffR = %d)", Att_2, p.OnOffR);

 //20240801
  //! 該当するスキャンの減衰値を上書きする
  if(p.OnOffR == 0 || p.OnOffR == 1){
    p.IfAtt_2 = Att_2;
  }
  else{
    p.IfAttDecR_2 = Att_2;
  }

  return;
}

//20241126 CO 2系統目
/*! ¥fn void ifSetLo1Att3(double Att3)
¥brief 1stIFのアッテネーターを設定する
¥param[in] Att必ずマイナス値をセットすること [dB]
*/
void ifSetLo1Att3(double Att_3){
  if(Att_3 > 0){
    uM1("ifSetLo1Att3(); CO  Att3[%d] > 0 error!!", Att_3);
    return;
  }
  uM2("ifSetLo1Att3(); CO Set p.req2.att3 = %lf (OnOffR = %d)", Att_3, p.OnOffR);

  if(p.OnOffR == 0 || p.OnOffR == 1){
    p.IfAtt_3 = Att_3;
  }
  else{
    p.IfAttDecR_3 = Att_3;
  }

  return;
}

//20241126 CI 2系統目
/*! ¥fn void ifSetLo1Att4(double Att4)
¥brief 1stIFのアッテネーターを設定する
¥param[in] Att必ずマイナス値をセットすること [dB]
*/
void ifSetLo1Att4(double Att_4){
  if(Att_4 > 0){
    uM1("ifSetLo1Att4(); CI  Att4[%d] > 0 error!!", Att_4);
    return;
  }
  uM2("ifSetLo1Att4(); CI Set p.req2.att4 = %lf (OnOffR = %d)", Att_4, p.OnOffR);

  if(p.OnOffR == 0 || p.OnOffR == 1){
    p.IfAtt_4 = Att_4;
  }
  else{
    p.IfAttDecR_4 = Att_4;
  }

  return;
}

/*! ¥fn void ifSetLo2Freq(double Freq)
¥brief 2ndIFのローカル周波数を設定する
¥param[in] Freq [GHz]
*/
void ifSetLo2Freq(double Freq){
  uM1("ifSetLo2Freq(); Set p.req2.lo2freq = %.10e [GHz]", Freq);
  p.req2.lo2freq = Freq;
  return;
}

/*! ¥fn void ifSetLo2Amp(double Amp)
¥brief 2ndIFのローカルの出力値を設定する
¥param[in] Amp [dBm]
*/
void ifSetLo2Amp(double Amp){
  uM1("ifSetLo2Amp(); Set p.req2.lo2amp = %lf [dBm]", Amp);
  p.req2.lo2amp = Amp;
  return;
}

/*! ¥fn int ifSetAttOnOffR(int OnOffR)
¥brief 可変減衰器の値をOnOffとRで切り替える
¥param[in] OnOffR 0:On 1:Off 2:R
¥return 0:Success 1:Fault
 */
int ifSetAttOnOffR(int OnOffR){
  //! 引数チェック
  if(OnOffR < 0 || OnOffR > 2){
    uM1("ifSetAttOnOffR(%d) error!!", OnOffR);
    return -1;
  }

  p.OnOffR = OnOffR;
//20240810
  if(OnOffR == 0 || OnOffR == 1){
    //! OnOff
    p.req2.att = p.IfAtt_1;
    p.req2.att2 = p.IfAtt_2;
    p.req2.att3 = p.IfAtt_3;
    p.req2.att4 = p.IfAtt_4;
  }
  else{
    //! R
    p.req2.att = p.IfAttDecR_1;
    p.req2.att2 = p.IfAttDecR_2;
    p.req2.att3 = p.IfAttDecR_3;
    p.req2.att4 = p.IfAttDecR_4;
  }
  uM2("ifSetAttOnOffR(); Set OnOffR=%d StepAtt=%.1lf [dB]", OnOffR, p.req2.att);
  uM2("ifSetAttOnOffR(); Set OnOffR=%d StepAtt2=%.1lf [dB]", OnOffR, p.req2.att2);
  uM2("ifSetAttOnOffR(); Set OnOffR=%d StepAtt3=%.1lf [dB]", OnOffR, p.req2.att3);
  uM2("ifSetAttOnOffR(); Set OnOffR=%d StepAtt4=%.1lf [dB]", OnOffR, p.req2.att4);
  return 0;
}

/*! ¥fn void ifGetLo1PFreq(double* Lo1PFreq)
¥brief 1stIFのローカルの周波数を取得する(指令値)
¥param[out] Lo1PFreq [GHz]
*/
double ifGetLo1PFreq(){
	return p.req1.lo1freq; //!< GHz
}

// 20250121　指令値を取得してSGに送る
void setLo1PFreqToSG() {
    double freq = ifGetLo1PFreq();
    uM1("setLo1PFreqToSG(); 1st Local Frequency Set: %f GHz\n", freq);
    penguin_signalG_setFrequency(freq);
}

/*! ¥fn void ifGetLo1PAmp(double* Lo1PAmp)
¥brief 1stIFのローカルの出力値を取得する(指令値)
¥param[out] Lo1PAmp [dBm]
*/
double ifGetLo1PAmp(){
	return p.req1.lo1amp;
}

// 20250121　指令値を取得してSGに送る
void setLo1PAmpToSG() {
    double amp = ifGetLo1PAmp();
    uM1("setLo1PAmpToSG(); 1st Local Amp Set: %f dBm\n", amp);
    penguin_signalG_setPower(amp);
}

/*! ¥fn void ifGetLo1PAtt(double* Lo1PAtt)
¥brief 1stIFのステップアッテネーターの減衰値を取得する(指令値)
¥param[out] Lo1PAtt [dB]
*/
double ifGetLo1PAtt(){
	return p.req2.att;
}

//20241126 以下にIFATT2,3,4分を追加
double ifGetLo1PAtt2(){
        return p.req2.att2;
}

double ifGetLo1PAtt3(){
        return p.req2.att3;
}

double ifGetLo1PAtt4(){
        return p.req2.att4;
}

/*! ¥fn void ifGetLo1RFreq(double* Lo1RFreq)
¥brief 1stIFのローカルの周波数を取得する(実際値)
¥param[out] Lo1RFreq [GHz]
*/
double ifGetLo1RFreq(){
	return p.aIf1.ansLo1Freq; //!< GHz
}

//20250128 SG周波数の実際値を取得してp.aIf1.ansLo1Freqに値をつめる
void getLo1RFreqfromSG(){
       double freq = penguin_signalG_getFrequency();
       p.aIf1.ansLo1Freq = freq;
       uM1("getLo1PFreqfromSG(); 1st Local Frequency Get: %f GHz\n", freq/1.0e9);
}

/*! ¥fn void ifGetLo1RAmp(double* Lo1RAmp)
¥brief 1stIFのローカルの出力値を取得する(実際値)
¥param[out] Lo1RAmp [dBm]
*/
double ifGetLo1RAmp(){
	return p.aIf1.ansLo1Amp;
}

//20250128 SGアンプの実際値を取得してp.aIf1.ansLo1Ampに値をつめる
void getLo1RAmpfromSG(){
       double amp = penguin_signalG_getPower();
       p.aIf1.ansLo1Amp = amp;
       uM1("getLo1PAmpfromSG(); 1st Local Amp Get: %f dBm\n", amp);
}

/*! ¥fn void ifGetLo1RAtt(double* Lo1RAtt)
¥brief 1stIFのステップアッテネーターの減衰値を取得する(実際値)
¥brief ステップアッテネーターから実際値を取得できないので、現在は指令値と同じ。
¥param[out] Lo1RAtt [dB]
*/
double ifGetLo1RAtt(){
	return p.aIf2.ansAtt1;
}

//20241126 以下IFATT2,3,4分を追加
double ifGetLo1RAtt2(){
        return p.aIf2.ansAtt2;
}

double ifGetLo1RAtt3(){
        return p.aIf2.ansAtt3;
}

double ifGetLo1RAtt4(){
        return p.aIf2.ansAtt4;
}

/*
void ifGetLo1RAtt(double* Lo1RAtt){
  *Lo1RAtt = p.aIf1.ansAtt1;
  return;
}
*/
/*! ¥fn void ifGetLo2PFreq(double* Lo2PFreq)
¥brief 2ndIFのローカルの周波数を取得する(計算値)
¥param[out] Lo2PFreq [GHz]
*/
double ifGetLo2PFreq(){
	return p.req2.lo2freq; //!< GHz
}
/*
void ifGetLo2PFreq(double* Lo2PFreq){
  *Lo2PFreq = p.req2.lo2freq; //!< GHz
  return;
}
*/

/*! ¥fn void ifGetLo2PAmp(double* Lo2PAmp)
¥brief 2ndIFのローカルの出力値を取得する(指令値)
¥param[out] Lo2PAmp [dBm]
*/
double ifGetLo2PAmp(){
	return p.req2.lo2amp;
}
/*
void ifGetLo2PAmp(double* Lo2PAmp){
  *Lo2PAmp=p.req2.lo2amp;
  return;
}
*/
/*! ¥fn void ifGetLo2RFreq(double* Lo2RFreq)
¥brief 2ndIFのローカルの周波数を取得する(実際値)
¥param[out] Lo2RFreq [GHz]
*/
double ifGetLo2RFreq(){
	return p.aIf2.ansLo2Freq; //!< GHz
}
/*
void ifGetLo2RFreq(double* Lo2RFreq){
  *Lo2RFreq = p.aIf2.ansLo2Freq; //!< GHz
  return;
}
*/

/*! ¥fn void ifGetLo2RAmp(double* Lo2RAmp)
¥brief 2ndIFのローカルの出力値を取得する(実際値)
¥param[out] Lo2RAmp [dBm]
*/
double ifGetLo2RAmp(){
	return p.aIf2.ansLo2Amp;
}
/*
void ifGetLo2RAmp(double* Lo2RAmp){
  *Lo2RAmp = p.aIf2.ansLo2Amp;
  return;
}
*/

/*
void ifGetVRAD(double* VRAD){
  *VRAD=p.dvrad / 1.0e3; //!< km/s
  return;
}
*/

/*! ¥fn void setParam()
¥brief パラメーター情報を取得する
* thread 1, phase C0
*/
void setParam(){
  if(confSetKey("Lo1Amp"))
    p.req1.lo1amp = atof(confGetVal());
  if(confSetKey("Lo2Amp"))
    p.req2.lo2amp = atof(confGetVal());
//20240810
  if(confSetKey("IfAtt_1"))
    p.IfAtt_1 = atof(confGetVal());
  if(confSetKey("IfAttDecR_1"))
    p.IfAttDecR_1 = atof(confGetVal());
  if(confSetKey("IfAtt_2"))
    p.IfAtt_2 = atof(confGetVal());
  if(confSetKey("IfAttDecR_2"))
    p.IfAttDecR_2 = atof(confGetVal());
  if(confSetKey("IfAtt_3"))
    p.IfAtt_3 = atof(confGetVal());
  if(confSetKey("IfAttDecR_3"))
    p.IfAttDecR_3 = atof(confGetVal());
  if(confSetKey("IfAtt_4"))
    p.IfAtt_4 = atof(confGetVal());
  if(confSetKey("IfAttDecR_4"))
    p.IfAttDecR_4 = atof(confGetVal());
  if(confSetKey("fif_1"))
    p.Fif_1 = atof(confGetVal());            //!< GHz
  if(confSetKey("fif_2"))
    p.Fif_2 = atof(confGetVal());  
   if(confSetKey("lo2_1"))
    p.lo2_1 = atof(confGetVal());            //!< GHz
  if(confSetKey("lo2_2"))
    p.lo2_2 = atof(confGetVal());
  if(confSetKey("lo2_3"))
    p.lo2_3 = atof(confGetVal());
   if(confSetKey("lo2_4"))
    p.lo2_4 = atof(confGetVal());            //!< GHz
  //if(confSetKey("Fif"))
  //  p.FifL = atof(confGetVal());    //!< 互換性のため当面残しておく
  if(confSetKey("FsaL_H"))
    p.fcenter = atof(confGetVal());         //!< GHz
  //if(confSetKey("SaFcenter_H"))
  //  p.safcenter = atof(confGetVal());     //!< Hz
//  if(confSetKey("LoInterval"))     20250130 OFF点観測時にしかifupdate()しない。
//    p.if1Interval = atoi(confGetVal());
/*
  if(confSetKey("trkTimeOffset"))
    p.trkTimeOffset = atof(confGetVal());
*/
/* 081121 out
  if(confSetKey("SourceFlag"))
    p.ioflg = atoi(confGetVal());           //!< 天体フラグ 0:太陽系外 1-9:惑星 10:月 11:太陽 12:COMET 99:AZEL
  if(confSetKey("Coordinate")){
    if(p.ioflg == 0){
      p.iaoflg = atoi(confGetVal());        //!< 位置座標系 0:惑星 1:RADEC 2:LB 3:AZEL
    }
    else{
      p.iaoflg = 0;
    }
  }
  if(confSetKey("Epoch"))
    p.ibjflg = atoi(confGetVal());          //!< 分点フラグ 1:B1950 2:J2000
  if(confSetKey("Velocity"))
    p.drdlv = atof(confGetVal()) * 1.0e3;   //!< 天体の視線速度 [m/s]
  if(confSetKey("X_Rad"))
    p.dsourc[0] = atof(confGetVal());
  if(confSetKey("Y_Rad"))
    p.dsourc[1] = atof(confGetVal());
*/

/*
  if(1)
    p.xin.irpflg = 1;                       //!< ポインティング 0:虚角 1:実角
  if(1)
    p.xin.iapflg = 3;                       //!< ポインティング座標定義初期値 1:RADEC 2:LB 3:AZEL
  if(1)
    p.xin.irsflg = 1;                       //!< スキャンニング 0:虚角 1:実角
  if(1){
*/
    /*! スキャンニングの座標定義の初期値はRADECを使用する
     *  if.cではスキャニングの座標定義は初期値をずっと用いる
     */
/*
    p.xin.iasflg = 1;
  }
  if(1)
    p.xin.itnscn = 1;                       //!< スキャンニング点数
  if(1)
    p.xin.inoscn = 1;                       //!< スキャンニング点
  if(confSetKey("Definition"))
    p.xin.ivdef = atoi(confGetVal());
  if(confSetKey("Frame"))
    p.xin.ivref = atoi(confGetVal());
  if(confSetKey("Ftrack"))
    p.xin.dobsfq = atof(confGetVal()) * 1.0e9; //!< 静止トラッキング周波数 [Hz]
*/
}

/*! ¥fn int checkParam()
¥brief パラメーターのチェック
¥retval 0 成功
¥retval -1 失敗
* thread 1, phase C0
*/
int checkParam(){
	if(controlBoardGet1stIF() != CONTROL_BOARD_USE_NO){//090611 in
  }
	if(controlBoardGet2ndIF() != CONTROL_BOARD_USE_NO){//090611 in
  }

	if(controlBoardGet1stIF() != CONTROL_BOARD_USE_NO || controlBoardGet2ndIF() != CONTROL_BOARD_USE_NO){//090611 in
    if(p.if1Interval < 0){
	uM1("checkParam(); ERROR: Invalid LoInterval value (%d).", p.if1Interval);//090611 in
/* 090611 out
      uM2("IfUse1(%d) LoInterval(%d) error!!¥n", p.use1, p.if1Interval);
*/
      return -1;
    }
/* 081121 out
    if(!((p.ioflg >= 0 && p.ioflg <= 12) || p.ioflg == 99)){
      uM2("IfUse(%d) SourceFlag(%d) error!!¥n", p.use1, p.ioflg);
      return -1;
    }
    if(p.iaoflg < 0 || p.iaoflg > 3){
      uM2("IfUse1(%d) Coordinate(%d) error!!¥n", p.use1, p.iaoflg);
      return -1;
    }
    if(p.ibjflg < 1 || p.ibjflg > 2){
      uM2("IfUse1(%d) Epoch(%d) error!!¥n", p.use1, p.ibjflg);
      return -1;
    }
*/
/* 081121 out
    if(p.xin.ivdef < 1 || p.xin.ivdef > 2){
      uM2("IfUse1(%d) Definition(%d) error!!¥n", p.use1, p.xin.ivdef);
      return -1;
    }
    if(p.xin.ivref < 1 || p.xin.ivref > 2){
      uM2("IfUse1(%d) Frame(%d) error!!¥n", p.use1, p.xin.ivref);
      return -1;
    }
    //if(p.xin.dobsfq < || p.xin.dobsfq > )
    //  return -1;
*/
  }
  return 0;
}
#endif


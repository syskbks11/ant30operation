/*!
¥file if.cpp
¥author Y.Koide
¥date 2006.11.28
¥brief IF制御用
*/
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>

#include "configuration.h"
#include "celestialObject.h"
#include "trk45Sub.h"
#include "errno.h"
#include "if.h"
#include "ifCalc2.h"

/* Constants used in ifRepeat() */
static const double _LO1_FREQ_THRESH = 1.0e-9; //!< 許容誤差[GHz]
static const double _LO1_AMP_THRESH = 0.01;    //!< 許容誤差[dB]
static const double _LO1_ATT_THRESH = 0.01;      //!< 許容誤差[dB]
static const double _LO2_FREQ_THRESH = 1.0e-9; //!< 許容誤差[GHz]
static const double _LO2_AMP_THRESH = 0.01;    //!< 許容誤差[dB]

//! 1stIFへのリクエスト構造体
typedef struct sReqIf1{
  double reqLo1Freq;    //!< 1st Local Frequency [GHz]
  double reqLo1Amp;     //!< 1st Local Amplitude [dBm]
  double reqAtt1;       //!< 1st IF Step Att [-dB]
}tReqIf1;

//! 2ndIFへのリクエスト構造体
typedef struct sReqIf2{
  double reqLo2Freq;    //!< 2nd Local Frequency [GHz]
  double reqLo2Amp;     //!< 2nd Local Amplitude [dBm]
}tReqIf2;


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
*/
int ifInit(){
  //char cerr[256];

  memset(&p, 0, sizeof(p));
  start = 0;

  //! 制御プログラムへの接続パラメータのみ取得
  if(confSetKey("IfUse01"))
    p.use1 = atoi(confGetVal());
  if(confSetKey("IfUse02"))
    p.use2 = atoi(confGetVal());
  if(confSetKey("IfIp01"))
    strcpy(p.if1ip, confGetVal());
  if(confSetKey("IfIp02"))
    strcpy(p.if2ip, confGetVal());
  if(confSetKey("IfPort01"))
    p.if1port = atoi(confGetVal());
  if(confSetKey("IfPort02"))
    p.if2port = atoi(confGetVal());

  //! パラメータチェック
  if(p.use1 < 0 || p.use1 > 2){
    uM1("IfUse01(%d) error!!¥n", p.use1);
    return -1;
  }
  if(p.use2 < 0 || p.use2 > 2){
    uM1("IfUse02(%d) error!!¥n", p.use2);
    return -1;
  }

  if(p.use1 == 1){
    if(!confSetKey("IfIp01")){
      uM1("IfUse01(%d) IfIp01() error!!¥n", p.use1); 
      return -1;
    }
    if(p.if1port <= 0){
      uM2("IfUse01(%d) IfPort01(%d) error!!¥n", p.use1, p.if1port);
      return -1;
    }
  }
  if(p.use2 == 1){
    if(!confSetKey("IfIp02")){
      uM1("IfUse02(%d) IfIp02() error!!¥n", p.use2);
      return -1;
    }
    if(p.if2port <= 0){
      uM2("IfUse02(%d) IfPort02(%d) error!!¥n", p.use2, p.if2port);
      return -1;
    }
  }
  
  if(p.use1 == 1){
    p.net1 = netclInit(p.if1ip, p.if1port);
    if(p.net1 == NULL){
      uM("ifInit(); netclInit(); if2 error");
      return IF_NET_ERR;
    }
  }
  if(p.use2 == 1){  
    p.net2 = netclInit(p.if2ip, p.if2port);
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
*/
int ifStart(){

  //! 観測パラメータの初期化
  p.lo1amp = 0;
  p.lo2amp = 0;
  p.IfAtt = 0;
  p.IfAttR = 0;
  p.FifL = 0;
  p.fcenter = 0;
  p.if1Interval = 0;
  p.trkTimeOffset = 0;
/* 081121 out
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
  p.dvrad = 0;
*/
  memset(&p.xout, 0, sizeof(p.xout));
  p.lo1freq = 0;
  p.lo2freq = 0;
  memset(&p.aIf1, 0, sizeof(p.aIf1));
  memset(&p.aIf2, 0, sizeof(p.aIf2));
  p.att = 0;
  
  //! 観測パラメータをセットする
  setParam();
  if(checkParam()){
    uM("ifInit(); checkParam(); error");
    return IF_PARAM_ERR;
  }

  //! 追尾ライブラリの初期化
	calc2Start();
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
  return 0;
}

/*! ¥fn int ifRepeat()
¥brief IF制御を行う。ローカル周波数の更新、ステップアッテネーターの設定。
¥brief 最低更新間隔(p.if1Interval)を超えていない場合は更新しない。
¥retval 0 成功
¥retval 0以外 エラーコード
*/
int ifRepeat(){
  //char tmp[1024];
  int ret;

  if(p.use1 == 0 && p.use2 == 0)
    return IF_NOT_USE;
  
  //! 制御プログラムへ送信
  ret = _ifNet();
  if(ret){
    uM1("ifRepeat(); _ifNet(); return [%d] error!!", ret);
    return ret;
  }

  //! 指令値どおりに設定されているか確認
  if(p.use1 == 1){
    if(fabs(p.aIf1.ansLo1Freq - p.lo1freq) >= _LO1_FREQ_THRESH){
      uM2("ifRepeat(); 1st Lo Freq req[%.10lf] ans[%.10lf] error!!",
	  p.lo1freq, p.aIf1.ansLo1Freq);
      ret = IF_STATE_ERR;
    }
    if(fabs(p.aIf1.ansLo1Amp - p.lo1amp) >= _LO1_AMP_THRESH){
      uM2("ifRepeat(); 1st Lo Amp req[%.2lf] ans[%.2lf] error!!",
	  p.lo1amp, p.aIf1.ansLo1Amp);
      ret = IF_STATE_ERR;
    }
    if(fabs(p.aIf1.ansAtt1 - p.att) >= _LO1_ATT_THRESH){
      uM2("ifRepeat(); StepAtt req[%.2lf] ans[%.2lf] error!!",
	  p.aIf1.ansAtt1, p.att);
      ret = IF_STATE_ERR;
    }
  }

  if(p.use2 == 1){
    if(fabs(p.aIf2.ansLo2Freq - p.lo2freq) >= _LO2_FREQ_THRESH){
      uM2("ifRepeat(); 2nd Lo Freq req[%.10lf] ans[%.10lf] error!!",
	  p.lo2freq, p.aIf2.ansLo2Freq);
      ret = IF_STATE_ERR;
    }
    if(fabs(p.aIf2.ansLo2Amp - p.lo2amp) >= _LO2_AMP_THRESH){
      uM2("ifRepeat(); 2nd Lo Amp req[%.2lf] ans[%.2lf] error!!",
	  p.lo2amp, p.aIf2.ansLo2Amp);
      ret = IF_STATE_ERR;
    }
  }

  return ret;
}

/*! ¥fn int ifUpdate(const int OnOffR)
¥breif 周波数追尾の値を更新する
¥return _ifCal()の戻り値
*/
int ifUpdate(){
  int ret;

  //! if1Interval秒以下だったら処理しない
  if(start != 0 && tmGetLag(p.vtm) < p.if1Interval){
    ifRepeat();
    return 0;
  }

  //! 周波数追尾を計算
  uLock();
	ret = calc2IfCal();
/* 
  ret = _ifCal();
*/
  uUnLock();
	//moved from _ifCal();
//	p.lo1freq      = calc2GetDflkfq() / 1.0e9 - p.FifL; //!< [GHz]

	p.lo1freq      = p.xout.dflkfq / 1.0e9 - p.FifL; //!< [GHz]

	p.lo2freq      = p.FifL - p.fcenter; //!< [GHz]


  if(ret){
    uM1("ifUpdate(); _ifCal(); return [%d] error!!", ret);
    return ret;
  }
/*
  uM3("ifUpdate(); Fobs %+10.9e Hz,  Fdop %+10.9e Hz,  Vrad %+10.9lf m/s", p.xin.dobsfq, p.xout.dflkfq, p.dvrad);
*/
	calc2PrintResults();
  //! 送信
  ret = ifRepeat();
/*
  uM1("ifUpdate(); DateTime %s", p.cstart);
*/
  uM3("Request Lo1(%.9lf GHz, %.1lf dBm), Att %4.1lf dB", p.lo1freq, p.lo1amp, p.att);
  uM2("Request Lo2(%.9lf GHz, %.1lf dBm)", p.lo2freq, p.lo2amp);
  uM3("Answer  Lo1(%.9lf GHz, %4.1lf dBm), Att %4.1lf dB", p.aIf1.ansLo1Freq, p.aIf1.ansLo1Amp, p.aIf1.ansAtt1);
  uM2("Answer  Lo2(%.9lf GHz, %4.1lf dBm)", p.aIf2.ansLo2Freq, p.aIf2.ansLo2Amp);

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

  tReqIf1 rIf1;
  tReqIf2 rIf2;
  tAnsIf1 aIf1;
  tAnsIf2 aIf2;

  rIf1.reqLo1Freq = p.lo1freq;         //!< [GHz]
  rIf1.reqLo1Amp  = p.lo1amp;          //!< [dBm]
  rIf1.reqAtt1    = p.att;             //!< [dB]
  rIf2.reqLo2Freq = p.lo2freq;         //!< [GHz]
  rIf2.reqLo2Amp  = p.lo2amp;          //!< [dBm]

  err = 0;
  //! 第一中間周波数部制御プログラム
  if(p.use1 == 1){
    ret = netclWrite(p.net1, (const unsigned char*)&rIf1, sizeof(rIf1));
    if(ret < 0){
      uM1("_ifNet(); netclWrite(); IF1 network error = %d", ret);
      err = IF_NET_ERR;
    }
    else if(ret != sizeof(rIf1)){
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
  if(p.use2 == 1){
    //netuCnvLong((unsigned char*)&rIf2.reqLo2Freq);
    //netuCnvLong((unsigned char*)&rIf2.reqLo2Amp);
    ret = netclWrite(p.net2, (const unsigned char*)&rIf2, sizeof(rIf2));
    if(ret < 0){
      uM1("_ifNet(); netclWrite(); IF2 network error = %d", ret);
      err = IF_NET_ERR;
    }
    else if(ret != sizeof(rIf2)){
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
  uM1("ifSetLo1Freq(); Set p.lo1Freq = %.10e [GHz]", Freq);
  p.lo1freq = Freq;
  return;
}

/*! ¥fn void ifSetLo1Amp(double Amp)
¥brief 1stIFのローカルの出力値を設定する。
¥param[in] Amp [dBm]
*/
void ifSetLo1Amp(double Amp){
  uM1("ifSetLo1Amp(); Set p.lo1amp = %lf [dBm]", Amp);
  p.lo1amp = Amp;
  return;
}

/*! ¥fn void ifSetLo1Att(double Att)
¥brief 1stIFのアッテネーターを設定する
¥param[in] Att必ずマイナス値をセットすること [dB]
*/
void ifSetLo1Att(double Att){
  if(Att > 0){
    uM1("ifSetLo1Att(); Att[%d] > 0 error!!", Att);
    return;
  }
  uM2("ifSetLo1Att(); Set p.att = %lf (OnOffR = %d)", Att, p.OnOffR);

  //! 該当するスキャンの減衰値を上書きする
  if(p.OnOffR == 0 || p.OnOffR == 1){
    p.IfAtt = Att;
  }
  else{
    p.IfAttR = Att;
  }

  return;
}

/*! ¥fn void ifSetLo2Freq(double Freq)
¥brief 2ndIFのローカル周波数を設定する
¥param[in] Freq [GHz]
*/
void ifSetLo2Freq(double Freq){
  uM1("ifSetLo2Freq(); Set p.lo2Freq = %.10e [GHz]", Freq);
  p.lo2freq = Freq;
  return;
}

/*! ¥fn void ifSetLo2Amp(double Amp)
¥brief 2ndIFのローカルの出力値を設定する
¥param[in] Amp [dBm]
*/
void ifSetLo2Amp(double Amp){
  uM1("ifSetLo2Amp(); Set p.lo2amp = %lf [dBm]", Amp);
  p.lo2amp = Amp;
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

  if(OnOffR == 0 || OnOffR == 1){
    //! OnOff
    p.att = p.IfAtt;
  }
  else{
    //! R
    p.att = p.IfAttR;
  }
  uM2("ifSetAttOnOffR(); Set OnOffR=%d StepAtt=%.1lf [dB]", OnOffR, p.att);
  return 0;
}

/*! ¥fn void ifGetLo1PFreq(double* Lo1PFreq)
¥brief 1stIFのローカルの周波数を取得する(指令値)
¥param[out] Lo1PFreq [GHz]
*/
double ifGetLo1PFreq(){
	return p.lo1freq; //!< GHz
}
/*
void ifGetLo1PFreq(double* Lo1PFreq){
  *Lo1PFreq=p.lo1freq; //!< GHz
  return;
}
*/

/*! ¥fn void ifGetLo1PAmp(double* Lo1PAmp)
¥brief 1stIFのローカルの出力値を取得する(指令値)
¥param[out] Lo1PAmp [dBm]
*/
double ifGetLo1PAmp(){
	return p.lo1amp;
}
/*
void ifGetLo1PAmp(double* Lo1PAmp){
  *Lo1PAmp=p.lo1amp;
  return;
}
*/
/*! ¥fn void ifGetLo1PAtt(double* Lo1PAtt)
¥brief 1stIFのステップアッテネーターの減衰値を取得する(指令値)
¥param[out] Lo1PAtt [dB]
*/
double ifGetLo1PAtt(){
	return p.att;
}
/*
void ifGetLo1PAtt(double* Lo1PAtt){
  *Lo1PAtt=p.att;
  return;
}
*/

/*! ¥fn void ifGetLo1RFreq(double* Lo1RFreq)
¥brief 1stIFのローカルの周波数を取得する(実際値)
¥param[out] Lo1RFreq [GHz]
*/
double ifGetLo1RFreq(){
	return p.aIf1.ansLo1Freq; //!< GHz
}
/*
void ifGetLo1RFreq(double* Lo1RFreq){
  *Lo1RFreq = p.aIf1.ansLo1Freq; //!< GHz
  return;
}
*/
/*! ¥fn void ifGetLo1RAmp(double* Lo1RAmp)
¥brief 1stIFのローカルの出力値を取得する(実際値)
¥param[out] Lo1RAmp [dBm]
*/
double ifGetLo1RAmp(){
	return p.aIf1.ansLo1Amp;
}
/*
void ifGetLo1RAmp(double* Lo1RAmp){
  *Lo1RAmp = p.aIf1.ansLo1Amp;
  return;
}
*/
/*! ¥fn void ifGetLo1RAtt(double* Lo1RAtt)
¥brief 1stIFのステップアッテネーターの減衰値を取得する(実際値)
¥brief ステップアッテネーターから実際値を取得できないので、現在は指令値と同じ。
¥param[out] Lo1RAtt [dB]
*/
double ifGetLo1RAtt(){
	return p.aIf1.ansAtt1;
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
	return p.lo2freq; //!< GHz
}
/*
void ifGetLo2PFreq(double* Lo2PFreq){
  *Lo2PFreq = p.lo2freq; //!< GHz
  return;
}
*/

/*! ¥fn void ifGetLo2PAmp(double* Lo2PAmp)
¥brief 2ndIFのローカルの出力値を取得する(指令値)
¥param[out] Lo2PAmp [dBm]
*/
double ifGetLo2PAmp(){
	return p.lo2amp;
}
/*
void ifGetLo2PAmp(double* Lo2PAmp){
  *Lo2PAmp=p.lo2amp;
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
*/
void setParam(){
  //if(confSetKey("useIf1"))
  //  p.use1 = atoi(confGetVal());
  //if(confSetKey("useIf2"))
  //  p.use2 = atoi(confGetVal());
  //if(confSetKey("if1ip"))
  //  strcpy(p.if1ip, confGetVal());
  //if(confSetKey("if2ip"))
  //  strcpy(p.if2ip, confGetVal());
  //if(confSetKey("if1port"))
  //  p.if1port = atoi(confGetVal());
  //if(confSetKey("if2port"))
  //  p.if2port = atoi(confGetVal());
  if(confSetKey("Lo1Amp"))
    p.lo1amp = atof(confGetVal());
  if(confSetKey("Lo2Amp"))
    p.lo2amp = atof(confGetVal());
  if(confSetKey("IfAtt"))
    p.IfAtt = atof(confGetVal());
  if(confSetKey("IfAttR"))
    p.IfAttR = atof(confGetVal());

  if(confSetKey("FifL"))
    p.FifL = atof(confGetVal());            //!< GHz
  //if(confSetKey("Fif"))
  //  p.FifL = atof(confGetVal());    //!< 互換性のため当面残しておく
  if(confSetKey("FsaL_H"))
    p.fcenter = atof(confGetVal());         //!< GHz
  //if(confSetKey("SaFcenter_H"))
  //  p.safcenter = atof(confGetVal());     //!< Hz
  if(confSetKey("LoInterval"))
    p.if1Interval = atoi(confGetVal());
  if(confSetKey("trkTimeOffset"))
    p.trkTimeOffset = atof(confGetVal());

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
*/
int checkParam(){
  //if(p.use1 < 0 || p.use1 > 2){
  //  uM1("useIf1(%d) error!!¥n", p.use1);
  //  return -1;
  //}
  //if(p.use2 < 0 || p.use2 > 2){
  //  uM1("useIf2(%d) error!!¥n", p.use2);
  //  return -1;
  //}

  //if(p.use1 == 1){
  //  if(!confSetKey("if1ip")){
  //    uM1("useIf1(%d) if1ip() error!!¥n", p.use1); 
  //    return -1;
  //  }
  //  if(p.if1port <= 0){
  //    uM2("useIf1(%d) if1port(%d) error!!¥n", p.use1, p.if1port);
  //    return -1;
  //  }
  //}
  //if(p.use2 == 1){
  //  if(!confSetKey("if2ip")){
  //    uM1("useIf2(%d) if2ip() error!!¥n", p.use2);
  //    return -1;
  //  }
  //  if(p.if2port <= 0){
  //    uM2("useIf2(%d) if2port(%d) error!!¥n", p.use2, p.if2port);
  //    return -1;
  //  }
  //}
  if(p.use1 != 0){
    //if(p.lo1amp < -30 || p.lo1amp > 10)
    //  return -1;
    //if(p.att1 < -11 || p.att1 > 0){
    //  uM2("useIf1(%d) Att_H(%d) error!!¥n", p.use1, p.att);
    //  return -1;
    //}
  }
  if(p.use2 != 0){
    //if(p.lo2amp < -30 || p.lo1amp > 10)
    //  return -1;
  }

  if(p.use1 != 0 || p.use2 != 0){
    //if(p.FifL < 4 || p.FifL > 6)
    //  return -1;
    if(p.if1Interval < 0){
      uM2("IfUse1(%d) LoInterval(%d) error!!¥n", p.use1, p.if1Interval);
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

/*!
\file ifCalc2.h
\author NAGAI Makoto
\date 2008.11.15
\brief Manages calculation of frequency tracking for 32-m telescope & 30-cm telescope
*/

#ifndef __IFCALC2_H__
#define __IFCALC2_H__

#ifdef __cplusplus
extern "C"{
#endif

//! 1stIFからの応答構造体
typedef struct sAnsIf1{
  double ansLo1Freq;    //!< [GHz]
  double ansLo1Amp;     //!< [dBm]
  double ansAtt1;       //!< [-dB]
}tAnsIf1;


//! 2ndIFからの応答構造体
typedef struct sAnsIf2{
  double ansLo2Freq;    //!< [GHz]
  double ansLo2Amp;     //!< [dBm]
}tAnsIf2;


typedef struct sParamIf{
  //! 接続用パラメータ
  int use1;             //!< If1を使用するか 0:未使用 1:使用 2:通信以外使用
  int use2;             //!< If2を使用するか 0:未使用 1:使用 2:通信以外使用
  char if1ip[256];      //!< 1stIF IP
  char if2ip[256];      //!< 2ndIF IP
  int  if1port;         //!< 1stIF Port
  int  if2port;         //!< 2ndIF Port

  //! 変数
  tmClass_t* vtm;            //!< 更新時間の管理
  void* net1;
  void* net2;
  int OnOffR;           //!< 現在観測対象 0:On 1:Off 2:R
  double att;           //!< StepAtt 現在の指令値 [dB]

  //! 観測パラメータ
  double lo1amp;        //!< Local1 Amplitude [dBm]
  double lo2amp;        //!< Local2 Amplitude [dBm]
  double IfAtt;        //!< StepAtt at OnOff-Point [dB]
  double IfAttR;          //!< StepAtt at R [dB]
  double FifL;          //!< 第一IF(Left)出力の信号中心周波数 [GHz]
  double fcenter;       //!< 観測する周波数Frf [Hz]
  //double safcenter;     //!< 分光計の中心周波数 [Hz]
  int  if1Interval;     //!< IF制御を更新する時間間隔 [sec]
  double trkTimeOffset; //!< 何秒後の追尾計算を行うか [sec]

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

  XTRKO xout;           //!< アンテナ速度などDAZEL
//  double dvrad;         //!< trk_velo用変数
  double lo1freq;       //!< Lo1周波数出力値 [GHz]
  double lo2freq;       //!< Lo2周波数出力値 [GHz]
  tAnsIf1 aIf1;         //!< IF1からの応答データ
  tAnsIf2 aIf2;         //!< IF2からの応答データ

//  int dayFlg;           //!< trk_00()を呼び出すタイミング用。UTにおける日付が変わったとき呼ぶ。
}tParamIf;




int calc2Init(tParamIf* p);
int calc2Start();
int calc2IfCal();
void calc2PrintResults();

double calc2GetDflkfq();

#ifdef __cplusplus
}
#endif
#endif

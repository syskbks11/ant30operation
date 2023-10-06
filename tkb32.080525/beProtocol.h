/*!
\file beProtocol.h
\author Y.Koide
\date 2008.03.07
\brief 中央制御バックエンド間通信プロトコル
*/
#ifndef __BEPROTOCOL_H__
#define __BEPROTOCOL_H__

//! 中央制御からバックエンドへの初期化リクエスト構造体
typedef struct beReqInit_s{
  long size;             //!< 送信データサイズ
  int paramSize;         //!< paramのサイズ
  char param;            //!< 初期パラメーター文字列.(PARAMETER1\tVALUE1\nPARAMETER2\tVALUE2\n....\nの形で入っている)
} beReqInit_t;

//! 中央制御からバックエンドへのリクエスト構造体
typedef struct beReqInteg_s{
  long size;              //!< 送信データサイズ
  int exeFlag;            //!< STOP=0, START=1, STATUS=2
  int OnOffRoad;          //!< ON=0, OFF=1, ROAD=2
  //int returnFlag;         //!< NORETSPE=0, RETSPE=1 (多分不要)
  int scanId;             //!< 現在の観測対象ID 0:Load -1:OFF点A -2:OFF点B 1:ON点1番 2:ON点2番 ...
  unsigned long nowTime;  //!< time(&now) nowTime=(unsigned long)now [sec] 単にキャスト
  int crctSec;            //!< 積分指令が届いた時刻から、何回目の正秒を跨いだら積分をスタートさせるか
  double DRA;             //!< スキャンオフセット Δ赤経[rad]
  double DDEC;            //!< スキャンオフセット Δ赤緯[rad]
  double DGL;             //!< スキャンオフセット Δ銀経[rad]
  double DGB;             //!< スキャンオフセット Δ銀緯[rad]
  double DAZ;             //!< スキャンオフセット ΔAz[rad]
  double DEL;             //!< スキャンオフセット ΔEl[rad]
  double RA;              //!< 観測点の座標の絶対値 赤経[rad]
  double DEC;             //!< 観測点の座標の絶対値 赤緯[rad]
  double GL;              //!< 観測点の座標の絶対値 銀経[rad]
  double GB;              //!< 観測点の座標の絶対値 銀緯[rad]
  double AZ;              //!< 観測点の座標の絶対値 Az[rad]
  double EL;              //!< 観測点の座標の絶対値 El[rad]
  double PAZ;             //!< 計算上のアンテナ方位角 Az[rad]
  double PEL;             //!< 計算上のアンテナ仰角 El[rad]
  double RAZ;             //!< 実際のアンテナ方位角 Az[rad]
  double REL;             //!< 実際のアンテナ仰角 El[rad]
  double PA;              //!< ポジションアングル(不要,初期化時にパラメータで渡している)
  //double HPBW;            //!< ビームサイズ
  //double EFFA;            //!< 開口能率
  //double EFFB;            //!< 主ビーム能率
  //double EFFL;            //!< アンテナ能率
  //double EFSS;            //!< FSS能率
  //double GAIN;            //!< アンテナ利得
  //double FQTRK;           //!< 静止トラッキング周波数 [Hz]
  double FQIF1;           //!< 第一中間周波数 [Hz]
  double VRAD;            //!< Vrad [km/s]
  int IFATT;              //!< 中間周波数部減衰器の加算値 [db]
  int dummy2;
  double TEMP;            //!< 気温 --- 10秒平均したもの [degC]
  double PATM;            //!< 気圧 --- 10秒平均したもの [hPa]
  double PH2O;            //!< 水蒸気圧 --- 10秒平均したもの [hPa]
  double VWIND;           //!< 風速 --- 10秒平均したもの [m/s]
  double DWIND;           //!< 風向 --- 10秒平均したもの。無風=0 NE=45 E=90 SE=135 S=180 SW=225 W=270 NW=315 N=360 [deg]などで表される16方位
  double TAU;             //!< 大気の光学的厚み(常に0)
  double TSYS;            //!< システム雑音温度 [K](不要)
  double BATM;            //!< 大気の温度 [K] (詳細不明TEMPとの違いは?)
  double IPINT;           //!< 強度較正体の温度 [K]--- 瞬間値
} beReqInteg_t;


//! バックエンドから中央制御への応答構造体
typedef struct beAnsStatus_s{
  long size;              //!< 送信データサイズ
  int endExeFlag;         //!< -1:FAILED 1:SUCCESS 2:STANDBY 3:EXECUTION 4:ACCEPT 5:REJECT
  int acqErr;             //!< 1:NORMAL 2:ADCovfl 3:ACCovfl 4:ADACCovfl
  float tsys;             //!< Tsys [K]
  float totalPower;       //!< Ptotal [dBm]
  int nbrOfChannel;       //!< number of spectrum-channel (多分不要)
  double spectrum;        //!< == float spectrum[nbrOfChannel] spectrum data (多分不要)
} beAnsStatus_t;

#endif //!< __BEPROTOCOL_H__

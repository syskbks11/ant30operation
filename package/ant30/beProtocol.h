#ifndef INCLUDE_GUARD_UUID_0f3ff94c_f986_45a3_87fd_dc8732088883
#define INCLUDE_GUARD_UUID_0f3ff94c_f986_45a3_87fd_dc8732088883
/*!
\file beProtocol.h
\author R.Enohi
\date 2024.11.26
\brief 中央制御バックエンド間通信プロトコル
       2輝線同時観測＋2偏波化対応
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
  int OnOffRoad;          //!< ON(PSW)=0, OFF=1, ROAD=2, ON(OTF)=3  //20240810
  //int returnFlag;         //!< NORETSPE=0, RETSPE=1 
  int scanId;             //!< 現在の観測対象ID 0:Load -1:OFF点A -2:OFF点B 1:ON点1番 2:ON点2番
  unsigned long nowTime;  //!< time(&now) nowTime=(unsigned long)now [sec] 単にキャスト
  int crctSec;            //!< 積分司令が届いた時刻から、何回目の正秒を跨いだら積分をスタートさせるか
  double DRA;             //!< スキャンオフセット　赤経[rad]
  double DDEC;            //!< スキャンオフセット　赤緯[rad]
  double DGL;             //!< スキャンオフセット　銀経[rad]
  double DGB;             //!< スキャンオフセット　銀緯[rad]
  double DAZ;             //!< スキャンオフセット　Az[rad]
  double DEL;             //!< スキャンオフセット　El[rad]
  double RA;              //!< 観測点の座標の絶対値　赤経[rad]
  double DEC;             //!< 観測点の座標の絶対値　赤緯[rad]
  double GL;              //!< 観測点の座標の絶対値　銀経[rad]
  double GB;              //!< 観測点の座標の絶対値　銀緯[rad]
  double AZ;              //!< 観測点の座標の絶対値　 Az[rad]
  double EL;              //!< 観測点の座標の絶対値　 El[rad]
  double PAZ;             //!< 計算上のアンテナ方位角 Az[deg]
  double PEL;             //!< 計算上のアンテナ仰角 El[deg]
  double RAZ;             //!< 実際のアンテナ方位角 Az[deg]
  double REL;             //!< 実際のアンテナ仰角 El[deg]
  double PA;              //!< ポジションアングル（不要。初期化時にパラメータで渡している）
  //double HPBW;            //!< ビームサイズ
  //double EFFA;            //!< 開口能率
  //double EFFB;            //!< 主ビーム能率
  //double EFFL;            //!< アンテナ能率
  //double EFSS;            //!< FSS能率
  //double GAIN;            //!< アンテナ利得
  //double FQTRK;           //!< 静止トラッキング周波数 [Hz]
  double FQTRK;             //!<  CO 静止周波数[GHz]  //20240810
  double FQTRK2;            //!<  CI 静止周波数[GHz]  //20240810
  double FQTRK3;             //!<  CO 静止周波数 2系統化[GHz]  //20241126
  double FQTRK4;            //!<  CI 静止周波数 2系統化[GHz]  //20241126
  double FQIF1;           //!< 第一中間周波数 [GHz] 1st Local ここにドップラートラッキングをかけた値が入る
  double VRAD;            //!< Vrad [km/s]
  int IFATT;              //!< CO IFのステップアッテネーターの値(ONOFFの時) [db] //20241126
  int IFATT2;             //!< CI IFのステップアッテネーターの値(ONOFFの時) [db] //20241126
  int IFATT3;             //!< CO IFのステップアッテネーターの値(ONOFFの時) 2系統目[db] //20241126
  int IFATT4;             //!< CI IFのステップアッテネーターの値(ONOFFの時) 2系統目[db] //20241126
  double LO2_1;           //!< 2nd Loacal CO [GHz]  //20240810
  double LO2_2;           //!< 2nd Loacal CI [GHz]  //20240810
  double LO2_3;           //!< 2nd Loacal CO 2系統目[GHz]  //20241126
  double LO2_4;           //!< 2nd Loacal CI 2系統目[GHz]  //20241126
  int dummy2;
  double TEMP;            //!< 気温 --- 10秒平均したもの [degC]　気象装置から取得
  double PATM;            //!< 気圧 --- 10秒平均したもの [hPa]　気象装置から取得
  double PH2O;            //!< 水蒸気圧 --- 10秒平均したもの [hPa]　気象装置から取得した湿度から計算により求める。
  double VWIND;           //!< 風速 --- 10秒平均したもの [m/s]　（不要）
  double DWIND;           //!< 風向 --- 10秒平均したもの。無風=0 NE=45 E=90 SE=135 S=180 SW=225 W=270 NW=315 N=360 [deg]などで表される１６方位（不要）
  double TAU;             //!< 光学的厚み（.device から読み取る）
  double TSYS;            //!< システム雑音温度 [K](不要)
  double BATM;            //!< 大気の温度 [K] (TEMPがあるので不要)　分光計calibrationに必要。ここにTEMPの値を詰める。
  double IPINT;           //!< 強度較正体の温度 [K]--- 瞬間値　分光計calibrationに必要。現在はダミー値を送っている。
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
#endif


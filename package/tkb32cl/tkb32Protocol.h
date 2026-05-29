/*!
\file tkb32Protocol.h
\date 2007.01.03
\author Y.Koide
\brief 通信用構造体
*/
#ifndef __TKB32PROTOCOL_H__
#define __TKB32PROTOCOL_H__

#define TKB32_REQ_VER 001
#define TKB32_ANS_VER 001

typedef struct sReq{
  long size;            //!< データサイズ byte
  int reqVer;           //!< プロトコルバージョン
  int reqFlg;           //!< リクエストの種類 0:Status 1:Initialize(SetParam) 2:RemoteControle 3:StartObs 4:StopObs 5:EndObs 6:GetParam
  char data;            //!< 各データの先頭バイト reqFlg=0,3,4の場合は0をセット
}tReq;

typedef struct sReqInit{
  long paramSize;       //!< パラメーターの文字数
  char param;           //!< パラメーター
}tReqInit;

typedef struct sReqRemote{
  unsigned int trkReq;   //!< Bit Flag 0:none 1:AzEl 2:ポインティングオフセット 4:Stop 8:unStow 16:Stow 32:Program Tracking 64:ACU Standby 128:Drive unlock 256:Drive lock
  unsigned int chopReq;  //!<          0:none 1:Close 2:Open 3:Origine
  unsigned int if1Req;   //!< Bit Flag 0:none 1:Set Frequency 2:Set Amplitude 4:Set Step Attenuter
  unsigned int if2Req;   //!< Bit Flag 0:none 1:Set Frequency 2:Set Amplitude
  unsigned int saacqReq; //!< Bit Flag 0:none 1:Set Integ Time 2:Start Integration 4:Stop
  unsigned int sasoftReq;//!<          0:none
  double trkAzEl[2];    //!< trkReq&=1 AzEl [deg]
  double trkPof[2];     //!< trkReq&=2 Poff [deg]
  int trkPofCoord;      //!< trkReq&=2 (未使用)Poff Coordinate 1:RADEC 2:LB 3:AZEL
  int feCompSw;         //!< (未使用)コンプレッサー電源スイッチ 0:None 1:Onにする 2:Offにする
  double if1Freq;       //!< if1Req&=1 [GHz]
  double if1Amp;        //!< if1Req&=2 [dBm]
  double if1Att;        //!< if1Req&=4 [dB]
  double if2Freq;       //!< if2Req&=1 [GHz]
  double if2Amp;        //!< if2Req&=2 [dBm]
  int saacqIntegTime;   //!< saacqReq&&1の時の積分時間の指定 [sec]
  int saacqOnOffR;      //!< saacqReq&&2の時の観測対象 1:On点 2:Off点 3:R-Sky
}tReqRemote;

typedef struct sAns{
  long size;            //!< データサイズ byte
  int ansVer;           //!< プロトコルバージョン
  int ansFlg;           //!< アンサーの種類 0:state 1:Initialize 2:RemoteControle 3:StopObs 4:StartObs 5:End
  char data;            //!< 各データの先頭バイト ansFlg=1,2,3,4の時は0:成功 other:失敗
}tAns;

typedef struct sAnsState{
  unsigned int UT;      //!< time(&UT)で得られる値
  unsigned int LST;     //!< trk20()で得られるLST値
  //! weath
  int weathState;       //!< 0:none other:error code
  int dummy1;
  double weathData[6];  //!< 気温 [K] 気圧 [hPa] 湿度 [hPa] 風向(North=0) [deg] 風速 [m/s] 最大瞬間風速[m/s]
  //! trk
  int trkState;         //!< 0:none other:error code
  int trkScanNo;        //!< 現在のスキャン位置番号 0:R-Sky ~-1:off点の番号 1~:On点の番号
  int trkScanCnt;       //!< ON点の観測回数
  int trkXYCoord;       //!< trkXYの座標系 0:Planet 1:RADEC 2:LB 3:AZEL
  int trkSofCoord;      //!< スキャンオフセットの座標系 1:RADEC 2:LB 3:AZEL
  int trkPofCoord;      //!< ポインティングオフセットの座標系 1:RADEC 2:LB 3:AZEL
  double trkXY[2];      //!< 設定値 RADEC, LB, AZEL のどれか設定したもの
  double trkSof[2];     //!< スキャンオフセット X Y [sec]
  double trkPof[2];     //!< ポジションオフセット X Y [sec]
  int trkPZONE;         //!< アンテナのZone計算値 0:Auto 1:CW 2:CCW
  int trkRZONE;         //!< アンテナのZone実際値 1:CW 2:CCW
  double trkPAZEL[2];   //!< アンテナ向き計算値(自転・公転・歳差・章動など基本となる誤差補正)
  double trkP2AZEL[2];  //!< アンテナ向き計算値2(気象補正込み)
  double trkP3AZEL[2];  //!< アンテナ向き計算値3(気象補正、器差補正込み)
  double trkRAZEL[2];   //!< アンテナ向き実際値
  double trkEAZEL[2];   //!< アンテナ向き誤差(= trkRAZEL - trkP3AZEL)
  int trkTracking;      //!< トラッキングしているか 1:Tracking 0:Swing or Stop
  int trkACUStatus[5];  //!< ACUのステータス
  //! chop
  int chopState;        //!< 0:none other:error code
  int chopStat;         //!< chopの状態
  int chopMoter;        //!< chopのモーターの状態
  int dummy2;
  double chopTemp1;     //!< 温度計ch1
  double chopTemp2;     //!< 温度計ch2 強度較正体の温度 K
  //! fe
  int feState;          //!< 0:none other:error code
  int dummy3;
  double feK;           //!< フロントエンド温度 K
  double fePa;          //!< フロントエンド気圧 Pa
  int feCompStat1;      //!< コンプレッサー温度警報信号 0:None 1:正常 2:異常
  int feCompStat2;      //!< コンプレッサー運転表示 0:None 1:正常 2:異常
  int feCompStat3;      //!< コンプレッサー制御電源 0:None 1:正常 2:異常
  //! if1
  int if1State;         //!< 0:none other:error code
  double ifVrad;        //!< Vrad [km/s]
  double if1PFreq;      //!< 1stIF周波数 Hz
  double if1PAmp;
  double if1PAtt;       //!< アッテネーター値 dB
  double if1RFreq;
  double if1RAmp;
  double if1RAtt;

  //! if2
  int if2State;         //!< 0:none other:error code
  int dummy6;
  double if2PFreq;      //!< 2ndIF周波数 Hz
  double if2PAmp;
  double if2RFreq;
  double if2RAmp;

  //! be
  int saacqState;       //!< 0:none other:error code
  int saacqExe;         //!< 分光計ステータス 0:積分失敗 1:積分成功 2:スタンバイ 3:実行中
  int saacqErr;         //!< 分光計のエラーステータス 1:正常 2:ADCovfl 3:ACCovfl 4:ADACCovfl
  int dummy7;
  double saacqTsys;     //!< Tsys [K]
  double saacqTotPow;   //!< 分光計のスペクトルの積分値の平均 [dBm/GHz]
}tAnsState;

#endif

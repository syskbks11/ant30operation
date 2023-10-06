/*!
¥file trkAcu.h
¥date 2008.11.13
¥author NAGAI Makoto
¥brief 通信用構造体
* trk.cppからacu.cpp用に分離
*/
#ifndef __TRKACU_H__
#define __TRKACU_H__

//! trkのパラメーター構造体
typedef struct sParamTrk{
  //! 接続パラメータ
  int TrkUse;           //!< trkを使用するか 0:使用しない, 1:使用, 2:通信以外使用
  int TrkCom;           //!< RS-232Cポート
  //! 変数
  void* vtm;            //!< 更新時間管理用

  //! 観測パラメータ
  double TrkInterval;   //!< 追尾制御を処理する間隔 sec
  int OnCoord;          //!< On点の座標定義 1:RADEC 2:LB 3:AZEL
  int OffCoord;         //!< Off点の座標定義 1:RADEC 2:LB 3:AZEL
  int OffNumber;     //!< Off点を2点使うか 1:NoUse 2:Use
  int ScanFlag;         //!< スキャンの定義 1:OnOff 2:5Points 3:Grid 4:Random 5:9Point 6:Raster 1001:SourceTracking 1002:Lee Tracking
  int OffInterval;      //!< One-Point、グリッド、ランダムマッピングでOn点何回に一回Off点を入れるか
  int OffMode;          //!< 1:OffsetFromCenter 2:AbsoluteValue
  char* SetPattern;     //!< マッピングの観測順序
  int OnNumber;         //!< マッピングの要素の数
  double PosAngle_Rad;  //!< ポジションアングル [rad]
  double LineTime;      //!< ラスターのアプローチ時間を考慮した1Lineの時間 [sec]
  //double LinePathX_Rad;  //!< ラスターのアプローチ時間を考慮した1Lineの距離X [rad]
  //double LinePathY_Rad; //!< ラスターのアプローチ時間を考慮した1Lineの距離Y [rad]
  double AntAzElMarg[2];//!< アンテナの許容誤差[deg]
  int AntAzElMargThrsd; //!< この回数、AZELが許容誤差内に連続で収まると追尾と判定する
  const char* AntInst[7];//!< アンテナ器差補正パラメータ文字列
  double TrkTimeOffset; //!< 追尾計算を行うときに何秒後の計算を行うかセットする 単位sec
  
  //! 追尾ライブラリ用パラメータ
  int ioflg;            //!< 天体フラグ 0:太陽系外 1-10:惑星 11:太陽 12:COMET 99:AZEL
  int iaoflg;           //!< 位置座標系 0:惑星 1:RADEC 2:LB 3:AZEL
  int ibjflg;           //!< 分点フラグ 1:B1950 2:J2000
  double drdlv;         //!< 天体の視線速度 [m/s]
  double dsourc[2];     //!< 天体位置 [rad]
  double dazelmin[2];   //!< アンテナの最小駆動限界 [deg]
  double dazelmax[2];   //!< アンテナの最大駆動限界 [deg]

  //! 各種変数
  XTRKI xin;            //!< 45m追尾ライブラリ入力パラメーター構造体
  double sof[2];        //!< 現在のスキャンオフセット値(ポジションアングルによる補正無しの値) [rad]
  int sofCoord;         //!< 現在のスキャンオフセットの座標系 1:RADEC 2:LB 3:AZEL
  double* off;          //!< Off点の位置 offX1 offY1 offX2 offY2 ... [sec]
  double* on;           //!< マッピングの際のOn点 onX1 onY1 onX2 onY2 ... [sec]
  double* linePath;     //!< ラスターの際のonからのパス
  int* mapPattern;      //!< マッピングの観測順序 0:R -:Off +:On 
  int mapNum;           //!< マッピングの点数
  char cstart[24];      //!< 観測時刻の設定 YYYYMMDDhhmmss.0 (JST)
  double dweath[3];     //!< 気温 [℃], 気圧 [hPa] 水蒸気圧 [hPa]
  //int pofFlg;           //!< Position offset 座標定義 0:虚角 1:実角
  int pofCoord;         //!< Position offset coord 一時格納用。5点法、9点法で使用
  double pof[2];        //!< Position offset 一時格納用。5点法、9点法で使用
  int rastStart;        //!< ラスター開始フラグ 0:none 1:RasterStart
  time_t rastStartTime;//!< ラスター開始時刻
  double* rastSof;      //!< ラスターの1スキャンのTrkIntervalごとのスキャンオフセット格納用 [rad]
  int rastSofSize;      //!< trkSofに登録されているスキャン位置の数
  int rastSofNo;        //!< 今何番目のラスタースキャン位置を見ているか
  double P[30];         //!< アンテナ器差補正パラメータ

  XTRKO xout;           //!< 45m追尾ライブラリ計算結果出力用構造体
  double doazel[2];     //!< 計算結果AZEL [rad]
  double doazel2[2];    //!< 計算結果に器差補正を加えた値 [deg]
  double doazelC[2];    //!< ACUへ出力用のAZEL [deg]
  double deazel[2];     //!< dazel-doazel2 [deg]
  double dgtdat[8][2];  //!< MapCenter(RADEC)(LB) Obs(RADEC)(LB) [rad] 
  tAcu acuStat;         //!< ACU現在のステータス
  int acuStatSBefore[5]; //!< 一つ前に取得したACUのステータスコード

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
  int dayFlg;           //!< trk_00()を呼び出すタイミング用。UTにおける日付が変わったとき呼ぶ。
  int sofNo;            //!< scan offsetの何番目を処理しているか
  int offNo;            //!< 何番のOff点を処理しているか 0 or 1
  int offFlg;           //!< 直前にOff点観測をしているか 0:No 1:Yes
  unsigned int lst;     //!< LST時分秒のみを秒換算したもの
}tParamTrk;

#endif

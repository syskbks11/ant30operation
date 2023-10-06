/*!
\file tkb32/tkb32ParserTkb32Func.h
\author Y.Koide
\date 2007.01.01
\brief Tsukuba 32m Central Program Functions
\brief init() -> repeat() -> end()
*/
#ifndef __TKB32PARSERTKB32FUNC_H__
#define __TKB32PARSERTKB32FUNC_H__
#ifdef __cplusplus
extern "C"{
#endif

typedef struct sParamTkb32{
/* 090610 moved to controlBoard.cpp
  //! 各制御の起動フラグ 0:起動無し, 1:起動, 2:起動(通信部以外)
  int useWeath;
  int useTrk;
  int useChop;
  int useFe;
  int useIf1;
  int useIf2;
  //int* BeUse;
*/

/* 090610 moved to _loadAntennaFile()
  char FileAnt[128];
*/
	//The following three values should be static, since they are passed to putenv(), though they appear only in _setEnvironmentForTrk45().
/* 090605 out, since they are in penguin_tracking
  char EnvAnt[128];
  char EnvEph[128];
  char EnvTime[128];
*/
  double OnOffTime;
/* 090610 out, since we use scanTable.scanTableGetScanFlag() instead. 
  int ScanFlag;        //!< 1:OnOff 2:5Point 3:Grid 4:Random 5:9Point 6:Rastar 1001:SourceTracking 1002:Lee Tracking
*/
  int SetNumber;
  int RSkyTime;
/* 090610 out, since we use scanTable.scanTableGetLineTime() instead. 
  double LineTime;     //! ラスターのアプローチ時間を考慮した1Lineの時間 [sec]
*/
  double TimeScan;     //080530 // 090608 in for 30 cm
  double TimeApp;      //080530 // 090608 in for 30 cm
  int BeNum;           //!< バックエンドの最大台数

  //! 内部で使用する変数
  int isInitialized;            //!< 初期化の状態 0:初期化前 1:初期化済み
  tmClass_t* vtm;           //!< tmモジュール用
  pthread_t thrdUpdateId;//!< ステータススレッドID
  int thrdUpdateRun;   //!< ステータス取得スレッド 0:停止中 1:実行中
  pthread_t thrdObsId; //!< 観測スレッドID
  int thrdObsRun;      //!< 観測スレッド 0:停止中 1:実行中
  int cntOn;           //!< On点観測のカウントを行う
  int isStow;          //!< stowが入っているか 0:unStow 1:Stow

  //! 通信用のデータ格納
  int reqRet;          //!< クライアントリクエストの結果 0:成功 other:失敗
/*
  tReq* req;           //!< クライアントからの要求
  tAns* ans;           //!< クライアントへの応答データ
*/
  tAnsState stat;      //!< 全ステータス情報
}tParamTkb32;

/* Each implementation is in tkb32Parser.cpp */
int obsInit();
int obsEnd();
int obsStart();
int obsStop();
void* obsAuto(void* _p);
int integStart(int OnOffR, const time_t startTime);

/* implementstion is in tkb32Func.cpp */
tParamTkb32* getParam();

#ifdef __cplusplus
}
#endif
#endif

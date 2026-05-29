#ifndef INCLUDE_GUARD_UUID_02f57d49_6c10_4d17_9f62_2a229efc679c
#define INCLUDE_GUARD_UUID_02f57d49_6c10_4d17_9f62_2a229efc679c
/*!
  ¥file struct_penweather.h
  ¥author Shunsuke HONDA
  ¥date 2024.10.01
  ¥brief Weatherデータ取扱用の構造体定義
  * For Tsukuba Antarctic 30-cm telescope
  */

#include <pthread.h>
#include "../libtkb/src/libtkb.h"

#ifdef __cplusplus
extern "C"{
#endif

  //! リクエスト構造体
  typedef struct sReqWeath{
    int reqSet; //!< 制御コマンド 0:none
  }tReqWeath;

  //! データ構造体
  typedef struct sDatWeath{
    int    status;      //!< 取得データ状況
    int    direct;      //!< 風向 16
    double windVel;     //!< 風速 m/s
    double windVelMax;  //!< 最大瞬間風速 m/s
    double temp;        //!< 気温 K
    double humid;       //!< 湿度 0--100 %
    double water;       //!< 水蒸気圧 hPa
    double press;       //!< 気圧 hPa
    double time;
  }tDatWeath;

  //! 通信用構造体 = リクエスト構造体 + データ構造体
  typedef struct sParamWeath{
    tReqWeath reqWeath;
    tDatWeath datWeath;
  }tParamWeath;

  //! スレッド構造体 = 更新時間関係 + 通信用構造体 + スレッド関係
  typedef struct sThrdWeath{
    //! 接続パラメータ
    tmClass_t* vtm;    //!< 更新時間管理用
    double interval;   //!< 更新時間間隔 [sec]

    tParamWeath p;     //!< データ構造体
    tmClass_t* dtm;    //!< データ取得時間

    //! スレッドパラメータ
    pthread_t thrdID;       //!< スレッドID
    int thrdRun;            //!< スレッド状況 1:実行 0:停止
    int thrdRet;            //!< スレッド戻り値
    pthread_t thrdStatusID; //!< ステータススレッドID
    int thrdStatusRun;      //!< ステータススレッド状況 1:実行 0:停止
  }tThrdWeath;

#ifdef __cplusplus
}
#endif
#endif


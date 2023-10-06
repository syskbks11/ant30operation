/*!
\file libtkb.h
\date 2007.11.12
\author Y.Koide
\brief 主要なモジュールをまとめたライブラリ
*/
#ifndef __LIBTKB_H__
#define __LIBTKB_H__

#include <stdio.h>
/*! u */
#define uMWrite(str, x1, x2, x3, x4) \
{ \
  fprintf(stdout, "%s ", uGetTime()); \
  fprintf(stdout, str, x1, x2, x3, x4); \
  fprintf(stdout, "\n"); \
  if(uFp()){ \
  fprintf(uFp(), "[%s-%s-%d] ", uGetDate(), __FILE__, __LINE__); \
  fprintf(uFp(), str, x1, x2, x3, x4); \
  fprintf(uFp(), "\n"); \
  } \
}

#define uEWrite(str, x1, x2, x3, x4) \
{ \
  fprintf(stderr, "%s ", uGetTime()); \
  fprintf(stderr, str, x1, x2, x3, x4); \
  fprintf(stderr, "\n"); \
  if(uFp()){ \
  fprintf(uFp(), "[%s-%s-%d] ERROR ", uGetDate(), __FILE__, __LINE__); \
  fprintf(uFp(), str, x1, x2, x3, x4); \
  fprintf(uFp(), "\n"); \
  exit(1); \
  } \
}

/*! \fn uM(str)
\brief メッセージ表示、ログ記録用関数
\brief 標準出力に時刻とメッセージ文字列を表示すると共にログに時刻、ファイル名、行番号をメッセージ文字列を表示する。
\param[in] str メッセージ文字列
*/
#define uM(str) { uMWrite((str), NULL, NULL, NULL, NULL); }

/*! \fn uM1(str)
\brief メッセージ表示、ログ記録用関数
\brief printf();形式で変数が1つの場合に使用。他はuM();を参照
\param[in] str 文字列
\param[in] x1 変数1
*/
#define uM1(str,x1) { uMWrite((str), (x1), NULL, NULL, NULL); }

/*! \fn uM2(str)
\brief メッセージ表示、ログ記録用関数
\brief printf();形式で変数が2つの場合に使用。他はuM();を参照
\param[in] str 文字列
\param[in] x1 変数1
\param[in] x2 変数2
*/
#define uM2(str,x1,x2) { uMWrite((str), (x1), (x2), NULL, NULL); }

/*! \fn uM3(str)
\brief メッセージ表示、ログ記録用関数
\brief printf();形式で変数が3つの場合に使用。他はuM();を参照
\param[in] str 文字列
\param[in] x1 変数1
\param[in] x2 変数2
\param[in] x3 変数3
*/
#define uM3(str,x1,x2,x3) { uMWrite((str), (x1), (x2), (x3), NULL); }

/*! \fn uM4(str)
\brief メッセージ表示、ログ記録用関数
\brief printf();形式で変数が4つの場合に使用。他はuM();を参照
\param[in] str 文字列
\param[in] x1 変数1
\param[in] x2 変数2
\param[in] x3 変数3
\param[in] x4 変数4
*/
#define uM4(str,x1,x2,x3,x4) { uMWrite((str), (x1), (x2), (x3), (x4)); }

#define uE(str) { uEWrite((str), NULL, NULL, NULL, NULL); }
#define uE1(str,x1) { uEWrite((str), (x1), NULL, NULL, NULL); }
#define uE2(str,x1,x2) { uEWrite((str), (x1), (x2), NULL, NULL, NULL); }
#define uE3(str,x1,x2,x3) { uEWrite((str), (x1), (x2), NULL, NULL); }
#define uE4(str,x1,x2,x3,x4) { uEWrite((str), (x1), (x2), (x3), NULL); }

typedef time_t;

#ifdef __cplusplus
template <class T> T uEndian(T* p);
extern "C"{
#endif

  /*! conf */
  /*! \fn const char* confVersion();
  \return confのバージョン文字列を返す 
  */  
  const char* confVersion();

  /*! \fn int confInit()
  \brief confの初期化を行う。
  \return 0:Success
  */
  int confInit();

  /*! \fn int confAddFile(const char* fileName)
  \brief ファイルからパラメータを取得する
  \param[in] fileName ファイル名込みのパス
  \reutrn 0:Success 1:Fault
  */
  int confAddFile(const char* fileName);

  /*! \fn int confAddStr(const char* str)
  \brief 1行もしくは複数行の文字列からパラメータを取得する
  \param[in] str パラメータの記された文字列
  \reutrn 0:Success
  */
  int confAddStr(const char* str);

  /*! \fn const char* confGetStr(const char* _setKey)
  \brief _setKeyに対応するパラメータの値を取得する。
  \param[in] _setKey キーとなる文字列
  \return NULL:Fault other:値の入った文字列へのポインタ
  */
  const char* confGetStr(const char* key);
  
  /*! \fn int confSetKey(const char* _setKey)
  \brief confGetVal();で値を取り出すために設定するキーの登録
  \param[in] _setKey キーとなる文字列
  \return 0:Fault 1:Success
  */  
  int confSetKey(const char* _setKey);
  
  /*! \fn const char* confGetVal()
  \brief confSetKey();で設定されたキーに対応するパラメータの値を取得する
  \return NULL:Fault other:対応する文字列へのポインタ
  */
  const char* confGetVal();
  
  /*! \fn const char* confGetAllKeyVal()
  \return 全パラメータの文字列。1行にキーと値が一組ずつ入っている。
  */
  const char* confGetAllKeyVal();
  
  /*! \fn void confPrint()
  \brief 全パラメータを表示、ログに記録する。
  */
  void confPrint();


  /*! netcl */
  /*! \fn const char* netclVersion()
  \return netclのバージョン文字列を返す 
  */
  const char* netclVersion();

  /*! \fn void* netclInit(const char* serverName, const int serverPort)
  \brief 初期化を行う。
  \param[in] serverName 接続先サーバのホストネームまたはIP
  \param[in] serverPort 接続先サーバのポート
  \return netclパラメータ NULL:Error
  */
  void* netclInit(const char* serverName, const int serverPort);

  /*! \fn void* netclInitWithTimeOut(const char* serverName, const int serverPort, const double t)
  \brief タイムアウトつきで初期化を行う。
  \param[in] serverName 接続先サーバのホストネームまたはIP
  \param[in] serverPort 接続先サーバのポート
  \param[in] t タイムアウト時間 sec
  \return NULL netclパラメータ
  */
  void* netclInitWithTimeOut(const char* serverName, const int serverPort, const double t);

  /*! \fn int netclEnd(void* _param)
  \brief 終了処理を行う。
  \param[in] _param netclパラメータ
  \return 0:正常終了 -1:Error
  */
  int netclEnd(void* _param);

  /*! \fn void netclSetTimeOut(void* _param, double t)
  \brief 受信時のタイムアウトを定義する
  \param[in] _param netclパラメータ
  \param[in] t タイムアウト時間 sec
  */
  void netclSetTimeOut(void* _param, const double t);

  /*! \fn int netclWrite(void* _param, const unsigned char* buf, const size_t bufSize)
  \brief データ送信。
  \param[in] _param netclパラメータ
  \param[in] buf 送信データ
  \param[in] bufSize 送信データサイズ
  \return >=0:送信したバイト数　<0:Errror
  */
  int netclWrite(void* _param, const unsigned char* buf, const size_t bufSize);

  /*! \fn int netclRead(void* _param, unsigned char* buf, size_t bufSize)
  \brief データを受信する
  \param[in] _param netclパラメータ
  \param[in] buf 受信データ
  \param[in] bufSize 受信データ格納領域サイズ
  \return >=0:受信したバイト数 <0:Error
  */
  int netclRead(void* _param, unsigned char* buf, size_t bufSize);


  /*! netsv */
  /*! \fn const char* netsvVersion()
  \return  netsvのバージョン文字列を返す 
  */
  const char* netsvVersion();

  /*! \fn void* netsvInit(const int serverPort)
  \brief netsvを初期化する際に呼び出す。単一クライアントのみ受付可能。
  \param[in] serverPort 待ちうけするポート番号
  \return NULL:失敗 other:データ構造体のポインタ
  */
  void* netsvInit(const int serverPort);

  /*! \fn int netsvEnd(void* _param)
  \brief 終了時に呼び出す。
  \param[in] _param netsv パラメーター
  \return 0:Success
  */
  int netsvEnd(void* _param);

  /*! \fn int netsvWaiting(void* _param)
  \brief Server listening
  \param[in] _param 
  \retval 0 Success
  \retval -1 Not connection
  \retval -2 Select error catche
  \retval -3 Over processMax
  */
  int netsvWaiting(void* _param);

  /*! \fn void* netsvWaitingProcess(void* _param)
  \brief netsvパラメータを子プロセス用にコピーする。接続を複数行う場合に使用。
  \param[in] _param netsvのパラメーター
  \return NULL:error other:netsv pointer
  */
  void* netsvWaitingProcess(void* _param);

  /*! \fn int netsvDisconnect(void* _param)
  \brief ソケットを閉じる
  \param[in] _param netsvのパラメーター
  \return 0:成功
  */
  int netsvDisconnect(void* _param);

  /*! \fn void netsvSetTimeOut(void* _param, double t)
  \brief 受信時のタイムアウトを定義する
  \param[in] _param netclパラメータ
  \param[in] t タイムアウト時間。-1ならSelectをしない sec
  */
  void netsvSetTimeOut(void* _param, const double t);

  /*! \fn int netsvWrite(void* _param, const unsigned char* buf, const size_t bufSize)
  \brief データ送信。
  \param[in] _param netsvパラメータ
  \param[in] buf 送信データ
  \param[in] bufSize 送信データサイズ
  \return >=0 送信したバイト数
  \return <0 Errror
  */
  int netsvWrite(void* _param, const unsigned char* buf, const size_t bufSize);

  /*! \fn int netsvRead(void* _param, unsigned char* buf, size_t bufSize)
  \brief データを受信する
  \param[in] _param netsvパラメータ
  \param[in] buf 受信データ
  \param[in] bufSize 受信データ格納領域サイズ
  \return >=0 受信したバイト数
  \return <0 Error
  */
  int netsvRead(void* _param, unsigned char* buf, size_t bufSize);


  /*! netu */
  /*! \fn const char* netuVersion()
  \return version
  */
  const char* netuVersion();

  /*! \fn void netuCnvInt(unsigned char* pa)
  \brief LabViewとの通信時にビックエンディアンとの相互変換を行なう。
  \brief 4バイトの型に対応
  */
  void netuCnvInt(unsigned char* a);

  /*! \fn void netuCnvLong(unsigned char* pa)
  \brief LabViewとの通信時にビックエンディアンとの相互変換を行なう。
  \brief 8バイトの型に対応
  */
  void netuCnvLong(unsigned char* a);


  /*! rs232c */
  /*! \fn const char* rs232cVersion()
  \reutrn バージョン文字列
  */
  const char* rs232cVersion();

  /*! \fn int rs232cInit(int comNo, int bps, int byteSize, int parityBit, int stopBit)
  \brief rs232cライブラリを使用する際に最初に呼び出す
  \param[in] comNo comポート番号。整数1～
  \param[in] bps 通信速度
  \param[in] byteSize 7:7bit 8:8bit
  \param[in] parityBit 0:none 1:oddParity 2:evenParity
  \param[in] stopBit 0,1:1bit 2:2bit 3:1.5bit
  \return 0:Success other:Error
  */
  int rs232cInit(int comNo, int bps, int byteSize, int parityBit, int stopBit);

  /*! \fn int rs232cEnd(int comNo)
  \brief rs232cライブラリを終了させる際に呼ぶ
  \param[in] comNo comポート番号。整数1～
  \return 0:Success other:Error
  */
  int rs232cEnd(int comNo);

  /*! \fn int rs232cWrite(int comNo, unsigned char* buf, unsigned int bufSize)
  \brief RS-232Cからデータを出力する
  \param[in] comNo comポート番号。整数1～
  \param[in] buf 出力データへのポインタ
  \param[in] bufSize 出力データのバイトサイズ
  \return over0:writeSize under0:Error
  */
  int rs232cWrite(int comNo, unsigned char* buf, const unsigned int bufSize);

  /*! \fn int rs232cRead(int comNo, unsigned char* buf, unsigned int bufSize)
  \brief RS-232Cからデータを読込む
  \param[in] comNo comポート番号。整数1～
  \param[in] buf 読込みデータ格納領域
  \param[in] bufSize 読込みデータ格納領域のバイトサイズ
  \return[in] over0:ReadSize, under0:Error
  */
  int rs232cRead(int comNo, unsigned char* buf, const unsigned int bufSize);


  /*! tm */
  /*! \fn const char* tmVersion()
  \return バージョン文字列を返す
  */
  const char* tmVersion();

  /*! \fn void* tmInit()
  \brief tmを使用するときに最初に呼び出す。
  \return NULL:Error Other:データ構造体へのポインタ
  */
  void* tmInit();

  /*! int tmEnd(void* _p)
  \brief tmの使用を終了するときにメモリなどを開放する。
  \return 0:Success
  */
  int tmEnd(void* _p);

  /*! \fn int tmReset(void* _p)
  \brief 時刻をセットする
  \return 0:Success
  */
  int tmReset(void* _p);


  /*! \fn double tmGetLag(void* _p)
  \brief tmGetInit(), tmGetReset()されてからの時間を[sec]で返す
  \param[in] _p tmのパラメーター
  \return 時間 [sec]
  */
  double tmGetLag(void* _p);

  /*! \fn const char* tmGetTimeStr(void* _p, double offset)
  \brief 時間を取得する
  \param[in] _p tmパラメーター
  \param[in] offset オフセット秒。現在時刻から何秒後の時刻を文字列として出力するか。
  \return YYYYMMDDhhmmss.ssssss(22文字)形式で文字列を返す
  */
  const char* tmGetTimeStr(void* _p, double offset);

  /*! \fn double tmGetDiff(const time_t t, const double msec)
  \brief 指定された時刻の現在時刻との差を返す。
  \brief tmInit()による初期化の必要なし。
  \param[in] t time(t)によって得られる秒と同じ定義
  \param[in] msec msec
  \return 時刻の差(=[入力時刻]-[現在時刻])
  */
  double tmGetDiff(const time_t t, const double msec);

  /*! int tmSleepMSec(double msec)
  \brief 指定したマイクロ秒間待つ
  \brief tmInit()による初期化の必要なし。
  \param[in] msec 待機する時間。マイクロ秒
  \return 0:成功, -1:失敗
  */
  int tmSleepMSec(double msec);


  /*! u */
  /*! \fn const char* uVersion()
  \return version
  */
  const char* uVersion();

  /*! \fn int uInit(const char* projectName)
  \brief uで使用するログファイルネームを設定する
  \param[in] profectName ログファイルのパスと頭文字をつける
  \return 0 正常終了
  \return 1 エラー
  */
  int uInit(const char* projectName);

  /*! \fn int uEnd()
  \brief uの使用終了時に呼ぶ。
  \return 0 正常終了
  \return 1 エラー
  */
  int uEnd();

  /*! \fn const char uGetLogName()
  \reutrn LogFileName
  */
  const char* uGetLogName();

  /*! \fn void uLock()
  \brief ロックする。セマフォを取得する。
  */
  void uLock();

  /*! \fn void uUnLock()
  \brief ロックを解除する。セマフォを破棄する。
  */
  void uUnLock();

  /*! \fn FILE* uFp()
  \brief ファイルポインターを返す。
  */
  FILE* uFp();

  /*! \fn const char* uGetDate()
  \brief 呼び出した時の日時を文字列として返す。
  \return 時刻の文字列。
  */
  const char* uGetDate();

  /*! \fn const char* uGetTime()
  \brief 呼び出した時の時刻を文字列として返す。
  \return 時刻の文字列。
  */
  const char* uGetTime();

  /*! \fn int uSigFunc(void(*_sigFunc)(void))
  \brief シグナル発生時にコールバック関数_sigFuncを呼び出すよう設定する
  \brief _sigFuncはvoid _sigFunc(void)の関数でなければならない
  \return 0:成功 1:失敗
  */
  int uSigFunc(void(*_sigFunc)(void));

#ifdef __cplusplus
}
#endif

#endif /*!< __LIBTKB_H__ */

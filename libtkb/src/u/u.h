/*!
\file u.h
\author Y.Koide
\date 2006.11.27
\brief Utilities.
*/
#ifndef __U_H__
#define __U_H__

/*! \fn #define uMWrite(str, x1, x2, x3, x4)
\brief 標準出力とログファイルにメッセージを出力する。
\param[in] thisFile __FILE__で渡される呼び出し元のファイル名。
\param[in] fileLine __LINE__で渡される呼び出し元の行番号。
\param[in] str 表示文字列の書式。fprintfに準拠した書式。
\param[in] x1 書式strの中に含まれる変数1
\param[in] x2 書式strの中に含まれる変数1
\param[in] x3 書式strの中に含まれる変数1
\param[in] x4 書式strの中に含まれる変数1
*/
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

/*! \fn #define uEWrite(str, x1, x2, x3, x4)
\brief エラー出力とログファイルにエラーメッセージを出力し異常終了する。
\param[in] thisFile __FILE__で渡される呼び出し元のファイル名。
\param[in] fileLine __LINE__で渡される呼び出し元の行番号。
\param[in] str 表示文字列の書式。fprintfに準拠した書式。
\param[in] x1 書式strの中に含まれる変数1
\param[in] x2 書式strの中に含まれる変数1
\param[in] x3 書式strの中に含まれる変数1
\param[in] x4 書式strの中に含まれる変数1
*/
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

/*! \fn #define uM*(str)
\brief メッセージ出力用
\brief 標準出力とログファイルに出力する。
*/
#define uM(str) { uMWrite((str), NULL, NULL, NULL, NULL); }
#define uM1(str,x1) { uMWrite((str), (x1), NULL, NULL, NULL); }
#define uM2(str,x1,x2) { uMWrite((str), (x1), (x2), NULL, NULL); }
#define uM3(str,x1,x2,x3) { uMWrite((str), (x1), (x2), (x3), NULL); }
#define uM4(str,x1,x2,x3,x4) { uMWrite((str), (x1), (x2), (x3), (x4)); }

/*! \fn #define uE*(str)
\brief エラー出力用
\brief エラー出力とログファイルに出力する。
*/
#define uE(str) { uEWrite((str), NULL, NULL, NULL, NULL); }
#define uE1(str,x1) { uEWrite((str), (x1), NULL, NULL, NULL); }
#define uE2(str,x1,x2) { uEWrite((str), (x1), (x2), NULL, NULL, NULL); }
#define uE3(str,x1,x2,x3) { uEWrite((str), (x1), (x2), NULL, NULL); }
#define uE4(str,x1,x2,x3,x4) { uEWrite((str), (x1), (x2), (x3), NULL); }


#ifdef __cplusplus
//! C++のみ対応の宣言
template <class T> T uEndian(T* p);

extern "C"{

#endif

  //! C,C++に対応の宣言
  const char* uVersion();
  int uInit(const char* projectName);
  int uEnd();
  const char* uGetLogName();
  void uLock();
  void uUnLock();
  FILE* uFp();
  const char* uGetDate();
  const char* uGetTime();
  short uEndianShort(short* pa);
  int uEndianInt(int* pa);
  long long uEndianLongLong(long long* pa);
  int uSigFunc(void* _sigFunc);

#ifdef __cplusplus
}
#endif

#endif

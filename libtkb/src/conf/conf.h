/*!
\file conf.h
\author Y.Koide
\date 2006.08.16
\brief パラメーターを読み込むためのライブラリ

*使い方*
初期化時にconfInit()を呼び出し、
使用後にconfEnd()にて終了する。
パラメーターを読み込むには、
・ファイルからはconfAddFile()
・文字列からはconfAddStr()
パラメーターを取得するには、
・confGetStr()にてパラメータを指定して文字列を取得する。
・confSetKey()でパラメーター名を設定直後、
　confGetVal()にて文字列を取得する。
・confGetAllKeyVal()にて全パラメータリストを取得する。
　キーと値データは\tにてセパレートされている。
confPrint()で画面にパラメーター一覧を表示できる。

*パラメータの書式*
・#はこの後の行末までコメントとして無視される。
・一行１つのパラメーターが設定でき、
　KEYWORD[空文字(' 'or'\t')]VALUE[\n\r\aEOFのどれか(どれか2つも可)]
　の書式で記入していなければならない。
・一行の最大文字数は1024文字であり、超えた場合、超えた分を除外したものを一行として扱う
・KEYWORDは空文字やダブルコーテーション(")を含んではならない。
・VALUEは空文字を含んではならない。
　ただし、VALUEが"で始った場合、VALUE内で２番目の"までの間の値をVALUEに格納し、
　""の間にある空文字もVALUEに含むことができる。
*/

#ifndef __CONF_H__
#define __CONF_H__
#ifdef __cplusplus
extern "C"{
#endif

  const char* confVersion();
  int confInit();
  int confAddFile(const char* fileName);
  int confAddStr(const char* str);
  const char* confGetStr(const char* key);
  int confSetKey(const char* _setKey);
  const char* confGetVal();
  const char* confGetAllKeyVal();
  void confPrint();

#ifdef __cplusplus
}
#endif
#endif

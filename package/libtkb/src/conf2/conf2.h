/*!
\file conf.h
\auther Y.Koide
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

  const char* conf2Version();
  void* conf2Init();
  int conf2End(void* _p);
  int conf2AddFile(void* _p, const char* fileName);
  int conf2AddStr(void* _p, const char* str);
  const char* conf2GetStr(void* _p, const char* key);
  int conf2SetKey(void* _p, const char* _setKey);
  const char* conf2GetVal(void* _p);
  const char* conf2GetAllKeyVal(void* _p);
  const char** conf2GetAllKeyVal2(void* _p);
  void conf2Print(void* _p);

#ifdef __cplusplus
}
#endif
#endif

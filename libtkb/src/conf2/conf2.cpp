/*!
\file conf.cpp
\auther Y.Koide
\date 2006.08.16
\brief パラメーターを読み込むためのライブラリ
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../u/u.h"
#include "conf2.h"

const char version[] = "0.0.0";

typedef struct{
  char** conf;              //!< パラメータ情報格納用
  int confNum;              //!< パラメータの数。
  int confNumMax;           //!< confに領域確保している数(実際はパラメータ名と値があるのでこの数値の2倍確保している)
  char* allKeyVal;          //!< 全てのキーと値を格納
  int valPos;               //!< 該当する値の位置
}tConfParam; //!< パラメータ構造体のタイプ宣言

static int addLine(void* _p, const char* line);
static int findPos(void* _p, const char* key);
static int pushKeyVal(void* _p, const char* _key, const char* _val);

#if 0
int main(){
  void* conf = conf2Init();
  conf2AddFile(conf, "test.conf");
  conf2AddStr(conf, "abc 192");
  conf2Print(conf);
  conf2End(conf);
}
#endif

/*! \fn const char* conf2Version()
\return バージョン情報を記述した文字列。
*/
const char* conf2Version()
{
  return version;
}

/*! \fn int conf2Init()
\brief 最初に呼び出す。
\return データ構造体へのポインタ
*/
void* conf2Init()
{
  tConfParam* p; //!< データ構造体へのポインタ

  p = new tConfParam();
  if(!p)
  {
    return NULL;
  }
  p->conf = NULL;
  p->confNum = 0;
  p->allKeyVal = NULL;
  p->valPos = -1;

  return p;
}

/*! \fn int conf2End(void* _p)
\breif 最後に必ず呼び出す。
*/
int conf2End(void* _p)
{
  tConfParam* p = (tConfParam*)_p;

  if(p)
  {
    if(p->allKeyVal)
    {
      delete[] p->allKeyVal;
    }
    if(p->conf)
    {
      for(int i = 0; i < p->confNumMax * 2; i++)
      {
        delete[] p->conf[i];
      }
      delete[] p->conf;
    }
    delete p;
    p = NULL;
  }
  return 0;
}

/*! \fn int conf2AddFile(const char* fileName)
\breif ファイルから設定情報を追加取得する。
\param[in] fileName ファイル名
\retval 0 成功
\retval 1 失敗
*/
int conf2AddFile(void* _p, const char* fileName)
{
  FILE* fp;
  int tmpSize = 256;
  char* tmp;
  char* tmp2;
  int c;
  int size;
 
  fp = fopen(fileName, "rb");
  if(fp == NULL)
  {
    return 1;
  }
  
  tmp = new char[tmpSize];

  size = 0;
  while(1){
    c = fgetc(fp);
    
    //! ファイル終端
    if(c == EOF){
      if(size != 0){
	tmp[size] = '\0';
	addLine(_p, tmp);
      }
      break;
    }
    
    //! 改行コードが見つかったらパラメータ追加
    if(c == '\n' || c == '\r' || c == '\a'){
      tmp[size] = '\0';
      addLine(_p, tmp);
      size = 0;
    }
    else{
      tmp[size] = (char)c;
      size++;
    }

    //! 格納領域をオーバーしたら新たに2倍の領域を作成し移動する
    if(size >= tmpSize){
      tmp2 = new char[tmpSize * 2];
      memcpy(tmp2, tmp, size);
      delete[] tmp;
      tmp = tmp2;
      tmpSize *= 2;
    }
  }
  fclose(fp);

  delete[] tmp;
  return 0;
}

/*! \fn int conf2AddStr(const char* str)
\brief 改行を含む文字列から設定情報を追加取得する。
\param[in] str 文字列
\retval 0 成功
*/
int conf2AddStr(void* _p, const char* str){
  char* tmp;                     //!< パラメータを1行ごとに格納する領域。
  const char* s;                 //!< strで行を検索していくためのポインタ。
  const char* a;                 //!< 改行コードを検索するためのポインタ
  const char* line;              //!< 検出した行の先頭ポインタ
  int len;                       //!< 検出した行の文字数


  s = str;
  while(strlen(s) > 0)
  {
    line = s;

    //! 改行コードを探して一行だけtmpにコピーする。
    if((a = strchr(s, '\n')) != NULL)      //!< unix,windowsの改行コード
    {
      len = a - s + 1;                     //!< \nの手前の文字まで何文字か
      s += len;                            //!< 次の行頭に移動
    }
    else if((a = strchr(s, '\a')) != NULL) //!< macの改行コード
    {
      len = a - s + 1;                     //!< \nの手前の文字まで何文字か
      s += len;                            //!< 次の行頭に移動
    }
    else{
      //! 最後の行
      len = strlen(s);
      s += len;
    }

    //! パラメータの追加
    tmp = new char[len +1];
    strncpy(tmp, line, len);
    tmp[len] = '\0';
    addLine(_p, tmp);
    
    delete[] tmp;
  }

  return 0;
}

/*! \fn const char* conf2GetStr(const char* _setKey)
\brief パラメータ名からパラメータ値を取得する
\param[in] _setKey パラメータ名
\return 設定値
*/
const char* conf2GetStr(void* _p, const char* _setKey)
{
  tConfParam* p = (tConfParam*)_p;
  
  int pos = findPos(_p, _setKey);
  if(pos < 0){
    return NULL;
  }
  return p->conf[pos * 2 + 1];
}

/*! \fn int conf2SetKey(const char* _setKey)
\brief confGetVal()で取得するパラメータ名を指定する。
\param[in] _setKey パラメータ名
\retval 1 該当あり
\retval 0 該当なし
*/
int conf2SetKey(void* _p, const char* _setKey)
{
  tConfParam* p = (tConfParam*)_p;
  
  //! 該当パラメータを探す
  int pos = findPos(_p, _setKey);
  if(pos < 0){
    //! 該当なし
    p->valPos = -1;
    return 0;
  }
  //! 該当あり
  p->valPos = pos;
  return 1;
}

/*! \fn const char* conf2GetVal()
\brief パラメータ値を取得する。パラメータ名の設定はconfSetKeyで行う。
\return パラメータ名
*/
const char* conf2GetVal(void* _p)
{
  tConfParam* p = (tConfParam*)_p;

  if(p->valPos < 0){
    return NULL;
  }
  return p->conf[p->valPos * 2 + 1];  
}

/*! \fn const char* conf2GetAllKeyVal()
\brief 全てのパラメータ名とパラメータ値を取得する。
\brief パラメータ名とパラメータ値は\tで区切られ、一行にパラメータ項目１つを書き出す。
\return パラメータ名とパラメータ値の一覧
*/
const char* conf2GetAllKeyVal(void* _p)
{
  tConfParam* p = (tConfParam*)_p;
  size_t size = 1; //! \0用

  if(p->confNum == 0){
    return NULL;
  }

  //! 全パラメータを書き出したときのサイズを算出する。
  for(int i = 0; i < p->confNum; i++)
  {
    size += strlen(p->conf[i * 2]) + strlen(p->conf[i * 2 + 1]) + 1 + 2; //!< \tと改行コード分(2byte)
  }

  //! 書き出しようの領域を確保
  if(p->allKeyVal)
  {
    delete[] p->allKeyVal;
  }
  p->allKeyVal = new char[size];

  //! 書き出し
  char* pTmp = p->allKeyVal;
  for(int i = 0; i < p->confNum; i++)
  {
    sprintf(pTmp, "%s\t%s\n", p->conf[i * 2], p->conf[i * 2 + 1]);
    pTmp += strlen(pTmp);
  }

  return p->allKeyVal;
}

/*! \fn const char* conf2GetAllKeyVal2()
\brief 全てのパラメータ名とパラメータ値を取得する。
\return パラメータ名とパラメータ値が1セットづつ格納されている。key1 val1 key2 val2 ...
*/
const char** conf2GetAllKeyVal2(void* _p)
{
  tConfParam* p = (tConfParam*)_p;
  return (const char**)(p->conf);
}

/*! \fn void conf2Print()
\brief パラメータ名とパラメータ値の一覧を表示する。
*/
void conf2Print(void* _p){
  tConfParam* p = (tConfParam*)_p;
  
  const char* kv = conf2GetAllKeyVal((void*)p);
  if(kv){
    uM1("===== ===== =====\n%s===== ===== =====", kv);
  }
}


/*! \fn int addLine(const char* line)
\brief 1行の文字列から設定情報を取得する。
\param[in] line 文字列
\retval 0 成功
\retval 1 失敗
*/
int addLine(void* _p, const char* line){
  tConfParam* p = (tConfParam*)_p;
  char* tmp;         //!< パラメータを1行ごとに格納する領域。
  char* tmp2;
  char* key;
  char* val;
  const char* ln;
  const char* a;
  const char* b;
  char* pTmp;
  int len;

  tmp = new char[strlen(line) + 1];
  tmp2 = new char[strlen(line) + 1];

  //! コメント&末端処理
  strcpy(tmp, line);
  ln = tmp;

  if((a = strchr(ln, '#')) != NULL)
  {
    len = a - ln;            //!< #直前までの文字数
    strncpy(tmp2, ln, len);
    tmp2[len] = '\0';
    strcpy(tmp, tmp2);
    ln = tmp;
  }
  if((a = strchr(ln, '\n')) != NULL)
  {
    len = a - ln;            //!< \n直前までの文字数
    strncpy(tmp2, ln, len);
    tmp2[len] = '\0';
    strcpy(tmp, tmp2);
    ln = tmp;
  }
  if((a = strchr(ln, '\r')) != NULL)
  {
    len = a - ln;            //!< \r直前までの文字数
    strncpy(tmp2, ln, len);
    tmp2[len] = '\0';
    strcpy(tmp, tmp2);
    ln = tmp;
  }
  if((a = strchr(ln, '\a')) != NULL)
  {
    len = a - ln;            //!< \a直前までの文字数
    strncpy(tmp2, ln, len);
    tmp2[len] = '\0';
    strcpy(tmp, tmp2);
    ln = tmp;
  }
  //! 行頭の空文字削除
  pTmp = tmp;
  len = 0;
  while(*pTmp == ' ' || *pTmp == '\t')
  {
    len++;
    pTmp++;
  }
  strcpy(tmp2, pTmp);
  strcpy(tmp, tmp2);

  if(strlen(tmp) <= 0)
  {
    delete[] tmp;
    delete[] tmp2;
    return 1;
  }
  
  //! パラメータ名とパラメータ値を区切る空字を検索する
  ln = tmp;
  a = strchr(ln, ' ');
  b = strchr(ln, '\t');
  if(a != NULL && b != NULL)
  {
    if(a < b)
    {
      len = a - ln;
    }
    else
    {
      len = b - ln;
    }
  }
  else if(a != NULL)
  {
    len = a - ln;
  }
  else if(b != NULL)
  {
    len = b - ln;
  }
  else
  {
    //! 検索失敗
    delete[] tmp;
    delete[] tmp2;
    return 1;
  }

  //! 領域確保
  key = new char[len + 1];
  val = new char[strlen(ln + len) + 1];

  //! パラメータ名とパラメータ値取得
  strncpy(key, ln, len);
  key[len] = '\0';
  strcpy(val, ln + len);

  //! valの先頭空文字削除
  pTmp = val;
  len = 0;
  while(*pTmp == ' ' || *pTmp == '\t'){
    len++;
    pTmp++;
  }
  strcpy(tmp2, pTmp);
  strcpy(val, tmp2);

  if(strlen(val) <= 0)
  {
    delete[] tmp;
    delete[] tmp2;
    delete[] key;
    delete[] val;
    return 1;
  }

  //! パラメーターの追加
  //! 同じkeyが登録されていた場合上書きする
  int pos = findPos(_p, key);
  if(pos < 0)
  {
    //! 新規
    pushKeyVal(_p, key, val);
  }
  else
  {
    //! 上書き
    delete[] p->conf[pos * 2 + 1];
    p->conf[pos * 2 + 1] = new char[strlen(val) + 1];
    strcpy(p->conf[pos * 2 + 1], val);
  }

  delete[] tmp;
  delete[] tmp2;
  delete[] key;
  delete[] val;

  return 0;
}

int findPos(void* _p, const char* key)
{
  tConfParam* p = (tConfParam*)_p;
  
  for(int i = 0; i < p->confNum; i++)
  {
    if(strcmp(p->conf[i * 2], key) == 0){
      return i;
    }
  }
  return -1;
}

int pushKeyVal(void* _p, const char* _key, const char* _val)
{
  tConfParam* p = (tConfParam*)_p;
  int num = p->confNum;
  char** tmp;

  num++;
  //! 格納領域がオーバーした場合は大き目の拡張領域を確保し、移動する
  if(num >= p->confNumMax - 1)
  {
    if(p->confNum == 0)
    {
      //! 最初に呼び出されたときは、適当に大きめに確保してやる。
      p->confNumMax = 256;
    }
    else
    {
      //! その後は2倍ずつ確保する。
      p->confNumMax *= 2;
    }

    //! 領域確保
    tmp = new char*[p->confNumMax * 2];
    memset(tmp, NULL, sizeof(*tmp) * p->confNumMax * 2);

    //! 旧データの文字列位置のコピー
    memcpy(tmp, p->conf, sizeof(*tmp) * 2 * p->confNum);
    //for(int i = 0; i < p->confNum * 2; i++)
    //{
    //  tmp[i] = p->conf[i];
    //}
    
    //! 残りは初期化
    //memset(tmp + p->confNum * 2, NULL, sizeof(*tmp) * (p->confNumMax - p->confNum) * 2);
    //for(int i = p->confNum * 2; i <  p->confNumMax * 2; i++)
    //{
    //  tmp[i] = NULL;
    //}

    if(p->conf)
    {
      delete[] p->conf;
    }
    p->conf = tmp;
  }
  p->conf[p->confNum * 2] = new char[strlen(_key) + 1];
  p->conf[p->confNum * 2 + 1] = new char[strlen(_val) + 1];
  strcpy(p->conf[p->confNum * 2], _key);
  strcpy(p->conf[p->confNum * 2 + 1], _val);

  p->confNum = num;
  return 0;
}

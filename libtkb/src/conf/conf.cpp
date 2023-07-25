/*!
\file conf.cpp
\author Y.Koide
\date 2006.08.16
\brief パラメーターを読み込むためのライブラリ
*/

#include <stdio.h>
#include <stdlib.h>
#include <map>
#include <string>
#include <iostream>
using namespace std;
#include "../u/u.h"
#include "conf.h"

const char version[] = "0.0.0";

static std::map<string, string> conf;///設定データ格納用
static string allKeyVal;///全てのキーと値を格納
string setKey;///キーを一時的に格納する

static int confAddLine(const char* line);

/*! \fn const char* confVersion();
\return confのバージョン文字列を返す 
*/
const char* confVersion(){
  return version;
}

/*! \fn int confInit()
\brief confの初期化を行う。
\return 0:Success
*/
int confInit(){
  conf.clear();
  return 0;
}

/*! \fn int confAddFile(const char* fileName)
\brief ファイルからパラメータを取得する
\param[in] fileName ファイル名込みのパス
\reutrn 0:Success 1:Fault
*/
int confAddFile(const char* fileName){
  FILE* fp;
  string line;
  int c;

  if(!(fp=fopen(fileName,"rb"))){
    return 1;
  }

  while(1){
    c = fgetc(fp);
    //while(fgets(_tmp, MAX_STR_LEN, fp)){
    
    if(c == EOF){
      if(!line.empty()){
	confAddLine(line.c_str());
      }
      break;
    }

    if(c == '\n' || c == '\r' || c == '\a'){
      if(!line.empty()){
	confAddLine(line.c_str());
	line.clear();
      }
    }
    else{
      line += (char)c;
    }
  }
  fclose(fp);
  return 0;
}

/*! \fn int confAddStr(const char* str)
\brief 1行もしくは複数行の文字列からパラメータを取得する
\param[in] str パラメータの記された文字列
\reutrn 0:Success 1:Fault
*/
int confAddStr(const char* str){
  string tmp,s;
  int i;

  s=str;
  while(s.size() > 0){
    tmp.clear();
    if((i=(int)s.find('\n'))!=-1){
      tmp=s.substr(0,i);
    }
    else if((i=(int)s.find('\a'))!=-1){
      tmp=s.substr(0,i);
    }
    else{
      tmp=s;
    }
    //else if((i=(int)s.find('\0'))!=-1){
    //  tmp=tmp.substr(0,i);
    //}
    confAddLine(tmp.c_str());
    //printf("confAddStr %s\n", tmp.c_str());
    if(i < s.size()){
      s=s.substr(i+1,s.size()-i-1);
    }
    else{
      break;
    }
  }
  return 0;
}

int confAddLine(const char* line){
  string tmp, key, val;
  int i,j,min;

  tmp=line;
  //コメント&末端処理
  if((i=(int)tmp.find('#'))!=-1){
    tmp=tmp.substr(0,i);
  }
  if((i=(int)tmp.find('\n'))!=-1){
    tmp=tmp.substr(0,i);
  }
  if((i=(int)tmp.find('\r'))!=-1){
    tmp=tmp.substr(0,i);
  }
  if((i=(int)tmp.find('\a'))!=-1){
    tmp=tmp.substr(0,i);
  }
  //if((i=(int)tmp.find('\0'))!=-1){
  //  tmp=tmp.substr(0,i);
  //}
  //if(tmp.empty()){
  //  tmp=line;
  //}
  //行頭の空文字削除
  while(tmp[0]==' ' || tmp[0]=='\t'){
    tmp = tmp.substr(1,tmp.size()-1);
  }
  if(tmp.empty())
    return -1;

  //スプリッター検索
  i=(int)tmp.find(' ');
  j=(int)tmp.find('\t');
  if(i>0 && j>0){
    if(i<j)
      min=i;
    else
      min=j;
  }
  else if(i>0 && j==-1){
    min=i;
  }
  else if(i==-1 && j>0){
    min=j;
  }
  else if(i==-1 && j==-1){
    return -1;
  }
  key=tmp.substr(0,min);
  val=tmp.substr(min+1, tmp.size()-min-1);
  //val=tmp.substr(min+1, tmp.size()-1);

  //valの先頭空文字削除
  while(val[0]==' ' || val[0]=='\t'){
    val = val.substr(1,val.size()-1);
  }
  //if(val[0]=='"'){
  //  //valが"で始まっていれば対応するはじめの"の中身をvalとする
  //  //対応する"(""以外)が見つからなければエラー
  //  //"内の「""」は"として扱う
  //  tmp = val.substr(1,val.size()-1);
  //  val.clear();
  //  while((i=(int)tmp.find('"'))!=-1){
  //    if(tmp.find("\"\"")!=i){
  //      val+=tmp.substr(0,i);
  //      break;
  //    }
  //    else{
  //      val+=tmp.substr(0, i)+'"';//""を"に変換して取り込んでいる
  //      if(tmp.size()<=i+2)
  //        break;
  //      tmp=tmp.substr(i+2, tmp.size()-i-2);
  //    }
  //  }
  //}
  //else{
  //  //val内の空文字以降を排除
  //  i=(int)val.find(' ');
  //  j=(int)val.find('\t');
  //  if(i>0 && j>0){
  //    if(i<j)
  //      min=i;
  //    else
  //      min=j;
  //  }
  //  else if(i>0 && j==-1){
  //    min=i;
  //  }
  //  else if(i==-1 && j>0){
  //    min=j;
  //  }
  //  else if(i==-1 && j==-1){
  //    //valに空文字は含まれない
  //    min=val.size();
  //  }
  //  val=val.substr(0,min);
  //}
  if(val.empty()){
    //printf("confInit(); val error!![%s]\n", val.c_str());
    return -1;
  }

  //パラメーターの追加
  //同じkeyが登録されていた場合上書きする
  if(conf.find(key)==conf.end()){
    conf.insert(map<string, string>::value_type(key, val));
  }
  else{
    conf.find(key)->second=val;
  }
  //printf(" %s\t%s\n",key.c_str(),val.c_str());
  return 0;
}

/*! \fn const char* confGetStr(const char* _setKey)
\brief _setKeyに対応するパラメータの値を取得する。
\param[in] _setKey キーとなる文字列
\return NULL:Fault other:値の入った文字列へのポインタ
*/
const char* confGetStr(const char* _setKey){
  string key=_setKey;

  if(conf.find(key) == conf.end()){
    return NULL;
  }
  return conf.find(key)->second.c_str();  
}

/*! \fn int confSetKey(const char* _setKey)
\brief confGetVal();で値を取り出すために設定するキーの登録
\param[in] _setKey キーとなる文字列
\return 0:Fault 1:Success
*/
int confSetKey(const char* _setKey){
  setKey=_setKey;

  if(conf.find(setKey) == conf.end()){
    return 0;
  }
  return 1;
}

/*! \fn const char* confGetVal()
\brief confSetKey();で設定されたキーに対応するパラメータの値を取得する
\return NULL:Fault other:対応する文字列へのポインタ
*/
const char* confGetVal(){
  if(conf.find(setKey) == conf.end()){
    return NULL;
  }
  return conf.find(setKey)->second.c_str();  
}

/*! \fn const char* confGetAllKeyVal()
\return 全パラメータの文字列。1行にキーと値が一組ずつ入っている。
*/
const char* confGetAllKeyVal(){
  map<string, string>::iterator itrConf;
  string tmp, tmp2;

  allKeyVal.clear();
  for(itrConf=conf.begin(); itrConf!=conf.end(); itrConf++){
    tmp=itrConf->second;
    //! スペースやタブが含まれる場合は'"'で囲う
    //if(((int)tmp.find(' '))!=-1 || ((int)tmp.find('\t'))!=-1){
    //  //! さらに、"が含まれる場合は""に変換する
    //  while((i=(int)tmp.find('"'))!=-1){
    //    tmp2=tmp.substr(0,i)+'"'+'"';
    //    if(tmp.size()==i+1){
    //      tmp.clear();
    //      break;
    //    }
    //    tmp=tmp.substr(i+1,tmp.size()-i-1);
    //  }
    //  tmp='"' + tmp2 + tmp + '"';
    //}
    allKeyVal += itrConf->first + "\t" + tmp + "\n";
  }
  //printf("%s", allKeyVal.c_str());
  return allKeyVal.c_str();
}

/*! \fn void confPrint()
\brief 全パラメータを表示、ログに記録する。
*/
void confPrint(){
  string tmp;
  tmp = confGetAllKeyVal();
  tmp = "===== ===== =====\n" + tmp + "===== ===== =====";
  uM1("%s", tmp.c_str());
}

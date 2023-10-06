/*!
\file conf.cpp
\author Y.Koide
\date 2006.08.16
\brief �p�����[�^�[��ǂݍ��ނ��߂̃��C�u����
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

static std::map<string, string> conf;///�ݒ�f�[�^�i�[�p
static string allKeyVal;///�S�ẴL�[�ƒl���i�[
string setKey;///�L�[���ꎞ�I�Ɋi�[����

static int confAddLine(const char* line);

/*! \fn const char* confVersion();
\return conf�̃o�[�W�����������Ԃ� 
*/
const char* confVersion(){
  return version;
}

/*! \fn int confInit()
\brief conf�̏��������s���B
\return 0:Success
*/
int confInit(){
  conf.clear();
  return 0;
}

/*! \fn int confAddFile(const char* fileName)
\brief �t�@�C������p�����[�^���擾����
\param[in] fileName �t�@�C�������݂̃p�X
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
\brief 1�s�������͕����s�̕����񂩂�p�����[�^���擾����
\param[in] str �p�����[�^�̋L���ꂽ������
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
  //�R�����g&���[����
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
  //�s���̋󕶎��폜
  while(tmp[0]==' ' || tmp[0]=='\t'){
    tmp = tmp.substr(1,tmp.size()-1);
  }
  if(tmp.empty())
    return -1;

  //�X�v���b�^�[����
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

  //val�̐擪�󕶎��폜
  while(val[0]==' ' || val[0]=='\t'){
    val = val.substr(1,val.size()-1);
  }
  //if(val[0]=='"'){
  //  //val��"�Ŏn�܂��Ă���ΑΉ�����͂��߂�"�̒��g��val�Ƃ���
  //  //�Ή�����"(""�ȊO)��������Ȃ���΃G���[
  //  //"���́u""�v��"�Ƃ��Ĉ���
  //  tmp = val.substr(1,val.size()-1);
  //  val.clear();
  //  while((i=(int)tmp.find('"'))!=-1){
  //    if(tmp.find("\"\"")!=i){
  //      val+=tmp.substr(0,i);
  //      break;
  //    }
  //    else{
  //      val+=tmp.substr(0, i)+'"';//""��"�ɕϊ����Ď�荞��ł���
  //      if(tmp.size()<=i+2)
  //        break;
  //      tmp=tmp.substr(i+2, tmp.size()-i-2);
  //    }
  //  }
  //}
  //else{
  //  //val���̋󕶎��ȍ~��r��
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
  //    //val�ɋ󕶎��͊܂܂�Ȃ�
  //    min=val.size();
  //  }
  //  val=val.substr(0,min);
  //}
  if(val.empty()){
    //printf("confInit(); val error!![%s]\n", val.c_str());
    return -1;
  }

  //�p�����[�^�[�̒ǉ�
  //����key���o�^����Ă����ꍇ�㏑������
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
\brief _setKey�ɑΉ�����p�����[�^�̒l���擾����B
\param[in] _setKey �L�[�ƂȂ镶����
\return NULL:Fault other:�l�̓�����������ւ̃|�C���^
*/
const char* confGetStr(const char* _setKey){
  string key=_setKey;

  if(conf.find(key) == conf.end()){
    return NULL;
  }
  return conf.find(key)->second.c_str();  
}

/*! \fn int confSetKey(const char* _setKey)
\brief confGetVal();�Œl�����o�����߂ɐݒ肷��L�[�̓o�^
\param[in] _setKey �L�[�ƂȂ镶����
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
\brief confSetKey();�Őݒ肳�ꂽ�L�[�ɑΉ�����p�����[�^�̒l���擾����
\return NULL:Fault other:�Ή����镶����ւ̃|�C���^
*/
const char* confGetVal(){
  if(conf.find(setKey) == conf.end()){
    return NULL;
  }
  return conf.find(setKey)->second.c_str();  
}

/*! \fn const char* confGetAllKeyVal()
\return �S�p�����[�^�̕�����B1�s�ɃL�[�ƒl����g�������Ă���B
*/
const char* confGetAllKeyVal(){
  map<string, string>::iterator itrConf;
  string tmp, tmp2;

  allKeyVal.clear();
  for(itrConf=conf.begin(); itrConf!=conf.end(); itrConf++){
    tmp=itrConf->second;
    //! �X�y�[�X��^�u���܂܂��ꍇ��'"'�ň͂�
    //if(((int)tmp.find(' '))!=-1 || ((int)tmp.find('\t'))!=-1){
    //  //! ����ɁA"���܂܂��ꍇ��""�ɕϊ�����
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
\brief �S�p�����[�^��\���A���O�ɋL�^����B
*/
void confPrint(){
  string tmp;
  tmp = confGetAllKeyVal();
  tmp = "===== ===== =====\n" + tmp + "===== ===== =====";
  uM1("%s", tmp.c_str());
}

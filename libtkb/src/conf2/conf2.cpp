/*!
\file conf.cpp
\auther Y.Koide
\date 2006.08.16
\brief �p�����[�^�[��ǂݍ��ނ��߂̃��C�u����
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../u/u.h"
#include "conf2.h"

const char version[] = "0.0.0";

typedef struct{
  char** conf;              //!< �p�����[�^���i�[�p
  int confNum;              //!< �p�����[�^�̐��B
  int confNumMax;           //!< conf�ɗ̈�m�ۂ��Ă��鐔(���ۂ̓p�����[�^���ƒl������̂ł��̐��l��2�{�m�ۂ��Ă���)
  char* allKeyVal;          //!< �S�ẴL�[�ƒl���i�[
  int valPos;               //!< �Y������l�̈ʒu
}tConfParam; //!< �p�����[�^�\���̂̃^�C�v�錾

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
\return �o�[�W���������L�q����������B
*/
const char* conf2Version()
{
  return version;
}

/*! \fn int conf2Init()
\brief �ŏ��ɌĂяo���B
\return �f�[�^�\���̂ւ̃|�C���^
*/
void* conf2Init()
{
  tConfParam* p; //!< �f�[�^�\���̂ւ̃|�C���^

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
\breif �Ō�ɕK���Ăяo���B
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
\breif �t�@�C������ݒ����ǉ��擾����B
\param[in] fileName �t�@�C����
\retval 0 ����
\retval 1 ���s
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
    
    //! �t�@�C���I�[
    if(c == EOF){
      if(size != 0){
	tmp[size] = '\0';
	addLine(_p, tmp);
      }
      break;
    }
    
    //! ���s�R�[�h������������p�����[�^�ǉ�
    if(c == '\n' || c == '\r' || c == '\a'){
      tmp[size] = '\0';
      addLine(_p, tmp);
      size = 0;
    }
    else{
      tmp[size] = (char)c;
      size++;
    }

    //! �i�[�̈���I�[�o�[������V����2�{�̗̈���쐬���ړ�����
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
\brief ���s���܂ޕ����񂩂�ݒ����ǉ��擾����B
\param[in] str ������
\retval 0 ����
*/
int conf2AddStr(void* _p, const char* str){
  char* tmp;                     //!< �p�����[�^��1�s���ƂɊi�[����̈�B
  const char* s;                 //!< str�ōs���������Ă������߂̃|�C���^�B
  const char* a;                 //!< ���s�R�[�h���������邽�߂̃|�C���^
  const char* line;              //!< ���o�����s�̐擪�|�C���^
  int len;                       //!< ���o�����s�̕�����


  s = str;
  while(strlen(s) > 0)
  {
    line = s;

    //! ���s�R�[�h��T���Ĉ�s����tmp�ɃR�s�[����B
    if((a = strchr(s, '\n')) != NULL)      //!< unix,windows�̉��s�R�[�h
    {
      len = a - s + 1;                     //!< \n�̎�O�̕����܂ŉ�������
      s += len;                            //!< ���̍s���Ɉړ�
    }
    else if((a = strchr(s, '\a')) != NULL) //!< mac�̉��s�R�[�h
    {
      len = a - s + 1;                     //!< \n�̎�O�̕����܂ŉ�������
      s += len;                            //!< ���̍s���Ɉړ�
    }
    else{
      //! �Ō�̍s
      len = strlen(s);
      s += len;
    }

    //! �p�����[�^�̒ǉ�
    tmp = new char[len +1];
    strncpy(tmp, line, len);
    tmp[len] = '\0';
    addLine(_p, tmp);
    
    delete[] tmp;
  }

  return 0;
}

/*! \fn const char* conf2GetStr(const char* _setKey)
\brief �p�����[�^������p�����[�^�l���擾����
\param[in] _setKey �p�����[�^��
\return �ݒ�l
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
\brief confGetVal()�Ŏ擾����p�����[�^�����w�肷��B
\param[in] _setKey �p�����[�^��
\retval 1 �Y������
\retval 0 �Y���Ȃ�
*/
int conf2SetKey(void* _p, const char* _setKey)
{
  tConfParam* p = (tConfParam*)_p;
  
  //! �Y���p�����[�^��T��
  int pos = findPos(_p, _setKey);
  if(pos < 0){
    //! �Y���Ȃ�
    p->valPos = -1;
    return 0;
  }
  //! �Y������
  p->valPos = pos;
  return 1;
}

/*! \fn const char* conf2GetVal()
\brief �p�����[�^�l���擾����B�p�����[�^���̐ݒ��confSetKey�ōs���B
\return �p�����[�^��
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
\brief �S�Ẵp�����[�^���ƃp�����[�^�l���擾����B
\brief �p�����[�^���ƃp�����[�^�l��\t�ŋ�؂��A��s�Ƀp�����[�^���ڂP�������o���B
\return �p�����[�^���ƃp�����[�^�l�̈ꗗ
*/
const char* conf2GetAllKeyVal(void* _p)
{
  tConfParam* p = (tConfParam*)_p;
  size_t size = 1; //! \0�p

  if(p->confNum == 0){
    return NULL;
  }

  //! �S�p�����[�^�������o�����Ƃ��̃T�C�Y���Z�o����B
  for(int i = 0; i < p->confNum; i++)
  {
    size += strlen(p->conf[i * 2]) + strlen(p->conf[i * 2 + 1]) + 1 + 2; //!< \t�Ɖ��s�R�[�h��(2byte)
  }

  //! �����o���悤�̗̈���m��
  if(p->allKeyVal)
  {
    delete[] p->allKeyVal;
  }
  p->allKeyVal = new char[size];

  //! �����o��
  char* pTmp = p->allKeyVal;
  for(int i = 0; i < p->confNum; i++)
  {
    sprintf(pTmp, "%s\t%s\n", p->conf[i * 2], p->conf[i * 2 + 1]);
    pTmp += strlen(pTmp);
  }

  return p->allKeyVal;
}

/*! \fn const char* conf2GetAllKeyVal2()
\brief �S�Ẵp�����[�^���ƃp�����[�^�l���擾����B
\return �p�����[�^���ƃp�����[�^�l��1�Z�b�g�Âi�[����Ă���Bkey1 val1 key2 val2 ...
*/
const char** conf2GetAllKeyVal2(void* _p)
{
  tConfParam* p = (tConfParam*)_p;
  return (const char**)(p->conf);
}

/*! \fn void conf2Print()
\brief �p�����[�^���ƃp�����[�^�l�̈ꗗ��\������B
*/
void conf2Print(void* _p){
  tConfParam* p = (tConfParam*)_p;
  
  const char* kv = conf2GetAllKeyVal((void*)p);
  if(kv){
    uM1("===== ===== =====\n%s===== ===== =====", kv);
  }
}


/*! \fn int addLine(const char* line)
\brief 1�s�̕����񂩂�ݒ�����擾����B
\param[in] line ������
\retval 0 ����
\retval 1 ���s
*/
int addLine(void* _p, const char* line){
  tConfParam* p = (tConfParam*)_p;
  char* tmp;         //!< �p�����[�^��1�s���ƂɊi�[����̈�B
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

  //! �R�����g&���[����
  strcpy(tmp, line);
  ln = tmp;

  if((a = strchr(ln, '#')) != NULL)
  {
    len = a - ln;            //!< #���O�܂ł̕�����
    strncpy(tmp2, ln, len);
    tmp2[len] = '\0';
    strcpy(tmp, tmp2);
    ln = tmp;
  }
  if((a = strchr(ln, '\n')) != NULL)
  {
    len = a - ln;            //!< \n���O�܂ł̕�����
    strncpy(tmp2, ln, len);
    tmp2[len] = '\0';
    strcpy(tmp, tmp2);
    ln = tmp;
  }
  if((a = strchr(ln, '\r')) != NULL)
  {
    len = a - ln;            //!< \r���O�܂ł̕�����
    strncpy(tmp2, ln, len);
    tmp2[len] = '\0';
    strcpy(tmp, tmp2);
    ln = tmp;
  }
  if((a = strchr(ln, '\a')) != NULL)
  {
    len = a - ln;            //!< \a���O�܂ł̕�����
    strncpy(tmp2, ln, len);
    tmp2[len] = '\0';
    strcpy(tmp, tmp2);
    ln = tmp;
  }
  //! �s���̋󕶎��폜
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
  
  //! �p�����[�^���ƃp�����[�^�l����؂�󎚂���������
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
    //! �������s
    delete[] tmp;
    delete[] tmp2;
    return 1;
  }

  //! �̈�m��
  key = new char[len + 1];
  val = new char[strlen(ln + len) + 1];

  //! �p�����[�^���ƃp�����[�^�l�擾
  strncpy(key, ln, len);
  key[len] = '\0';
  strcpy(val, ln + len);

  //! val�̐擪�󕶎��폜
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

  //! �p�����[�^�[�̒ǉ�
  //! ����key���o�^����Ă����ꍇ�㏑������
  int pos = findPos(_p, key);
  if(pos < 0)
  {
    //! �V�K
    pushKeyVal(_p, key, val);
  }
  else
  {
    //! �㏑��
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
  //! �i�[�̈悪�I�[�o�[�����ꍇ�͑傫�ڂ̊g���̈���m�ۂ��A�ړ�����
  if(num >= p->confNumMax - 1)
  {
    if(p->confNum == 0)
    {
      //! �ŏ��ɌĂяo���ꂽ�Ƃ��́A�K���ɑ傫�߂Ɋm�ۂ��Ă��B
      p->confNumMax = 256;
    }
    else
    {
      //! ���̌��2�{���m�ۂ���B
      p->confNumMax *= 2;
    }

    //! �̈�m��
    tmp = new char*[p->confNumMax * 2];
    memset(tmp, NULL, sizeof(*tmp) * p->confNumMax * 2);

    //! ���f�[�^�̕�����ʒu�̃R�s�[
    memcpy(tmp, p->conf, sizeof(*tmp) * 2 * p->confNum);
    //for(int i = 0; i < p->confNum * 2; i++)
    //{
    //  tmp[i] = p->conf[i];
    //}
    
    //! �c��͏�����
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

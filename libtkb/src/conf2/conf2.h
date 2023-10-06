/*!
\file conf.h
\auther Y.Koide
\date 2006.08.16
\brief �p�����[�^�[��ǂݍ��ނ��߂̃��C�u����

*�g����*
����������confInit()���Ăяo���A
�g�p���confEnd()�ɂďI������B
�p�����[�^�[��ǂݍ��ނɂ́A
�E�t�@�C�������confAddFile()
�E�����񂩂��confAddStr()
�p�����[�^�[���擾����ɂ́A
�EconfGetStr()�ɂăp�����[�^���w�肵�ĕ�������擾����B
�EconfSetKey()�Ńp�����[�^�[����ݒ蒼��A
�@confGetVal()�ɂĕ�������擾����B
�EconfGetAllKeyVal()�ɂđS�p�����[�^���X�g���擾����B
�@�L�[�ƒl�f�[�^��\t�ɂăZ�p���[�g����Ă���B
confPrint()�ŉ�ʂɃp�����[�^�[�ꗗ��\���ł���B

*�p�����[�^�̏���*
�E#�͂��̌�̍s���܂ŃR�����g�Ƃ��Ė��������B
�E��s�P�̃p�����[�^�[���ݒ�ł��A
�@KEYWORD[�󕶎�(' 'or'\t')]VALUE[\n\r\aEOF�̂ǂꂩ(�ǂꂩ2����)]
�@�̏����ŋL�����Ă��Ȃ���΂Ȃ�Ȃ��B
�E��s�̍ő啶������1024�����ł���A�������ꍇ�A�������������O�������̂���s�Ƃ��Ĉ���
�EKEYWORD�͋󕶎���_�u���R�[�e�[�V����(")���܂�ł͂Ȃ�Ȃ��B
�EVALUE�͋󕶎����܂�ł͂Ȃ�Ȃ��B
�@�������AVALUE��"�Ŏn�����ꍇ�AVALUE���łQ�Ԗڂ�"�܂ł̊Ԃ̒l��VALUE�Ɋi�[���A
�@""�̊Ԃɂ���󕶎���VALUE�Ɋ܂ނ��Ƃ��ł���B
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

2008.03.18 Y.Koide
�EM1�̐�� ���̂��߂�libtkb�̋Z�p�����B�������̂��߂̋Z�p�����ł͂Ȃ��B
�܂��A���쌠��������Ă͂��Ȃ��B
�E�{���C�u�����̃��C�Z���X�`�Ԃ�GNU LESSER GENERAL PUBLIC LICENSE(LGPL)�Ƃ��A
���҂�����libtkb���C�u������p�����v���O�������\�[�X�R�[�h�̌��J�����ŏ��n�z�z���邱�Ƃ��ւ���B
�������Alibtkb�̓��I�ȊO�����C�u�����Ƃ��Ă̎g�p�Ɍ����L�̌���ł͂Ȃ��B
�ڍׂ�LICENSE.TXT���Q�ƁB
�EMakefile��p����FFC�p��gcc�p(���얢�m�F)���C�u�����̃R���p�C�����s����B
�Elibtkb��libtkb.sln��VC++�p���C�u�����̃R���p�C�����s����

2008.03.01 Y.Koide
�Ers232c, rs232c2��Init�֐��̈���parityBit�̒�`���C���B
���܂�0:none 1:evenParity 2:oddParity�������̂�0:none 1:oddParity 2:evenParity�ɒ������B

2007.10.25 Y.Koide
�Elibtkb01���č\�z
conf,conf2,netcl,netsv,netu,rs232c,tm,u���܂ށB

2007.10.21 Y.Koide
�Econf�ɂ�Val��"�̒�`��ύX�B
Val��Key�Ƌ󕶎�������Őڂ��Ă���AVal���̂ɋ󕶎����܂ނ��Ƃ��\�Ƃ���B
����āA���܂ł�Val�ɋ󕶎����܂ޏꍇ��""�ň͂��Ă����̂�p�~����B
���l��Val�̒���"���܂�ł��Ă�""�ƕϊ����ċL���K�v�͂Ȃ��B

2007.10.18 Y.Koide
�E���C�u�����֘A�p�̃��|�W�g��libtkb01���쐬�B
�E./src/{conf,netcl,netsv,netu,u,rs232c,tm}���쐬
�EMakefile���쐬
% make
��./export/�ɃX�^�e�B�b�N���C�u���������������B
�E�S���C�u�������W�񂵂�./export/libtkb.a�𐶐�����悤�ɂ����B
�C���N���[�h�t�@@�C���͊e���C�u�����̂��g�p���A���C�u�����̂�libtkb.a���g�p����
����
�E�g����
% FCC -I/usr/FFC/include -I./myinclude/ libtkb.a xxx.cpp

�ȏ�

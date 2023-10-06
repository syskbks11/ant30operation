#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>

#include <profhead.h>
#include <starlib.h>

/*
Profile process library

update history
1992/12/23 T.Handa
1992/12/28 T.Handa
*/

/*-----Frequency and Velocity calcuration----*/

/* definitions for iteration in STAR_FreqOnBECh(profile,freq) */ 
#define	POINTS	5	/* interation start points, minimum 2 */
#define	LOOP_LIMIT	100	/* interation loop limit */


double STAR_BindedCh(profile,ch)
/*
PROFILE�\���̂���A�������`�����l��ch�ł̃f�[�^�`�����l���̈ʒu�����߂�B
���ۂ̃f�[�^������_�̒[��
�P����profile.be_status.data_ch�܂�
������̃f�[�^������_��
�P����profile.be_status.be_ch�܂łƂ���B
�`�����l���ʒu�����ɑΉ�����`�����l���ʒu��Ԃ�

1992/12/27 ���c���O
*/
PROFILE	*profile;
double ch;
{
	double	ch_data;

/* ���݂̃f�[�^�̓I�[�o���b�v�Ȃ��ŕ�����f�[�^��[���畽�ω����Ă���Ɖ��� */
	ch_data=(ch-(double)(*profile).be_status.ch_orig+0.5)
		/(double)(*profile).be_status.ch_width+0.5;

	return(ch_data);
}

double STAR_BEDeltaFreqOnBE(profile,ch)
/*
PROFILE�\���̂���A������`�����l��ch�ł̕��������g���̔��W�������߂�B
���ۂ̃f�[�^������_�̒[��
�P����profile.be_status.be_ch�܂łƂ���B
�`�����l���ԍ����������قǎ��g�����Ⴂ�Ƃ��Ă���B

1992/12/27 ���c���O
1993/6/16 ���쏮�� �d�l�ύX��debug
*/
PROFILE *profile;
double ch;
{
	double	delta_freq,	/* ������ł̎��g��(MHz) */
		ch_pos;

/*	ch_pos=STAR_OriginalCh(profile,ch)-(*profile).be_status.basis_ch;*/
	ch_pos=ch-(*profile).be_status.basis_ch;

	delta_freq=(((*profile).be_status.disp4*ch_pos*4.0
		+(*profile).be_status.disp3)*ch_pos*3.0
		+(*profile).be_status.disp2)*ch_pos*2.0
		+(*profile).be_status.disp1;

	return(delta_freq);
}

double STAR_FreqOnBECh(profile,freq)
/*
PROFILE�\���̂ƁA���������g������f�[�^�`�����l���̈ʒu�����߂�B
���ۂ̃f�[�^������_�̒[��
�P����profile.be_status.data_ch�܂�
������̃f�[�^������_��
�P����profile.be_status.be_ch�܂łƂ���B
�`�����l���ʒu�����ɑΉ�����`�����l���ʒu��Ԃ�

return=-(*profile).be_status.data_ch) if out of BackEnd range underflow
return=((*profile).be_status.data_ch)*2 if out of BackEnd range overflow
return=-9998.0 if cannot converge in the limit

1992/12/28 ���c���O
1993/6/16 ���쏮�� debug
*/
PROFILE *profile;
double freq;
{
	double	freq_limit, freq_resid, freq_iter[2], ch_iter[2];
	double	ch_points[POINTS],freq_points[POINTS];
	double	neighbor_ch[2],neighbor_freq[2];
	double	freq_dev;

	double	ch_pos0,ch_pos1;

	int	ii,counter;

/* iteration limit = half of the resolution */
	freq_limit=(*profile).be_status.freq_res/2.0;

/* calculate standard points */ 
	for(ii=0;ii<POINTS;ii++) {
		ch_points[ii]
		=(double)((*profile).be_status.be_ch/(POINTS-1)*ii+1);
	}
	ch_points[0]=(-1.0); ch_points[POINTS-1]=ch_points[POINTS-1]+1.0;
	if(ch_points[POINTS-1]<(double)((*profile).be_status.be_ch+1))
		ch_points[POINTS-1]=(double)((*profile).be_status.be_ch+1);

	for(ii=0;ii<POINTS;ii++) {
		freq_points[ii]=STAR_ChFreqOnBE(profile,ch_points[ii]);
	}

/* out of the BE coverage */
	if(freq>freq_points[POINTS-1]) {
		return((double)(*profile).be_status.data_ch*2.0);
	}
	else if(freq<freq_points[0]) {
		return((double)(-(*profile).be_status.data_ch));
	}

/* find the nearest two standard points */
	for(ii=0;ii<POINTS;ii++) {
		if(freq_points[ii]<=freq) {
			neighbor_ch[0]=ch_points[ii];
			neighbor_freq[0]=freq_points[ii];
		}
	}
	for(ii=POINTS-1;ii>=0;ii--) {
		if(freq_points[ii]>=freq) {
			neighbor_ch[1]=ch_points[ii];
			neighbor_freq[1]=freq_points[ii];
		}
	}

/* hit the standard point */
	if(neighbor_freq[0]==neighbor_freq[1]) {
		ch_iter[0]=STAR_BindedCh(profile,ch_iter[0]);
/*		return(neighbor_ch[0]);*/
		return(ch_iter[0]);
	}

/* linear evaluation */
	ch_iter[0]=(freq-neighbor_freq[0])/(neighbor_freq[1]-neighbor_freq[0])
		*(neighbor_ch[1]-neighbor_ch[0])+neighbor_ch[0];
/*	freq_iter[0]=STAR_ChFreqOnBE(profile,ch_iter[0]);*/
	ch_pos0=ch_iter[0]-(*profile).be_status.basis_ch;
	freq_iter[0]=((((*profile).be_status.disp4*ch_pos0
			+(*profile).be_status.disp3)*ch_pos0
			+(*profile).be_status.disp2)*ch_pos0
			+(*profile).be_status.disp1)*ch_pos0
			+(*profile).be_status.disp0;

/* iteration by Newton-Lapson method */
	for(ii=0;ii<LOOP_LIMIT;ii++) {
		freq_dev=STAR_BEDeltaFreqOnBE(profile,ch_iter[0]);
		ch_iter[1]=ch_iter[0]-(freq_iter[0]-freq)/freq_dev;
/*		freq_iter[1]=STAR_ChFreqOnBE(profile,ch_iter[1]);*/
		ch_pos1=ch_iter[1]-(*profile).be_status.basis_ch;
		freq_iter[1]=((((*profile).be_status.disp4*ch_pos1
			+(*profile).be_status.disp3)*ch_pos1
			+(*profile).be_status.disp2)*ch_pos1
			+(*profile).be_status.disp1)*ch_pos1
			+(*profile).be_status.disp0;


		freq_resid=freq_iter[1]-freq;
		if(fabs(freq_resid)<=freq_limit) break;

		ch_iter[0]=ch_iter[1];
		freq_iter[0]=freq_iter[1];
	}

/* check conversion */
	if(fabs(freq_resid)>freq_limit) return(-9998.0);

	ch_iter[1]=STAR_BindedCh(profile,ch_iter[1]);
	return(ch_iter[1]);	
}

double STAR_FrequencyCh(profile,freq)
/*
PROFILE�\���̂���A�ϑ����g���ł̃f�[�^�`�����l�������߂�B
���ۂ̃f�[�^������_�̒[��
�P����profile.be_status.data_ch�܂łƂ���B

1992/12/27 ���c���O
*/
PROFILE *profile;
double freq;	/* ��M�d�g�ł̎��g��(GHz) */
{
	double	be_freq,	/* ������ł̎��g��(MHz) */
		center_freq,	/* ������ł̖��ڒ��S���g��(MHz) */
		offset_freq;	/* ������ł̖��ڒ��S�����offset���g��(MHz) */
	double	data_ch;	/* �f�[�^�ł̃`�����l�� */

	center_freq=(*profile).be_status.center_freq;
	offset_freq=(freq-(*profile).rx_status.rest_freq)*1.0e3;

	if((*profile).be_status.sign>=0)
		be_freq=center_freq+offset_freq;
	else
		be_freq=center_freq-offset_freq;

	data_ch=STAR_FreqOnBECh(profile,be_freq);

	return(data_ch);
}


double STAR_VelocityCh(profile,v_lsr)
/*
PROFILE�\���̂���ALSR�ɑ΂���ϑ��������x�ł̃f�[�^�`�����l�������߂�B
���ۂ̃f�[�^������_�̒[��
�P����profile.be_status.data_ch�܂łƂ���B

1992/12/28 ���c���O
*/
PROFILE *profile;
double v_lsr;	/* ���x(km/s) */
{
	double	offset_vel,	/* �ǔ����x�����offset���x(km/s) */
		offset_freq,	/* �ǔ����g�������offset���g��(GHz) */
		rf_freq,	/* RF���g��(GHz) */
		data_ch;	/* �f�[�^�ł̃`�����l�� */

	offset_vel=v_lsr-(*profile).rx_status.v_lsr;
	offset_freq=(-offset_vel)
		*((*profile).rx_status.rest_freq/LIGHT_SPEED);

	rf_freq=offset_freq+(*profile).rx_status.rest_freq;

	data_ch=STAR_FrequencyCh(profile,rf_freq);

	return(data_ch);
}


double STAR_OriginalCh(profile,ch)
/*
PROFILE�\���̂���A�f�[�^�`�����l��ch�ł̕������`�����l���ʒu�����߂�B
���ۂ̃f�[�^������_�̒[��
�P����profile.be_status.data_ch�܂�
������̃f�[�^������_��
�P����profile.be_status.be_ch�܂łƂ���B
�`�����l���ʒu�����ɑΉ�����`�����l���ʒu��Ԃ�

1992/12/25 ���c���O
*/
PROFILE	*profile;
double ch;
{
	double	ch_be;

/* ���݂̃f�[�^�̓I�[�o���b�v�Ȃ��ŕ�����f�[�^��[���畽�ω����Ă���Ɖ��� */
	ch_be	=(ch-0.5)*(double)(*profile).be_status.ch_width
		+(double)(*profile).be_status.ch_orig-0.5;

	return(ch_be);
}

double STAR_ChFreqOnBE(profile,ch)
/*
PROFILE�\���̂���A�f�[�^�`�����l��ch�ł̕��������g�������߂�B
���ۂ̃f�[�^������_�̒[��
�P����profile.be_status.data_ch�܂łƂ���B
�`�����l���ԍ����������قǎ��g�����Ⴂ�Ƃ��Ă���B

1992/12/25 ���c���O
*/
PROFILE *profile;
double ch;
{
	double	be_freq,	/* ������ł̎��g��(MHz) */
		ch_pos;

	ch_pos=STAR_OriginalCh(profile,ch)-(*profile).be_status.basis_ch;

	be_freq	=((((*profile).be_status.disp4*ch_pos
		   +(*profile).be_status.disp3)*ch_pos
		   +(*profile).be_status.disp2)*ch_pos
		   +(*profile).be_status.disp1)*ch_pos
		   +(*profile).be_status.disp0;

	return(be_freq);
}

double STAR_ChFrequency(profile,ch)
/*
PROFILE�\���̂���A�f�[�^�`�����l��ch�ł̊ϑ����g�������߂�B
���ۂ̃f�[�^������_�̒[��
�P����profile.be_status.data_ch�܂łƂ���B

1992/12/27 ���c���O
*/
PROFILE *profile;
double ch;
{
	double	be_freq,	/* ������ł̎��g��(MHz) */
		center_freq,	/* ������ł̖��ڒ��S���g��(MHz) */
		offset_freq,	/* ������ł̖��ڒ��S�����offset���g��(MHz) */
		rf_freq;	/* ��M�d�g�ł̎��g��(GHz) */

	be_freq=STAR_ChFreqOnBE(profile,ch);
	center_freq=(*profile).be_status.center_freq;

	if((*profile).be_status.sign>=0)
		offset_freq=be_freq-center_freq;
	else
		offset_freq=center_freq-be_freq;		

	rf_freq=offset_freq/1.0e3+(*profile).rx_status.rest_freq;

	return(rf_freq);
}
	
double STAR_ChVelocity(profile,ch)
/*
PROFILE�\���̂���A�f�[�^�`�����l��ch�ł�LSR�ɑ΂���ϑ��������x�����߂�B
���ۂ̃f�[�^������_�̒[��
�P����profile.be_status.data_ch�܂łƂ���B

1992/12/28 ���c���O
*/
PROFILE *profile;
double ch;
{
	double	rf_freq,	/* RF���g��(GHz) */
		offset_freq,	/* �ǔ����g�������offset���g��(GHz) */
		offset_vel,	/* �ǔ����x�����offset���x(km/s) */
		velocity;	/* ���x(km/s) */


	rf_freq=STAR_ChFrequency(profile,ch);
	offset_freq=rf_freq-(*profile).rx_status.rest_freq;

	offset_vel=(-offset_freq)
		/(*profile).rx_status.rest_freq*LIGHT_SPEED;

	velocity=offset_vel+(*profile).rx_status.v_lsr;

	return(velocity);
}

/*-----BitMap processing----*/


void STAR_RWriteBaselineFlags(profile,ch_range,flag)
/*
�^����ꂽ�`���l���͈͂ɑΉ�����x�[�X���C���r�b�g�}�b�v�t���O������
flag�Ŏw�肳�ꂽ�t���O�𗧂Ă�B
1993/1/9 �ؑ��C
*/
PROFILE		*profile;
RANGE_long	ch_range;	/* �t���O�͈� */ 
int		flag;
{
	int data_ch;

	data_ch = (*profile).be_status.data_ch;
	STAR_RWriteBitMapFlags(&((*profile).baseline[0]),ch_range,data_ch,flag);
}	

void STAR_WriteBaselineFlag(profile,ch,flag)
/*
�^����ꂽ�`���l���ɑΉ�����x�[�X���C���r�b�g�}�b�v�t���O������
flag�Ŏw�肳�ꂽ�t���O�𗧂Ă�B
1993/1/9 �ؑ��C
*/
PROFILE		*profile;
int		ch;	/* �t���O���������ރ`�����l���ԍ� */ 
int		flag;
{
	int flagbit;
	int data_ch;

	data_ch = (*profile).be_status.data_ch;

	if(ch<1)		ch = 1;
	else if(ch>data_ch)	ch = data_ch;

	STAR_WriteBitMapFlag(&((*profile).baseline[0]),ch,data_ch,flag);

}	

int STAR_ReadBaselineFlag(profile,ch)
/*
�^����ꂽ�`���l���ɑΉ�����x�[�X���C���r�b�g�}�b�v�t���O������ǂ݁A
�t���O�������Ă���ΐ���(int)�P�𗧂��Ă��Ȃ���΂O��Ԃ��B
1993/1/9 �ؑ��C
*/
PROFILE	*profile;
int ch;
{
	int flagbit;

	flagbit = STAR_ReadBitMapFlag(ch,&((*profile).baseline[0]));
	return(flagbit);
}	

void STAR_RWriteSpuriousFlags(profile,ch_range,flag)
/*
�^����ꂽ�`���l���͈͂ɑΉ�����X�v���A�X�r�b�g�}�b�v�t���O������
flag�Ŏw�肳�ꂽ�t���O�𗧂Ă�B
1993/1/9 �ؑ��C
*/
PROFILE		*profile;
RANGE_long	ch_range;	/* �t���O�͈� */ 
int		flag;
{
	int data_ch;

	data_ch = (*profile).be_status.data_ch;
	STAR_RWriteBitMapFlags(&((*profile).spurious[0]),ch_range,data_ch,flag);
}	

void STAR_WriteSpuriousFlag(profile,ch,flag)
/*
�^����ꂽ�`���l���ɑΉ�����X�v���A�X�r�b�g�}�b�v�t���O������
flag�Ŏw�肳�ꂽ�t���O�𗧂Ă�B
1993/1/9 �ؑ��C
*/
PROFILE		*profile;
int		ch;	/* �t���O���������ރ`�����l���ԍ� */ 
int		flag;
{
	int flagbit;
	int data_ch;

	data_ch = (*profile).be_status.data_ch;

	if(ch<1)		ch = 1;
	else if(ch>data_ch)	ch = data_ch;

	STAR_WriteBitMapFlag(&((*profile).spurious[0]),ch,data_ch,flag);

}	


int STAR_ReadSpuriousFlag(profile,ch)
/*
�^����ꂽ�`���l���ɑΉ�����X�v���A�X�r�b�g�}�b�v�t���O������ǂ݁A
�t���O�������Ă���ΐ���(int)�P�𗧂��Ă��Ȃ���΂O��Ԃ��B
1993/1/9 �ؑ��C
*/
PROFILE	*profile;
int ch;
{
	int flagbit;

	flagbit = STAR_ReadBitMapFlag(ch,&((*profile).spurious[0]));
	return(flagbit);
}	


int STAR_ReadBitMapFlag (ch, bitmap_flag)
/*
�^����ꂽ�`���l���ɑΉ�����X�v���A�X����уx�[�X���C���r�b�g�}�b�v
�t���O������ǂ݁A�t���O�������Ă���ΐ���(int)�P�𗧂��Ă��Ȃ���΂O��Ԃ��B
1992 Dec. 18 Written by Neko
*/
int ch;				/* �����f�[�^�̃`���l�� */
unsigned char bitmap_flag[];	/* �f�[�^�̃r�b�g�}�b�v�t���O���� */

{
	int byte_number, bit_number;	/* �t���O�f�[�^���i�[����Ă���ꏊ */
	int flag = 0;
	unsigned char mask = 1;

/* �t���O�̊i�[����Ă���ʒu�̌v�Z */

	byte_number = (ch - 1) / (sizeof(unsigned char)* 8) ;
	bit_number = ch - byte_number*sizeof(unsigned char)* 8 - 1;

/* �t���O��ON/OFF�̃`�F�b�N */
	
	if ((bitmap_flag[byte_number] & mask << bit_number) != 0)
	{
		flag = 1;
	}
/* �t���OON�Ȃ�P���AOFF�Ȃ�0��Ԃ� */

	return (flag);
}

void STAR_WriteBitMapFlag(bitmap_flag, ch, data_ch, flag)
/*
�t���O�𗧂Ă�`���l�����󂯎��A�Ή�����r�b�g�}�b�v������flag�Ŏw�肳�ꂽ
�t���O�𗧂Ă�B
1992 Dec. 18 Written by Neko
1993/1/8 T.Handa
*/
unsigned char bitmap_flag[];	/* �f�[�^�̃r�b�g�}�b�v�t���O���� */
int ch;		/* �t���O���Ă�ׂ��`���l�� */
int data_ch;	/* �����f�[�^�`���l���̑��� */
int flag;	/* ���Ă�ׂ��t���O�l 0�܂��͂���ȊO */
{
	int byte_number, bit_number;
	unsigned char	flag_byte;
	unsigned char	mask=0x01;

/* ch���͈͂��͂ݏo�Ă����ꍇ�̍Đݒ� */
	if(ch<1) ch=1; if(ch>data_ch) ch=data_ch;

/* Byte�ʒu�ւ̊��Z */
	byte_number = (ch - 1) / (sizeof(unsigned char)* 8) ;
	bit_number = ch - byte_number*sizeof(unsigned char)* 8 - 1;

/* flag�̐ݒ� */
	if(flag==0){
		flag_byte=~bitmap_flag[byte_number];
		flag_byte=flag_byte | mask << bit_number;
		bitmap_flag[byte_number]=~flag_byte;
	}
	else{
		flag_byte=bitmap_flag[byte_number];
		flag_byte=flag_byte | mask << bit_number;
		bitmap_flag[byte_number]=flag_byte;
	}
}

void STAR_RWriteBitMapFlags(bitmap_flag, ch_range, data_ch, flag)
/*
�t���O�𗧂Ă�`���l���͈�(ch_range.min��ch_range.max)���󂯎��A�Ή�����r�b�g�}�b�v������flag�Ŏw�肳�ꂽ�t���O�𗧂Ă�B
1992 Dec. 18 Written by Neko
1993/1/8 T.Handa
*/
unsigned char bitmap_flag[];	/* �f�[�^�̃r�b�g�}�b�v�t���O���� */
RANGE_long ch_range;		/* �t���O�͈́i�召�֌W�s��j */
int data_ch;			/* �����f�[�^�`���l���̑��� */
int flag;			/* ���Ă�ׂ��t���O�l 0�܂��͂���ȊO */
{
	int flag_ch;
	int byte_number;

	unsigned char	flag_byte;

	flag_ch = (data_ch - 1)/(sizeof(unsigned char)*8) +1;
	
/* �t���O�͈�(ch_range.min��ch_range.max)�̑召�֌W�𒲂ׁA�\�[�g����B */
	STAR_SortRangeLong(&ch_range);

/* ch_range.min��ch_range.max�͈̔͂ɂ���`���l���̃r�b�g�Ƀt���O�𗧂Ă�B*/

	{
		int ch = 1;
		int fswitch;
			/* fswitch = 0 : �t���O�͈͂ɂ܂������Ă��Ȃ� */
			/* fswitch = 1 : �t���O�͈͓� */
			/* fswitch = 2 : �t���O�͈͂���o�� */

		fswitch = 0;

		for (byte_number = 0; byte_number < flag_ch; ++byte_number)
		{
			int bit_number;
			unsigned char mask = 1;

			for (bit_number = 0; bit_number < 8; ++bit_number)
			{
				if (fswitch == 0)
				{
					if (ch >= ch_range.min)
					{
						fswitch = 1;
			if(flag==0){
				flag_byte=~bitmap_flag[byte_number];
				flag_byte=flag_byte | mask << bit_number;
				bitmap_flag[byte_number]=~flag_byte;
			}
			else{
						bitmap_flag[byte_number] = bitmap_flag[byte_number] | mask << bit_number;
			}
					}
				}
				else
				{
					if (ch <= ch_range.max)
					{
			if(flag==0){
				flag_byte=~bitmap_flag[byte_number];
				flag_byte=flag_byte | mask << bit_number;
				bitmap_flag[byte_number]=~flag_byte;
			}
			else{
						bitmap_flag[byte_number] = bitmap_flag[byte_number] | mask << bit_number;
			}
					}
					else
					{
						fswitch = 2;
					}
				}
/*			printf ("ch = %d fswitch = %d\n", ch, fswitch); */
			++ch;
			}
		}
	}
}

/* -------PROFILE header parameters processing -------*/

double STAR_CalcRms(profile, ch_range)
/*
rms���x�����v�Z����

1992/12/22 Naomi Hirano
1993/1/8 Toshihiro Handa (ver.1.01) profile.rx_status.rms�ւ̎�����������
1993/1/19 Toshihiro Handa (ver.1.02) �֐��E�}�N�������̕ύX�A
	�`�����l�����Ȃ��ꍇrms=0.0�Ƃ���B
*/
PROFILE *profile;	/* �v���t�@�C���f�[�^ */
RANGE_long ch_range;	/* rms�v�Z���s�Ȃ��`���l���͈� */

{
	int ch_number = 0;
	float sum = 0;
	double sum2 = 0;
	float average;
	double average2;
	double rms;
	float work_data[MAX_CH];
	int ch;
	int BS_flag, SP_flag;
	int range_min, range_max;

/*	int STAR_ReadBitMapFlag();	 �r�b�g�}�b�v�t���O�ǂݎ��֐� */
/*	void STAR_SortRangeLong();	 �͈͂�����RANGE�\���̂̓��e�̃\�[�g */

	int i;

/* Checking the value of ch_range.min and ch_range.max */

	if (ch_range.min < 0 || ch_range.max < 0)
	{
		ch_range.min = 0;
		ch_range.max = 0;
	}

/* Calculating the "sum" by using ch_range.min and ch_range.max */
/* (Only for the case ch_range.min and ch_range.max are defined) */

	if (ch_range.min != 0 || ch_range.max != 0)
	{
		STAR_SortRangeLong(&ch_range);

		if (ch_range.max > (*profile).be_status.data_ch)
		{
			ch_range.max = (*profile).be_status.data_ch;
		}

		for (ch = ch_range.min; ch <= ch_range.max; ++ch)
		{
			SP_flag = STAR_ReadBitMapFlag (ch, (*profile).spurious);
			
			if (SP_flag == 0)
			{
				work_data[ch_number] = (*profile).data[ch-1];
				sum = sum + work_data[ch_number];
				++ch_number;
			}
		}
	}
	else

/* Calculating the "sum" by using the baseline bitmap flag */

	{
		for(ch = 1; ch <= (*profile).be_status.data_ch; ++ch)
		{
/*			BS_flag = STAR_ReadBitMapFlag (ch, (*profile).baseline);
			SP_flag = STAR_ReadBitMapFlag (ch, (*profile).spurious);*/
			BS_flag=STAR_ReadBaselineFlag(profile,ch);
			SP_flag=STAR_ReadSpuriousFlag(profile,ch);

			if (BS_flag == FLAGGED && SP_flag == UNFLAGGED)
			{
				work_data[ch_number] = (*profile).data[ch-1];
				sum = sum + work_data[ch_number];
				++ch_number;
			}
		}

/* If the baseline is not yet substracted */

		if (ch_number == 0)
		{
			range_min = (*profile).be_status.data_ch / 8;
			range_max = (*profile).be_status.data_ch / 4;


			for (ch = range_min; ch <= range_max; ++ch)
			{
/*
				SP_flag = STAR_ReadBitMapFlag (ch, (*profile).spurious);
*/
				SP_flag=STAR_ReadSpuriousFlag(profile,ch);
				if (SP_flag == 0)
				{
					work_data[ch_number] = (*profile).data[ch-1];
					sum = sum + work_data[ch_number];
					++ch_number;
				}	
			}
			if (ch_number == 0)
			{
				ch_number = 1;
/*				sum = 9999; */
				sum = 0.0;
			}
		}
	}


	if(ch_number==0){
		(*profile).rx_status.rms=0.0;
		return(0.0);
	}


/* Calculating the average */

	average = sum / ch_number;


/* Calculating the sum of residial**2 */

	for (i = 0; i < ch_number; ++i)
	{
		sum2 = sum2 + (work_data[i] - average)*(work_data[i] - average);
	}

/* Calculating rms */

	average2 = sum2/ch_number;

	rms =sqrt(average2);

/* input profile header */
	(*profile).rx_status.rms=(float)rms;

	return (rms);
}

int	STAR_AddHistory(profile,history)
/*
HISTORY��profile�ɒǉ�����BEND�����̊֐��ŕt�������Ă悢�B
END�̎����t�����s�Ȃ��B
0   ����I��
100 �q�X�g���B�[�X�e�b�v����������

1992/12/12 T.Handa
*/
PROFILE	*profile;	/* �ǉ������profile�f�[�^ */
HISTORY	*history;	/* �ǉ�����history�\���� */
{
	int	lines;
	long	pres_time;
	int	current_step;
	int	add_size,add_byte;

	current_step=(*profile).sys_head.hist_step;

/* ----correct increasing the sizes---- */
/* previous last step is "END" --> adjust sizes */
	if(strcmp((*profile).history[current_step].step,"END")==0 ||
	   strcmp((*profile).history[current_step].step,"end")==0)
	{
		(*profile).sys_head.hist_line--;
		(*profile).sys_head.l_file-=16;
		(*profile).sys_head.l_head-=16;
	}

	add_size=(*history).size+1;
	add_byte=add_size*16;

	(*profile).sys_head.hist_line+=add_size;
	(*profile).sys_head.l_file+=add_byte;
	(*profile).sys_head.l_head+=add_byte;

/* increase history step in normal history addition */
	if(strcmp((*history).step,"END")!=0
	&& strcmp((*history).step,"end")!=0)
	{
		(*profile).sys_head.hist_step++;
	}
	else
	{
		(*history).size=0;
	}

/* too many history steps */
	if((*profile).sys_head.hist_step>=MAX_HISTORY_STEP)
		return(100);

/* copy history to profile.history */
	(*profile).history[current_step].size=(*history).size;
	(*history).step[7]=NULL;
	strcpy((*profile).history[current_step].step,(*history).step);
	time(&pres_time);
	(*profile).history[current_step].time=pres_time;
	for(lines=0;lines<(*history).size;lines++)
	{
		(*history).prm[lines][16]=NULL;
		strcpy((*profile).history[current_step].prm[lines],
			(*history).prm[lines]);
	}

/* add history is alread "END" */
	if(strcmp((*history).step,"END")==0
	|| strcmp((*history).step,"end")==0)
		return(0);

/* ---add "END" step--- */

	(*profile).sys_head.hist_line++;
	(*profile).sys_head.l_file+=16;
	(*profile).sys_head.l_head+=16;
	current_step=(*profile).sys_head.hist_step;
/* too many history steps */
	if((*profile).sys_head.hist_step>=MAX_HISTORY_STEP)
		return(100);

	(*profile).history[current_step].size=0;
	strcpy((*profile).history[current_step].step,"END");	
	(*profile).history[current_step].time=pres_time;

	return(0);
}

/* -------PROFILE data processing -------*/

void	STAR_CopyProfile(out_prf,in_prf)
/*
PROFILE�^�ϐ���copy����
1992/12/11 T.Handa
*/
PROFILE	*out_prf,	/* �R�s�[��̃v���t�@�C�� */
	*in_prf;	/* �R�s�[���̃v���t�@�C�� */
{
	unsigned int	ii;
	int		err_code; 	/* �G���[�R�[�h */

	unsigned short	history_steps;

/* copy STAR pipeflow header */
	strcpy((*out_prf).file_id,(*in_prf).file_id);
	strcpy((*out_prf).file_name,(*in_prf).file_name);

/* copy STAR file system header */
	STAR_CopySyshead(&((*out_prf).sys_head),&((*in_prf).sys_head));
	history_steps=(unsigned short)(*out_prf).sys_head.hist_step;

/* copy scan file header */
	STAR_CopyObsprm(&((*out_prf).obs_prm),&((*in_prf).obs_prm));
	STAR_CopyObject(&((*out_prf).object),&((*in_prf).object));
	STAR_CopyAntstatus(&((*out_prf).ant_status),&((*in_prf).ant_status));
	STAR_CopyRxstatus(&((*out_prf).rx_status),&((*in_prf).rx_status));
	STAR_CopyBestatus(&((*out_prf).be_status),&((*in_prf).be_status));
	STAR_CopyObslog(&((*out_prf).obs_log),&((*in_prf).obs_log));
/* copy�q�X�g���B�[ */
	for(ii=0;ii<=history_steps;ii++)
	{
	STAR_CopyHistory(&((*out_prf).history[ii]),&((*in_prf).history[ii]));
	}
/* copy�X�v���A�X�r�b�g�}�b�v */
	STAR_CopySpurious((*out_prf).be_status.data_ch,
		(*out_prf).spurious,(*in_prf).spurious);
/* copy�x�[�X���C���r�b�g�}�b�v */
	STAR_CopyBaseline((*out_prf).be_status.data_ch,
		(*out_prf).baseline,(*in_prf).baseline);
/* copy�v���t�@�C���f�[�^ */
	STAR_CopyProfdata((*out_prf).be_status.data_ch,
		(*out_prf).data,(*in_prf).data);
}


/* --- Part for STAR_CopyProfile --- */
void	STAR_CopySyshead(out_sys_head,in_sys_head)
SYS_HEAD	*out_sys_head,*in_sys_head;
{
	(*out_sys_head).l_file=(*in_sys_head).l_file;
	(*out_sys_head).l_head=(*in_sys_head).l_head;
	(*out_sys_head).format_id=(*in_sys_head).format_id;
	(*out_sys_head).version=(*in_sys_head).version;
	(*out_sys_head).data_type=(*in_sys_head).data_type;
	(*out_sys_head).hist_step=(*in_sys_head).hist_step;
	(*out_sys_head).hist_line=(*in_sys_head).hist_line;
	(*out_sys_head).scan=(*in_sys_head).scan;
	(*out_sys_head).backup=(*in_sys_head).backup;
	(*out_sys_head).num_scan=(*in_sys_head).num_scan;
}
void	STAR_CopyObject(out_object,in_object)
OBJECT	*out_object,*in_object;
{
	strcpy((*out_object).name,(*in_object).name);
	(*out_object).cood_sys=(*in_object).cood_sys;
	(*out_object).on_lb.x=(*in_object).on_lb.x;
	(*out_object).on_lb.y=(*in_object).on_lb.y;
	(*out_object).on_radec.x=(*in_object).on_radec.x;
	(*out_object).on_radec.y=(*in_object).on_radec.y;
	(*out_object).on_azel.x=(*in_object).on_azel.x;
	(*out_object).on_azel.y=(*in_object).on_azel.y;
	(*out_object).off_lb.x=(*in_object).off_lb.x;
	(*out_object).off_lb.y=(*in_object).off_lb.y;
	(*out_object).off_radec.x=(*in_object).off_radec.x;
	(*out_object).off_radec.y=(*in_object).off_radec.y;
	(*out_object).orig_xy.x=(*in_object).orig_xy.x;
	(*out_object).orig_xy.y=(*in_object).orig_xy.y;
	(*out_object).pa_xy=(*in_object).pa_xy;
	(*out_object).on_xy.x=(*in_object).on_xy.x;
	(*out_object).on_xy.y=(*in_object).on_xy.y;
}
void	STAR_CopyObsprm(out_obs_prm,in_obs_prm)
OBS_PRM	*out_obs_prm,*in_obs_prm;
{
	strcpy((*out_obs_prm).project,(*in_obs_prm).project);
	strcpy((*out_obs_prm).scd_file,(*in_obs_prm).scd_file);
	(*out_obs_prm).line_numb=(*in_obs_prm).line_numb;
	(*out_obs_prm).sw_mode=(*in_obs_prm).sw_mode;
	(*out_obs_prm).mjd=(*in_obs_prm).mjd;
	(*out_obs_prm).start=(*in_obs_prm).start;
	(*out_obs_prm).stop=(*in_obs_prm).stop;
	(*out_obs_prm).on_time=(*in_obs_prm).on_time;
	(*out_obs_prm).off_time=(*in_obs_prm).off_time;
	strcpy((*out_obs_prm).observer1,(*in_obs_prm).observer1);
	strcpy((*out_obs_prm).observer2,(*in_obs_prm).observer2);
	strcpy((*out_obs_prm).observer3,(*in_obs_prm).observer3);
	strcpy((*out_obs_prm).observer4,(*in_obs_prm).observer4);
}
void	STAR_CopyAntstatus(out_ant_status,in_ant_status)
ANT_STATUS	*out_ant_status,*in_ant_status;
{
	strcpy((*out_ant_status).tele,(*in_ant_status).tele);
	(*out_ant_status).ap_eff=(*in_ant_status).ap_eff;
	(*out_ant_status).mb_eff=(*in_ant_status).mb_eff;
	(*out_ant_status).fss_eff=(*in_ant_status).fss_eff;
	(*out_ant_status).hpbw=(*in_ant_status).hpbw;
}
void	STAR_CopyRxstatus(out_rx_status,in_rx_status)
RX_STATUS	*out_rx_status,*in_rx_status;
{
	strcpy((*out_rx_status).name,(*in_rx_status).name);
	(*out_rx_status).sb_mode=(*in_rx_status).sb_mode;
	(*out_rx_status).v_lsr=(*in_rx_status).v_lsr;
	(*out_rx_status).rest_freq=(*in_rx_status).rest_freq;
	(*out_rx_status).obs_freq=(*in_rx_status).obs_freq;
	(*out_rx_status).lo_freq1=(*in_rx_status).lo_freq1;
	(*out_rx_status).if_freq1=(*in_rx_status).if_freq1;
	(*out_rx_status).lo_freq2=(*in_rx_status).lo_freq2;
	(*out_rx_status).if_freq2=(*in_rx_status).if_freq2;
	(*out_rx_status).lo_freq3=(*in_rx_status).lo_freq3;
	(*out_rx_status).if_freq3=(*in_rx_status).if_freq3;
	(*out_rx_status).mltplx1=(*in_rx_status).mltplx1;
	(*out_rx_status).mltplx2=(*in_rx_status).mltplx2;
	(*out_rx_status).mltplx3=(*in_rx_status).mltplx3;
	(*out_rx_status).tsys=(*in_rx_status).tsys;
	(*out_rx_status).trx=(*in_rx_status).trx;
	(*out_rx_status).rms=(*in_rx_status).rms;
	(*out_rx_status).calib=(*in_rx_status).calib;
	(*out_rx_status).weight=(*in_rx_status).weight;
	(*out_rx_status).scale=(*in_rx_status).scale;

}
void	STAR_CopyBestatus(out_be_status,in_be_status)
BE_STATUS	*out_be_status,*in_be_status;
{
	strcpy((*out_be_status).name,(*in_be_status).name);
	(*out_be_status).be_ch=(*in_be_status).be_ch;
	(*out_be_status).basis_ch=(*in_be_status).basis_ch;
	(*out_be_status).sign=(*in_be_status).sign;
	(*out_be_status).data_ch=(*in_be_status).data_ch;
	(*out_be_status).center_freq=(*in_be_status).center_freq;
	(*out_be_status).disp0=(*in_be_status).disp0;
	(*out_be_status).disp1=(*in_be_status).disp1;
	(*out_be_status).disp2=(*in_be_status).disp2;
	(*out_be_status).disp3=(*in_be_status).disp3;
	(*out_be_status).disp4=(*in_be_status).disp4;
	(*out_be_status).freq_res=(*in_be_status).freq_res;
	(*out_be_status).ch_width=(*in_be_status).ch_width;
	(*out_be_status).ch_orig=(*in_be_status).ch_orig;
}
void	STAR_CopyObslog(out_obs_log,in_obs_log)
OBS_LOG	*out_obs_log,*in_obs_log;
{
	strcpy((*out_obs_log).site_name,(*in_obs_log).site_name);
	(*out_obs_log).meteo.tmp=(*in_obs_log).meteo.tmp;
	(*out_obs_log).meteo.press=(*in_obs_log).meteo.press;
	(*out_obs_log).meteo.water=(*in_obs_log).meteo.water;
	(*out_obs_log).tau0=(*in_obs_log).tau0;
	strcpy((*out_obs_log).memo,(*in_obs_log).memo);
	strcpy((*out_obs_log).comment,(*in_obs_log).comment);
}
void	STAR_CopyHistory(out_history,in_history)
HISTORY		*out_history,*in_history;
{
	unsigned int ii;

	(*out_history).size=(*in_history).size;
	strcpy((*out_history).step,(*in_history).step);
	(*out_history).time=(*in_history).time;
	for(ii=0;ii<(*in_history).size;ii++)
	{
		strcpy((*out_history).prm[ii],(*in_history).prm[ii]);
	}
}
void	STAR_CopySpurious(data_ch,out_spurious,in_spurious)
unsigned int	data_ch;
unsigned char	*out_spurious,*in_spurious;
{
	unsigned int sp_ch;
	unsigned int	ii;

	sp_ch=(data_ch-1)/(sizeof(unsigned char)*8)+1;
	for(ii=0;ii<sp_ch;ii++)
		out_spurious[ii]=in_spurious[ii];
}
void	STAR_CopyBaseline(data_ch,out_baseline,in_baseline)
unsigned int	data_ch;
unsigned char	*out_baseline,*in_baseline;
{
	unsigned int sp_ch;
	unsigned int	ii;

	sp_ch=(data_ch-1)/(sizeof(unsigned char)*8)+1;
	for(ii=0;ii<sp_ch;ii++)
		out_baseline[ii]=in_baseline[ii];
}
void	STAR_CopyProfdata(data_ch,out_data,in_data)
unsigned int	data_ch;
float	*out_data,*in_data;
{
	unsigned int	ii;

	for(ii=0;ii<data_ch;ii++)
		out_data[ii]=in_data[ii];
}

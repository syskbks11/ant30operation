/* struct definitions
for COST60cm telescope driver

T.Handa 1991/12/18
T.Handa 1992/08/26 Reduce for STAR system
*/

typedef struct {
		double l; 	/* �w�������̕����]�� */
		double m; 	/* �x�������̕����]�� */
		double n; 	/* �y�������̕����]�� */
	} DIRECT_COS;

typedef struct {
		double d_psi; 		/* ���o�͓̏� ����(rad) */
		double d_epsilon; 	/* �����X�p�͓̏� ����(rad) */
		double epsilon; 	/* ���ω����X�p ��(rad) */
	} NUTATION_PRM;

typedef struct {
		double x; 	/* �ϑ��n�_�o�x(degree):���o���� */
		double y; 	/* �ϑ��n�_�ܓx(degree):�k�܂��� */
		double z; 	/* �ϑ��n�_�C��(m) */
	} SITE_POS;

typedef struct {
		double tmp; 	/* ��C���x(degree C) */
		double water; 	/* �����C����(mbar) */
		double press; 	/* �n�� 1.5m �ɂ������C��(mbar) */
	} METEO_PRM;

typedef struct {
		double rest; 	/* �Î~���g��(GHz) */
		double ifreq; 	/* ��P���Ԏ��g��(GHz) */
		double v_lsr; 	/* �V�̂̂k�r�q���x(km/s) */
		double v_trk; 	/* �ǔ����x(km/s) */
		double sw;		/* �X�C�b�`���O���x(Hz) */
		double offset;	/* ���g���X�C�b�`���O�I�t�Z�b�g���g��(GHz) */
		short  sb_mode; /* �T�C�h�o���h���[�h 0x0004:LSB-SSB,0x0005:LSB-DSB,0x0006:USB-SSB,0x0007:USB-DSB */
	} FREQ;

typedef struct {
		double d; 		/* 1899/12/31 12:00(ET)���猻�݂܂ł̌o�ߗ�\���i���j */
		double t; 		/* 1899/12/31 12:00(ET)���猻�݂܂ł̌o�ߗ�\���i�����E�X���I�j */
		double t50; 	/* B1950.0����̌o�ߎ��i�x�b�Z�����I�j */
	} EP_TIME;

typedef struct {
		double x_rad; 	/* �V�̌o�x(rad) */
		double y_rad; 	/* �V�̈ܓx(rad) */
	} XY_POS_RAD;


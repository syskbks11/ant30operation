typedef struct	{	
		double x; 	/* V�ox(degree) */
		double y; 	/* V��x(degree) */
	} XY_POS;	/* two-dimensional position on the sky */

typedef struct {
		double l; 	/* w������]� */
		double m; 	/* x������]� */
		double n; 	/* y������]� */
	} DIRECT_COS;

typedef struct {
		double d_psi; 		/* �o�ͮ �(rad) */
		double d_epsilon; 	/* ��Xp�ͮ �(rad) */
		double epsilon; 	/* �ϩ�Xp �(rad) */
	} NUTATION_PRM;

typedef struct {
		double x; 	/* Ϫn_ox(degree):o�� */
		double y; 	/* Ϫn_�x(degree):kܪ� */
		double z; 	/* Ϫn_C�(m) */
	} SITE_POS;

typedef struct {
		double tmp; 	/* �C�x(degree C) */
		double water; 	/* �C��(mbar) */
		double press; 	/* n� 1.5m ɨ���C�(mbar) */
	} METEO_PRM;

typedef struct {
		double rest; 	/* �~�g(GHz) */
		double ifreq; 	/* �P��g(GHz) */
		double v_lsr; 	/* V��krq�x(km/s) */
		double v_trk; 	/* ���x(km/s) */
		double sw;		/* XCb`O�x(Hz) */
		double offset;	/* �gXCb`OItZbg�g(GHz) */
		short  sb_mode; /* TChoh[h 0x0004:LSB-SSB,0x0005:LSB-DSB,0x0006:USB-SSB,0x0007:USB-DSB */
	} FREQ;

typedef struct {
		double d; 		/* 1899/12/31 12:00(ET)������o��\i�j */
		double t; 		/* 1899/12/31 12:00(ET)������o��\iEX�Ij */
		double t50; 	/* B1950.0���o�ixbZ�Ij */
	} EP_TIME;

typedef struct {
		double x_rad; 	/* V�ox(rad) */
		double y_rad; 	/* V��x(rad) */
	} XY_POS_RAD;




#define	CHAR_LENGTH	80
#define	MAX_CHAR_LENGTH	81920
#define	PI	3.1415927



extern void 	lb_radec();
extern void 	dir_cos();
extern void 	direction();
extern double 	stod();



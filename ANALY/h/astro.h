typedef struct	{	
		double x; 	/* VÌox(degree) */
		double y; 	/* VÌÜx(degree) */
	} XY_POS;	/* two-dimensional position on the sky */

typedef struct {
		double l; 	/* w²ûüÌûü]· */
		double m; 	/* x²ûüÌûü]· */
		double n; 	/* y²ûüÌûü]· */
	} DIRECT_COS;

typedef struct {
		double d_psi; 		/* ©oÌÍ® Õ(rad) */
		double d_epsilon; 	/* ©¹XpÌÍ® Ã(rad) */
		double epsilon; 	/* ½Ï©¹Xp Ã(rad) */
	} NUTATION_PRM;

typedef struct {
		double x; 	/* Ïªn_ox(degree):oª³ */
		double y; 	/* Ïªn_Üx(degree):kÜª³ */
		double z; 	/* Ïªn_C²(m) */
	} SITE_POS;

typedef struct {
		double tmp; 	/* åC·x(degree C) */
		double water; 	/* öCª³(mbar) */
		double press; 	/* nã 1.5m É¨¯éåC³(mbar) */
	} METEO_PRM;

typedef struct {
		double rest; 	/* Ã~üg(GHz) */
		double ifreq; 	/* æPÔüg(GHz) */
		double v_lsr; 	/* VÌÌkrq¬x(km/s) */
		double v_trk; 	/* Çö¬x(km/s) */
		double sw;		/* XCb`O¬x(Hz) */
		double offset;	/* ügXCb`OItZbgüg(GHz) */
		short  sb_mode; /* TChoh[h 0x0004:LSB-SSB,0x0005:LSB-DSB,0x0006:USB-SSB,0x0007:USB-DSB */
	} FREQ;

typedef struct {
		double d; 		/* 1899/12/31 12:00(ET)©ç»İÜÅÌoßï\iúj */
		double t; 		/* 1899/12/31 12:00(ET)©ç»İÜÅÌoßï\iEX¢Ij */
		double t50; 	/* B1950.0©çÌoßixbZ¢Ij */
	} EP_TIME;

typedef struct {
		double x_rad; 	/* VÌox(rad) */
		double y_rad; 	/* VÌÜx(rad) */
	} XY_POS_RAD;




#define	CHAR_LENGTH	80
#define	MAX_CHAR_LENGTH	81920
#define	PI	3.1415927



extern void 	lb_radec();
extern void 	dir_cos();
extern void 	direction();
extern double 	stod();



/*
Software Tools for radio Astronomical data Reduction

standard library header file
1992/4/28 T.Handa
1992/8/25 T.Handa
1992/9/9  T.Handa
1992/11/25 T.Handa
1992/11/27 T.Handa
1992/12/11 T.Handa
1992/12/21 T.Handa
1992/12/23 T.Handa
1992/12/28 T.Handa
1993/1/8 T.Handa
1993/1/12 T.Handa
1994/8/30 T.OKA
*/

typedef struct	{	double	max;
			double	min;
		} RANGE;
typedef struct	{	double	dx;
			double	dy;
		} XY_SIZE;
/*typedef struct	{	double	x;
			double	y;
		} XY_POS;*/	/* defined in profhead.h */
typedef struct	{	long	max;
			long	min;
		} RANGE_long;
typedef struct	{	long	dx;
			long	dy;
		} XY_SIZE_long;
typedef struct	{	long	x;
			long	y;
		} XY_POS_long;
typedef struct	{	RANGE	x;
			RANGE	y;
		} BOX_RANGE;
typedef struct	{	RANGE_long	x;
			RANGE_long	y;
		} BOX_RANGE_long;


#define	LIGHT_SPEED	2.99792458e5	/* light speed in km/s */


#define	FLAGGED		1	/* for bitmap flag */
#define	UNFLAGGED	0

#define	BAD_SCAN	1	/* for .sdb good/bad flag */
#define	GOOD_SCAN	0

#define	MAX_FILE_NAME_LENGTH	128	/* see also in profhead.h */
#define MAX_PRM_RECORD_LENGTH	81
#define MAX_IDX_RECORD_LENGTH	81


/*-------file names, parameter processing-------*/
#ifdef _NO_PROTO
extern int	STAR_StdFileExtName();
extern int	STAR_StdBakFileName();
extern void	STAR_DividePrmRecord();
#else /* undef _NO_PROTO */
extern int	STAR_StdFileExtName(char *, char *, char *, char *);
extern int	STAR_StdBakFileName(char *, char *, int, char *);
extern void	STAR_DividePrmRecord(char *, char *, char *);
#endif /* endif of define _NO_PROTO */

/*-------PROFILE header processing-------*/

#ifdef _NO_PROTO
extern int	STAR_AddHistory();
extern void	STAR_RWriteBaselineFlags();
extern void	STAR_WriteBaselineFlag();
extern int	STAR_ReadBaselineFlag();
extern void	STAR_RWriteSpuriousFlags();
extern void	STAR_WriteSpuriousFlag();
extern int	STAR_ReadSpuriousFlag();

extern int	STAR_ReadBitMapFlag();
extern void	STAR_WriteBitMapFlag();
extern void	STAR_RWriteBitMapFlags();
extern double	STAR_CalcRms();

#else /* undef _NO_PROTO */
extern int	STAR_AddHistory(PROFILE *, HISTORY *);
extern void	STAR_RWriteBaselineFlags(PROFILE *, RANGE_long, int);
extern void	STAR_WriteBaselineFlag(PROFILE *, int, int);
extern int	STAR_ReadBaselineFlag(PROFILE *, int);
extern void	STAR_RWriteSpuriousFlags(PROFILE *, RANGE_long, int);
extern void	STAR_WriteSpuriousFlag(PROFILE *, int, int);
extern int	STAR_ReadSpuriousFlag(PROFILE *, int);

extern int	STAR_ReadBitMapFlag(int, unsigned char *);
extern void	STAR_WriteBitMapFlag(unsigned char *, int, int, int);
extern void	STAR_RWriteBitMapFlags(unsigned char *, RANGE_long, int, int);
extern double	STAR_CalcRms(PROFILE *, RANGE_long);
#endif /* endif of define _NO_PROTO */

/*-------frequency and velocity calcurations-------*/
#ifdef _NO_PROTO
extern double	STAR_BindedCh();
extern double	STAR_BEDeltaFreqOnBE();
extern double	STAR_FreqOnBECh();
extern double	STAR_FrequencyCh();
extern double	STAR_VelocityCh();

extern double	STAR_OriginalCh();	
extern double	STAR_ChFreqOnBE();
extern double	STAR_ChFrequency();
extern double	STAR_ChVelocity();

#else /* undef _NO_PROTO */
extern double	STAR_BindedCh(PROFILE *, double);
extern double	STAR_BEDeltaFreqOnBE(PROFILE *, double);
extern double	STAR_FreqOnBECh(PROFILE *, double);
extern double	STAR_FrequencyCh(PROFILE *, double);
extern double	STAR_VelocityCh(PROFILE *, double);

extern double	STAR_OriginalCh(PROFILE *, double);
extern double	STAR_ChFreqOnBE(PROFILE *, double);
extern double	STAR_ChFrequency(PROFILE *, double);
extern double	STAR_ChVelocity(PROFILE *, double);
#endif /* endif of define _NO_PROTO */

/*--------general data processing---------*/
#ifdef _NO_PROTO
extern double	ipow();
extern void	LeastSquareFit();
#else /* undef _NO_PROTO */
extern double	ipow(float, int);
extern void	LeastSquareFit(float *, float *, int, int, float *);
#endif /* endif of define _NO_PROTO */

/*--------data sorting---------*/
#ifdef _NO_PROTO
extern void	STAR_SortRange();
extern void	STAR_SortRangeLong();
extern void	STAR_SortBoxRange();
extern void	STAR_SortBoxRangeLong();
#else /* undef _NO_PROTO */
extern void	STAR_SortRange(RANGE *);
extern void	STAR_SortRangeLong(RANGE_long *);
extern void	STAR_SortBoxRange(BOX_RANGE *);
extern void	STAR_SortBoxRangeLong(BOX_RANGE_long *);
#endif /* endif of define _NO_PROTO */

/*--------PROFILE data processing--------*/
#ifdef _NO_PROTO
extern void	STAR_CopyProfile();

extern void	STAR_CopyAntstatus();
extern void	STAR_CopyBaseline();
extern void	STAR_CopyBestatus();
extern void	STAR_CopyHistory();
extern void	STAR_CopyObject();
extern void	STAR_CopyObslog();
extern void	STAR_CopyObsprm();
extern void	STAR_CopyProfdata();
extern void	STAR_CopyRxstatus();
extern void	STAR_CopySpurious();
extern void	STAR_CopySyshead();

#else /* undef _NO_PROTO */
extern void	STAR_CopyProfile(PROFILE *, PROFILE *);

extern void	STAR_CopyAntstatus(ANT_STATUS *, ANT_STATUS *);
extern void	STAR_CopyBaseline(unsigned int,
			unsigned char *, unsigned char *);
extern void	STAR_CopyBestatus(BE_STATUS *, BE_STATUS *);
extern void	STAR_CopyHistory(HISTORY *, HISTORY *);
extern void	STAR_CopyObject(OBJECT *, OBJECT *);
extern void	STAR_CopyObslog(OBS_LOG *, OBS_LOG *);
extern void	STAR_CopyObsprm(OBS_PRM *, OBS_PRM *);
extern void	STAR_CopyProfdata(unsigned int, float *, float *);
extern void	STAR_CopyRxstatus(RX_STATUS *, RX_STATUS *);
extern void	STAR_CopySpurious(unsigned int,
			unsigned char *, unsigned char *);
extern void	STAR_CopySyshead(SYS_HEAD *, SYS_HEAD *);
#endif /* endif of define _NO_PROTO */

/*----- INDEX processing -----*/
#ifdef _NO_PROTO
extern int	index_load();
extern int	index_save();
extern int	index_headin();
extern int	index_headout();
#else /* undef _NO_PROTO */
extern int	index_load(FILE	*, char *, unsigned short *);
extern int	index_save(FILE	*, char *filename, unsigned short);
extern int	index_headin(char *);
extern int	index_headout(char *);
#endif /* endif of define _NO_PROTO */

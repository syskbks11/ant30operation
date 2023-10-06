/*		fprintf(fp,"%d\t",pix[i]-pixd[i]);	*/	
/*	Struct difinitions for reading FITS format data header

1993/9/1	Tomoharu OKA	:

*/

/*#include	<limits.h>*/
#include	<float.h>




#define	PI	3.1415927
#define	RAD	0.01745329278	/* PI/180.0 */

#define	MAX_DIMENSION	8
#define	MAX_DATA_NUM	2048
#define	MAX_MATRIX_DIM	512
#define	MAX_CHAR_NUMBER	80
#define	BIT_RECORD	23040
#define	DEFAULT_BLANK_VALUE	(-FLT_MAX)

/*
#define	CHAR_BIT		 0x8
#define	SCHAR_MIN		-0x80
#define	SCHAR_MAX		 0x7F
#define	UCHAR_MAX		 0xFF
#define	CHAR_MIN		-0x80
#define	CHAR_MAX		 0x7F
#define	SHRT_MIN		-0x8000
nagai*/
#define	SHRT_MAX		 0x7FFF
/*nagai
#define	USHRT_MAX		 0xFFFF
#define	INT_MIN			-0x80000000
#define	INT_MAX			 0x7FFFFFFF
#define	UINT_MAX		 0xFFFFFFFF
#define	LONG_MIN		-0x80000000
nagai*/
#define	LONG_MAX		 0x7FFFFFFF
/*nagai
#define	ULONG_MAX		 0xFFFFFFFF
*/



typedef	struct	{	unsigned short	simple;
			short	bitpix;
			long	naxis[MAX_DIMENSION];	
			double		bscale;
			double		bzero;
			char		bunit[MAX_CHAR_NUMBER];
			double		blank;
			char		object[MAX_CHAR_NUMBER];
			char		date_obs[MAX_CHAR_NUMBER];
			char		date_map[MAX_CHAR_NUMBER];
			char		origin[MAX_CHAR_NUMBER];
			char		instrume[MAX_CHAR_NUMBER];
			char		telescop[MAX_CHAR_NUMBER];
			char		observer[MAX_CHAR_NUMBER];
			double		crval[MAX_DIMENSION];	
			double		crpix[MAX_DIMENSION];	
			double		cdelt[MAX_DIMENSION];
			double		cd[MAX_DIMENSION][MAX_DIMENSION];
			int		cd_mode;	/*0:CDELT, 1:CD*/	
			double		crota[MAX_DIMENSION];	
			char		*ctype[MAX_DIMENSION];	
			long		data_num;
			double		datamax;	
			double		datamin;	
			double		epoch;	
		} FITS_HEAD;	

typedef struct	{	FITS_HEAD	head;
			float		*data;
		} FITS;

typedef struct	{	unsigned short	x;
			unsigned short	y;
			unsigned short	v;
		} AXIS_INDEX;

typedef struct	{	unsigned short	dim;
			int		num[MAX_DIMENSION];
			float		axisx[MAX_MATRIX_DIM];
			float		axisy[MAX_MATRIX_DIM];
			float		data[MAX_MATRIX_DIM][MAX_MATRIX_DIM];
		} MATRIX;




#ifdef _NO_PROTO
extern FILE 	*open_Fits();
extern int 	read_Fits();
extern int 	output_Fits();
extern int 	read_FitsHeader();
extern int 	read_FitsData();
extern int 	divide_Line();
extern int 	compress();
extern int 	get_MatrixData();
extern int 	OutputColumnData();
extern int 	OutputMatrixData();
extern int 	OutputMatrixData2();
extern int	slice_Fits();
extern long 	DataPosition();
extern float 	Data();
extern char 	*space_pad();
extern long 	PIXtoPOS();
extern int 	POStoPIX();
extern int	tint();
extern int 	XtoPIX();
extern int 	PIXtoX();
extern int 	PIXtoMX();
extern int 	MXtoPIX();
extern int 	TXtoPIX();
extern int 	PIXtoTX();
extern int 	FindDec();
extern int	rotate_ra();
extern int	rotate_dec();
extern int	rotate_dec();
extern int	PIXtoPRJ();
extern int	PRJtoPIX();
#else
extern FILE 	*open_Fits(char *);
extern int 	read_Fits(FITS *, FILE *);
extern int 	output_Fits(FITS *, FILE *);
extern int read_FitsHeader(FITS_HEAD *, int *, AXIS_INDEX *, FILE *);
extern int divide_Line(char *, char *, char *, char *);
extern int compress(char *, char *);
extern int get_MatrixData(FITS_HEAD *, int, MATRIX *, AXIS_INDEX *, FILE *);
extern int OutputMatrixData(MATRIX *, FILE *);
extern int 	OutputMatrixData2(MATRIX *, FILE *);
extern int	slice_Fits(FITS *, int, int);
extern long 	DataPosition(FITS *, int *);
extern float 	Data();
extern char 	*space_pad();
extern long 	PIXtoPOS(FITS *, int *);
extern int 	POStoPIX(FITS *, long, int *);
extern int	tint(double);
extern int 	XtoPIX(FITS *, double *, int *);
extern int 	PIXtoX(FITS *, double *, int *);
extern int 	PIXtoWX(FITS *, double *, int *);
extern int 	WXtoPIX(FITS *, double *, int *);
extern int 	TXtoPIX(FITS *, double *, int *);
extern int 	PIXtoTX(FITS *, double *, int *);
extern int 	FindDec();
extern int	rotate_ra();
extern int	rotate_dec();
extern int	rotate_dec();
extern int	PIXtoPRJ();
extern int	PRJtoPIX();
#endif


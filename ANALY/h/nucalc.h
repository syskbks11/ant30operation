extern short	sweepout();
extern short	inverse();
extern short	Lagrange_interpolation();
extern short	differential_Lagrange();
extern short	spline();
extern short	differential_spline();
extern short	fft();
extern short	fft2();

#define		SWEEPOUT_MAX_DIM		256
#define		SPLINE_MAX_DATA_NUM		32
#define		SPLINE_ORDER			3
#define		FFT_MAX_DATA_NUM		512
#define		FFT_ITER			9
#define		EPS				1.0e-10
#define		PI	3.1415927
#define		MAX_ITERATION			1024
#define		GAUSSIAN_NUM			16


typedef struct {
	float	*x;
	float	*y;
	long	num;
} SPECT;



typedef struct {/*	y = a + b (x-xg) + c exp(-(x-xg)^2/s^2) 	*/
	double	ag[GAUSSIAN_NUM];
	double	bg[GAUSSIAN_NUM];
	double	cg[GAUSSIAN_NUM];
	double	sg[GAUSSIAN_NUM];
	double	xg[GAUSSIAN_NUM];
	int	num;
} GAUSS;

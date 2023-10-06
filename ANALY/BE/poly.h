/*
External Functions
*/

/*	Functions 	*/




/*	Constants	*/


#define		BLTM	1.38066e-16	/*	Boltzmann constant	*/
#define		PLNK	6.62618e-27	/*	Planck constant		*/
#define		PL_BL	4.7992844e-11	/*	(PLNK/BLTM)	*/

#define		C	2.99792e+10
#define		E	4.80324e-10
#define		Me	9.10953e-28
#define		Mp	1.67265e-24


#define		TBB	2.7		/*	Cosmic background temperature	*/
#define		PL_BL_TBB	1.777512741e-11		/*(PLNK/(BLTM*TBB))*/
#define		PC	3.08e18
#define		HtoAv	5.34e-22
#define		Evib	2.6
#define		YEAR	3.1557600e7	/*	1 year in seconds	*/

#define		mu_CO	0.11e-18	/*	CO electric dipole moment	*/
#define		B_CO	57.8975e9	/*	CO rotational constant		*/ 	
#define		mu_CH	1.46e-18	/*	CH electric dipole moment	*/
#define		B_CH	420.0e9		/*	CH rotational constant		*/ 	
#define		Nc_T_CH	6.6e9		/*	CH ncrit/T3^0.5			*/
#define		mu_OH	1.67e-18	/*	OH electric dipole moment	*/
#define		B_OH	565.8e9		/*	OH rotational constant		*/ 	
#define		Nc_T_OH	1.5e10		/*	OH ncrit/T3^0.5			*/
#define		mu_H2O	1.94e-18	/*	H2O electric dipole moment	*/
#define		B_H2O	350.0e9		/*	H2O rotational constant		*/ 	
#define		Nc_T_H2O	2.3e10	/*	H2O ncrit/T3^0.5			*/

#define		DZ	2.5e-4
#define		DAV	0.1
#define		MAX_DEPTH	15.0
#define		T_STEP_MIN	1.0e4
#define		T_STEP_MAX	1.0e7


#define		DELIM	"\t ,"
#define		MAX_CHEM_IT		200
#define		MAX_THERM_IT		100
#define		MAX_IT		1000
#define		MAX_INC		10000
#define		CONV	1.0e-8

#define		MAX_HEAT_PROC	128
#define		MAX_COOL_PROC	128
#define		DT	0.1

#define		MAX_CHAR_LENGTH	256
#define		CHAR_LENGTH	80

#define		MAX_POLYTROPE_INDEX	999999.9
#define		MAX_DATA_NUM	10000



/*	Temporary defined functions	*/

#define		POW2(x)		pow((double)(x),2.0)
#define		POW3(x)		pow((double)(x),3.0)
#define		POW4(x)		pow((double)(x),4.0)
#define		DBL		(double)

/*	Pararameter files	*/


/*	Definition of struct variable	*/

typedef struct {
	double	*xi;
	double	*theta;
	double	*rho;
	int	num;
} MODEL;

typedef struct {
	double	*r;
	double	*sigma;
	double	*n;
	int	num;
} DATA;

typedef struct {
	double	alpha;
	double	rhoc;
	double	ximax;
	double	n;
} PARAM;



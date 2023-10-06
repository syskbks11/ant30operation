#include	<stdio.h>
#include	<math.h>

#define	PI	(3.1415927)

#define	MAX_DATA_NUM	2048
#define	MAX_CHAR_LENGTH	256
#define	MAX_ITERATION	10000
#define	RAND_TRIAL_NUM	1000
#define	EPS	1.0e-10

int	main(argc,argv)
int	argc;
char	*argv[];
{
double	Br32, Br34, sigma_Br32, sigma_Br34;
double	dBr32, dBr34;
int	ii, jj, it;
double	rnd1, rnd2;

/*	rotational constants (measured)*/
Br32 = 6.0815e9;	sigma_Br32 = 1.60205e2;
Br34 = 5.9329e9;	sigma_Br34 = 1.60205e2*2.0;



for(ii=0;ii<RAND_TRIAL_NUM;ii++){

	rnd1 = rand()/(pow(2.0,31.0)-1.0);
	rnd2 = rand()/(pow(2.0,31.0)-1.0);
	dBr32 = sigma_Br32*pow((-2.0)*log(rnd1), 0.5)*cos(2.0*PI*rnd2);
	dBr34 = sigma_Br34*pow((-2.0)*log(rnd2), 0.5)*sin(2.0*PI*rnd1);

	fprintf(stdout,"%e\t%e\t%lf\t%lf\n",rnd1,rnd2,dBr32,dBr34);

}


}











/*
2001/07/30	Coding start
*/

#define	DEBUG	1

#include	<stdio.h>
#include	<string.h>
#include	<stdlib.h>
#include	<float.h>
#include	<math.h>

#define	BLTM	1.38054e-16
#define	PLNK	6.62559e-27
#define	LIGHT	2.99792458e10
#define	NU10	492.169e9
#define	NU21	809.0e9
#define	TBB	2.7

#define	MAX_IT	1000
#define	DIFF	0.01
#define	EPS	1.0e-10

#define	TEX_MIN	5.0
#define	TEX_MAX	50.0
#define	TEX_STEP	0.25

#define	TAU_MIN	3.0e-2
#define	TAU_MAX	3.0e2
#define	TAU_STEP	0.01
#define	TAU_LSTEP	0.01



double	JJ(nu, Tex)
double	nu, Tex;
{
double	y;
y = PLNK*nu/BLTM / (exp(PLNK*nu/(BLTM*Tex)) - 1.0);
return(y);
}

double	rho(Tex)
double	Tex;
{
double	y;
y = 1.28 * exp(-23.6/Tex) 
	* (1.0 - exp(-38.9/Tex))/(1.0 - exp(-23.6/Tex)) ; 
return(y);
}




double	F(Tex, tau, T)
double	Tex, tau, T;
{
double	y;
y = ( JJ(NU10,Tex) - JJ(NU10,TBB) )*( 1.0 - exp((-1.0)*tau) ) - T;
return(y);
}

double	G(Tex, tau, T)
double	Tex, tau, T;
{
double	y;
y = ( JJ(NU21,Tex) - JJ(NU21,TBB) )*( 1.0 - exp((-1.0)*tau*rho(Tex)) ) - T;
return(y);
}









int	main()
{
int	ii, jj;
double	T10, T21, I10, I21;
double	s10, s21;
double	Tex, tau, ltau, Nci;
double	kai2, cl;
char	buffer[80];


fprintf(stderr,"Tpeak(1-0) = ");
fgets(buffer,80,stdin);
T10=atof(buffer);
fprintf(stderr,"sigma(1-0) = ");
fgets(buffer,80,stdin);
s10=atof(buffer);
fprintf(stderr,"Tpeak(2-1) = ");
fgets(buffer,80,stdin);
T21=atof(buffer);
fprintf(stderr,"sigma(2-1) = ");
fgets(buffer,80,stdin);
s21=atof(buffer);

/*
fprintf(stderr,"Integrated Intensity(1-0) = ");
fgets(buffer,80,stdin);
I10=atof(buffer);
*/

Tex=TEX_MIN;
for(ii=1;;ii++){
tau=TAU_MIN;	ltau=log10(tau);
for(jj=1;;jj++){
	kai2 = pow(F(Tex, tau, T10)/s10,2.0) + pow(G(Tex, tau, T21)/s21,2.0);
	cl = exp((-1.0)*kai2/2.0);
	
	fprintf(stdout,"%lf\t%lf\t%lf\t%lf\n",Tex,ltau,kai2,cl);
	
	ltau=ltau+TAU_LSTEP;	
	tau=pow(10.0,ltau);
/*	tau=tau+TAU_STEP	*/
	if(tau>TAU_MAX)	break;	
}
	Tex=Tex+TEX_STEP;
	if(Tex>TEX_MAX)	break;	
}





/*
Nci = 1.98e15*( exp(23.6/Tex) + 3.0 + 5.0*exp(-38.9/Tex) )
	* I10 * tau/(1.0-exp((-1.0)*tau))
	/(1.0 - JJ(NU10,TBB)/JJ(NU10,Tex));
*/


}

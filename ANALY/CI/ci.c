/*
2001/07/03	Coding tau
2001/07/04	rho() Ver.1.00 
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
#define	INI_TEX	30.0
#define	INI_TAU	0.001

double	max(x1, x2)
double	x1, x2;
{
if(x1>=x2)	return(x1);
else		return(x2);

}

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
y = ( JJ(NU10,Tex) - JJ(NU10,TBB) )*( 1.0 - exp((-1.0)*tau)) - T;
return(y);
}

double	G(Tex, tau, T)
double	Tex, tau, T;
{
double	y;
y = ( JJ(NU21,Tex) - JJ(NU21,TBB) )*( 1.0 - exp((-1.0)*tau*rho(Tex)) ) - T;
return(y);
}



double	dFdTex(Tex, tau, T)
double	Tex, tau, T;
{
int	i;
double	y1, y2, dT;

dT = fabs(Tex)*DIFF;
y2=0.0;
for(i=0;i<MAX_IT;i++){
	y1 = (F(Tex+dT,tau,T) - F(Tex,tau,T))/dT;
	if(fabs((y1-y2)/F(Tex,tau,T))<EPS)	break;
	else{	dT=dT/2.0;	y2=y1;}
}	/* fot i loop	*/
/*fprintf(stderr,"i=%d\n",i);*/
return(y1);
}

double	dGdTex(Tex, tau, T)
double	Tex, tau, T;
{
int	i;
double	y1, y2, dT;

dT = fabs(Tex)*DIFF;
y2=0.0;
for(i=0;i<MAX_IT;i++){
	y1 = (G(Tex+dT,tau,T) - G(Tex,tau,T))/dT;
	if(fabs((y1-y2)/G(Tex,tau,T))<EPS)	break;
	else{	dT=dT/2.0;	y2=y1;}
}	/* fot i loop	*/
return(y1);
}


double	dFdtau(Tex, tau, T)
double	Tex, tau, T;
{
int	i;
double	y1, y2, dtau;

dtau = fabs(tau)*DIFF;
y2=0.0;
for(i=0;i<MAX_IT;i++){
	y1 = (F(Tex,tau+dtau,T) - F(Tex,tau,T))/dtau;
	if(fabs((y1-y2)/F(Tex,tau,T))<EPS)	break;
	else{	dtau=dtau/2.0;	y2=y1;}
}	/* for i loop	*/
return(y1);
}

double	dGdtau(Tex, tau, T)
double	Tex, tau, T;
{
int	i;
double	y1, y2, dtau;

dtau = fabs(tau)*DIFF;
y2=0.0;
for(i=0;i<MAX_IT;i++){
	y1 = (G(Tex,tau+dtau,T) - G(Tex,tau,T))/dtau;
	if(fabs((y1-y2)/G(Tex,tau,T))<EPS)	break;
	else{	dtau=dtau/2.0;	y2=y1;}
}	/* for i loop	*/
return(y1);
}








int	main()
{
int	i, j;
double	T10, T21, I10, I21;
double	Tex, tau, Nci;
double	dTex, dtau;
double	det;
static double	ini_Tex=INI_TEX, ini_tau=INI_TAU;
char	buffer[80];


fprintf(stderr,"Tpeak(1-0) = ");
fgets(buffer,80,stdin);
T10=atof(buffer);
fprintf(stderr,"Tpeak(2-1) = ");
fgets(buffer,80,stdin);
T21=atof(buffer);

fprintf(stderr,"Integrated Intensity(1-0) = ");
fgets(buffer,80,stdin);
I10=atof(buffer);
/*
fprintf(stderr,"Integrated Intensity(2-1) = ");
fgets(buffer,80,stdin);
I21=atof(buffer);

fprintf(stderr,"%lf\t%lf\t%lf\t%lf\n",T10,T21,I10,I21);
*/


Tex = ini_Tex;
tau = ini_tau;

#if DEBUG
fprintf(stderr,"%e\t%e\t%e\t%e\t%e\t%e\n"
	, F(Tex,tau),G(Tex,tau)
	, dFdTex(Tex,tau), dFdtau(Tex,tau)
	, dGdTex(Tex,tau), dGdtau(Tex,tau));
#endif


for(i=0;i<MAX_IT;i++){
	det = dFdTex(Tex,tau,T10)*dGdtau(Tex,tau,T21) - 
		dFdtau(Tex,tau,T10)*dGdTex(Tex,tau,T21) ; 
	dTex = ( dGdtau(Tex,tau,T21)*F(Tex,tau,T10) - dFdtau(Tex,tau,T10)*G(Tex,tau,T21))
		* (-1.0)/det ;  
	dtau = ( (-1.0)*dGdTex(Tex,tau,T21)*F(Tex,tau,T10) + dFdTex(Tex,tau,T10)*G(Tex,tau,T21))
		* (-1.0)/det ;  
		
#if DEBUG
/*
fprintf(stderr,"%d\t%e\t%e\t%e\t%e\n",i,dFdTex(Tex,tau,T10),dGdTex(Tex,tau,T21)
		,dFdtau(Tex,tau,T10),dGdtau(Tex,tau,T21));
*/
fprintf(stderr,"%d\t%e\t%e\n",i,Tex,tau);
#endif

/*	if(det==0.0){	ini_tau=ini_tau/2.0; 
			tau=ini_tau;
			continue;}
*/

	if(max(fabs(dTex/Tex),fabs(dtau/tau))<EPS)	break;
	else	{Tex=Tex+dTex; tau=tau+dtau;}
	
	if(Tex<0.0)	Tex=fabs(Tex);
	if(tau<0.0)	tau=fabs(tau);
}	/*	for i loop	*/


Nci = 1.98e15*( exp(23.6/Tex) + 3.0 + 5.0*exp(-38.9/Tex) )
	* I10 * tau/(1.0-exp((-1.0)*tau))
	/(1.0 - JJ(NU10,TBB)/JJ(NU10,Tex));

fprintf(stderr,"it=%d\tdet=%e\n\n",i,det);
/*fprintf(stderr,"Tex=%lf\ttau10=%lf\n",Tex,tau);
fprintf(stderr,"N(CI)=%e\n",Nci);*/
fprintf(stdout,"T(1-0)=%lf\tT(2-1)=%lf\n",T10,T21);
fprintf(stdout,"Tex=%lf\ttau10=%lf\n",Tex,tau);
fprintf(stdout,"N(CI)=%e\n",Nci);

}

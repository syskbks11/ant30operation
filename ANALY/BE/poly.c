/************************************************/
/*						*/
/*	Solving a Bonner-Ebert sphere		*/
/*		2002/9/2 by Tomo OKA		*/
/*						*/
/*	Usage: poly -n (index) [-d]		*/
/*						*/
/************************************************/

#include	<stdio.h>
#include	<math.h>

#include	"poly.h"

#define	MAX_POLYTROPE_INDEX	999999.9


extern double	f();
extern double	g();
extern double	theta_t();
extern void	options();



main(argc,argv)
int	argc;
char	*argv[];
{
int	ii, jj;
int	debug, inf;
double	r, p, V, N;
double	n;
double	alpha, lambda, kappa;
double	rho, xi, theta, Y;
double	dxi, dtheta, dY;

double	k11, k12, k13, k14;	/*	Y deviation (f)	*/
double	k21, k22, k23, k24;	/*	theta deviation (g)	*/

options(argc,argv,&n,&debug);

xi = 0.0;	dxi = 0.005;
if(n==MAX_POLYTROPE_INDEX)	theta = 0.0;
else				theta = 1.0;	
Y = 0.0;

if(((n==0.0)||(n==1.0)||(n==5.0))&&(debug==1)){
	fprintf(stdout,"%e\t%e\t%e\n",xi,theta,(theta-theta_t(xi,n))/theta_t(xi,n));	
}
else{
	if(n==MAX_POLYTROPE_INDEX)	fprintf(stdout,"%e\t%e\t%e\n",xi,theta,exp((-1.0)*theta));
	else				fprintf(stdout,"%e\t%e\t%e\n",xi,theta,pow(theta,n));
}

for(ii=0;ii<=MAX_INC;ii++){
	k11 = dxi*f(xi,theta,n);
	k21 = dxi*g(xi,Y,n);
	k12 = dxi*f(xi+dxi/2.0,theta+k21/2.0,n);
	k22 = dxi*g(xi+dxi/2.0,Y+k11/2.0,n);
	k13 = dxi*f(xi+dxi/2.0,theta+k22/2.0,n);
	k23 = dxi*g(xi+dxi/2.0,Y+k12/2.0,n);
	k14 = dxi*f(xi+dxi,theta+k23,n);
	k24 = dxi*g(xi+dxi,Y+k13,n);

	Y = Y + (k11+2.0*k12+2.0*k13+k14)/6.0;
	theta = theta + (k21+2.0*k22+2.0*k23+k24)/6.0;
	xi = xi+dxi;

	if(theta<0.0)	break;

	if(((n==0.0)||(n==1.0)||(n==5.0))&&(debug==1)){
		fprintf(stdout,"%e\t%e\t%e\n",xi,theta,(theta-theta_t(xi,n))/theta_t(xi,n));	
	}
	else{
		if(n==MAX_POLYTROPE_INDEX)	fprintf(stdout,"%e\t%e\t%e\n",xi,theta,exp((-1.0)*theta));
		else				fprintf(stdout,"%e\t%e\t%e\n",xi,theta,pow(theta,n));
	}

}

}


double	f(xi, theta, n)
double	xi, theta, n;
{
double	ff;

if(n==MAX_POLYTROPE_INDEX)
			ff = pow(xi,2.0)*exp((-1.0)*theta);
else if(n<(-1.0)){
	if(theta<=0.0)	ff = 0.0;
	else		ff = (1.0)*pow(xi,2.0)*pow(theta,n);
}
else{
	if(theta<=0.0)	ff = 0.0;
	else		ff = (-1.0)*pow(xi,2.0)*pow(theta,n);
}

return(ff);
}


double	g(xi, Y, n)
double	xi, Y, n;
{
double	gg;

if(xi==0.0)	gg = 0.0;
else		gg = pow(xi,(-2.0))*Y;

return(gg);
}


double	theta_t(xi,n)
double	xi, n;
{
double	tt;

if(xi==0.0)	tt=0.0;

if(n==0.0)		tt = 1.0-pow(xi,2.0)/6.0;
else if(n==1.0)		tt = sin(xi)/xi;
else if(n==5.0)		tt = 1.0/pow((1.0+pow(xi,2.0)/3.0),0.5);

return(tt);
}



void	options(argc,argv,n,d)
int	argc;
char	*argv[];
double	*n;
int	*d;
{
int	ii, jj;
char	*token;

/* default values */
*n=0.0;
*d=0;

/* interpret options */
for(ii=1;ii<argc;ii++){
	if(argv[ii][0]=='-'){
		if(strstr(argv[ii],"n")!=NULL){
			ii++;
			if((strstr(argv[ii],"inf")!=NULL)||(strstr(argv[ii],"INF")!=NULL)){
				*n=MAX_POLYTROPE_INDEX;
			}
			else	*n=atof(argv[ii]);
		}
		if(strstr(argv[ii],"d")!=NULL){
			*d=1;
		}
	}
}
}


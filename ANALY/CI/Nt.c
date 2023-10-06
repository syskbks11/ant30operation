#include	<stdio.h>
#include	<math.h>

#define         PLNK    6.62559e-27
#define         BLTM    1.38054e-16
#define TBB     2.73


double	J(T,nu)
double	T, nu;
{
double	J, c;
c=PLNK/BLTM*(nu*1.0e9);
J=c/(exp(c/T)-1.0);
return(J);
}


main()
{
double	Tex, Tb, TAU,tau, nu, W, N;
int	j;

/*fprintf(stderr,"nu[GHz]=");
fscanf(stdin,"%lf",&nu);*/
nu=492.160651;
/*fprintf(stderr,"Jlower=");
fscanf(stdin,"%d",&j);*/
fprintf(stderr,"Tex=");
fscanf(stdin,"%lf",&Tex);

/*fprintf(stderr,"Tb=");
fscanf(stdin,"%lf",&Tb);
tau = (-1.0)*log(1.0 - Tb/(J(Tex,nu)-J(TBB,nu)));
fprintf(stdout,"tau=%lf\n",tau);
*/
TAU=1.0;

fprintf(stderr,"W[K km/s]=");
fscanf(stdin,"%lf",&W);


N=1.98e15*(exp(23.6/Tex)+3.0+5.0*exp(-38.9/Tex))
	*W*TAU/(1.0-(J(TBB,nu)/J(Tex,nu)) );
fprintf(stdout,"N(CI) = %6.4e*%7.5lf*%e = %e\n",
	1.98e15*(exp(23.6/Tex)+3.0+5.0*exp(-38.9/Tex) )*(1.0-(J(TBB,nu)/J(Tex,nu)) ), 
	TAU, W, N);


}
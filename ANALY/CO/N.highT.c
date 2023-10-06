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
double	Tex, Tb, tau, nu, W, N, TAU, X;
int	j;

fprintf(stderr,"nu[GHz]=");
fscanf(stdin,"%lf",&nu);
fprintf(stderr,"Jlower=");
fscanf(stdin,"%d",&j);
fprintf(stderr,"Tex=");
fscanf(stdin,"%lf",&Tex);
fprintf(stderr,"tau=");
fscanf(stdin,"%lf",&tau);

fprintf(stderr,"W[K km/s]=");
fscanf(stdin,"%lf",&W);

X=5.557e17*Tex/pow(nu,2.0)*
	exp((double)(j+2)*PLNK*nu*1.0e9/(2.0*BLTM*Tex))
	/(1.0-(J(TBB,nu)/J(Tex,nu)));
if(tau==0.0)	TAU = 1.0;
else		TAU = tau/(1.0-exp(-tau));
N=X*TAU*W;
fprintf(stdout,"N(CO) = %e*%e*%e = %e\n",X,TAU,W,N);


}
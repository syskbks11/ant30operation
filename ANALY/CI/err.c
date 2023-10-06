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
double	Tex, Tb, tau, nu, W, N;
int	j;

W=1.0;
nu=492.160651;


for(j=0;j<=90;j++){
	Tex = 10.0 + (double)j; 
	N=1.9e15*(exp(23.6/Tex)+3.0+5.0*exp(-38.9/Tex))
		*W/(1.0-(J(TBB,nu)/J(Tex,nu)) );
	fprintf(stdout,"%lf\t%e\n",Tex,N);
}

}
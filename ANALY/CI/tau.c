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
double	Tex, Tb, tau, nu;

fprintf(stderr,"nu[GHz]=");
fscanf(stdin,"%lf",&nu);
fprintf(stderr,"Tex=");
fscanf(stdin,"%lf",&Tex);
fprintf(stderr,"Tb=");
fscanf(stdin,"%lf",&Tb);

tau = (-1.0)*log(1.0 - Tb/(J(Tex,nu)-J(TBB,nu)));

fprintf(stdout,"tau=%lf\n",tau);

}
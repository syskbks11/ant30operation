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
double	Tex, Tb, tau, nu1, nu2, W1, W2, Nco, Nci;
int	j;

/*fprintf(stderr,"nu[GHz]=");
fscanf(stdin,"%lf",&nu);*/
/*nu1 = 110.201353;*/
nu1 = 109.782182;
nu2 = 492.160651;


/*W1 = 50.435;*/
W1 = 12.698;
W2 = 12.562;


Tex =10.0;
while(Tex<=100.0){
	Nco = 5.557e17*Tex/pow(nu1,2.0)*
		exp((double)(2)*PLNK*nu1*1.0e9/(2.0*BLTM*Tex))
        	*W1/(1.0-(J(TBB,nu1)/J(Tex,nu1)));
	Nci=1.98e15*(exp(23.6/Tex)+3.0+5.0*exp(-38.9/Tex))
		*W2/(1.0-(J(TBB,nu2)/J(Tex,nu2)) );
	fprintf(stdout,"%lf\t%e\t%e\n",Tex,Nco,Nci);
	Tex = Tex + 1.0;
}

}




#include	<stdio.h>
#include	<math.h>
#include	<string.h>
#include	<stdlib.h>

#define	PLNK    (6.62559e-27)	/*cgs*/
#define	BLTM    (1.38054e-16)	/*cgs*/
#define	PI	3.1415927
#define TBB     2.73		/*K]*/
#define	mu	(0.112e-18)	/*[esu]*/
#define	B12co	(57.635602e9)	/*[Hz]*/
#define	B13co	(55.1006765e9)	/*[Hz]*/
#define	Bc18o	(54.891091e9)	/*[Hz]*/

#define	MAX_J	10000
#define	EPS	(1.0e-8)



double	J(T,nu)
double	T, nu;
{
double	J, Tc;
Tc=(PLNK/BLTM)*(nu);
J=Tc/(exp(Tc/T)-1.0);
return(J);
}



double	Z(T)
double	T;
{
int	ii;
double	z, dz, zd;

z=0.0;
for(ii=0;ii<MAX_J;ii++){
	dz=(double)(2*ii+1)*exp(-(PLNK*B12co*(double)(ii)*(double)(ii+1))/(BLTM*T));
/*fprintf(stderr,"J=%d:%e\n",ii,dz);*/
	z+=dz;
	if(fabs(dz/z)<EPS)	break;	
}
zd = (BLTM*T)/(PLNK*B12co);

return(z);
}


main()
{
double	Tex, F1, F2, F;
char	iso[16];
double	nu115, nu492;
int	j;


Tex = 20 ;
j=0;

nu115 = 2.0*B12co*(double)(j+1);
nu492 = 492.1606e9;

for(Tex=3.0;Tex<=100.0;Tex+=1.0){

    F1 = 1.98e10*(exp(23.6/Tex)+3.0+5.0*exp(-38.9/Tex) )
    	* (1.0-(J(TBB,nu492)/J(Tex,nu492)) ); 
    
    F2 = (3.0*BLTM)/(16.0*pow(PI,3.0)*pow(mu,2.0)*B12co)
    	* Z(Tex)/pow((double)(j+1),2.0) 
    	* exp((PLNK*B12co*(double)(j+1)*(double)(j+2))/(BLTM*Tex)) 
    	/ (1.0 - (J(TBB,nu115)/J(Tex,nu115)) ) ;

    fprintf(stdout,"%lf\t%e\t%e\t%lf\n",Tex,F1,F2,F1/F2);

}






}
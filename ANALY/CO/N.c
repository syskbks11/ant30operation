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

double	nu, B;
int	j;


double	J(T)
double	T;
{
double	J;
J = (PLNK*nu/BLTM)/(exp(PLNK*nu/(BLTM*T))-1.0)   ;
return(J);
}


double	Z(T)
double	T;
{
int	ii;
double	z, dz, zd;
z=0.0;
for(ii=0;ii<MAX_J;ii++){
	dz=(double)(2*ii+1)*exp(-(PLNK*B*(double)(ii)*(double)(ii+1))/(BLTM*T));
/*fprintf(stderr,"J=%d:%e\n",ii,dz);*/
	z = z+dz;
	if(fabs(dz/z)<EPS)	break;	
}
zd = (BLTM*T)/(PLNK*B);

fprintf(stderr,"Z = %e/%e\n",z,zd);
return(z);
}


main()
{
double	Tex, Tb, tau, W, N, TAU, X;
char	iso[16];

inp:;
fprintf(stderr,"Isotope (12CO, 13CO, C18O) :");
fscanf(stdin,"%s",iso);
if((strcmp(iso,"12CO")==0)||(strcmp(iso,"12co")==0))		B=B12co;
else if((strcmp(iso,"13CO")==0)||(strcmp(iso,"13co")==0))	B=B13co;
else if((strcmp(iso,"C18O")==0)||(strcmp(iso,"c18o")==0))	B=Bc18o;
else{	fprintf(stderr,"This program does not support %s.\n",iso); 
	goto inp;
}

fprintf(stderr,"Lower rotational level=");
fscanf(stdin,"%d",&j);
fprintf(stderr,"Tex=");
fscanf(stdin,"%lf",&Tex);
fprintf(stderr,"tau=");
fscanf(stdin,"%lf",&tau);
fprintf(stderr,"W[K km/s]=");
fscanf(stdin,"%lf",&W);

nu = 2.0*B*(double)(j+1);
W = W*1.0e5;

X = (3.0*BLTM)/(16.0*pow(PI,3.0)*pow(mu,2.0)*B)
	* Z(Tex)/pow((double)(j+1),2.0) 
	* exp((PLNK*B*(double)(j+1)*(double)(j+2))/(BLTM*Tex))
	/ (1.0 - (J(TBB)/J(Tex)) ) ;

if(tau==0.0)	TAU = 1.0;
else		TAU = tau/(1.0-exp(-tau));


N=X*TAU*W;
fprintf(stdout,"N(CO) = %e*%e*%e = %e\n",X,TAU,W,N);


}
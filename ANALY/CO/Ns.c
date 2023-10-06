#include	<stdio.h>
#include	<math.h>

#define	TEXFIX	1

#define         PLNK    6.62559e-27
#define         BLTM    1.38054e-16
#define TBB     2.73
#define	NU13	110.201353
#define	NU18	109.782182
#define	NUc1	492.160651
#define	NU32	345.795989
#define	_13CO_C18O	7.4	

#define	_13CO_INT	"/home3/tomo/DATA/ID4/45m/tex/13CO.slice"
#define	_13CO_PK	"/home3/tomo/DATA/ID4/45m/tex/13CO.PK.slice"
#define	_C18O_INT	"/home3/tomo/DATA/ID4/45m/tex/C18O.slice"
#define	_C18O_PK	"/home3/tomo/DATA/ID4/45m/tex/C18O.PK.slice"
#define	_C1_INT		"/home3/tomo/DATA/ID4/FST/tex/C1.slice"
#define	_C1_PK		"/home3/tomo/DATA/ID4/FST/tex/C1.PK.slice"

extern double	J();



main()
{
FILE	*fp13i, *fp18i, *fpc1i;
FILE	*fp13p, *fp18p, *fpc1p;
double	x, y, co13i, co18i, c1i;
double	co13p, co18p, c1p;
double	tex, Tb, tau13, tau18, tauc1; 
double	Nco13, Nco18, Nc1;
double	nu, W, N, TAU, X;
int	j;


fp13i=fopen(_13CO_INT,"r");
if(fp13i==NULL){fprintf(stderr,"Can't open %s!\n",_13CO_INT);	exit(101);}
fp13p=fopen(_13CO_PK,"r");
if(fp13p==NULL){fprintf(stderr,"Can't open %s!\n",_13CO_PK);	exit(101);}
fp18i=fopen(_C18O_INT,"r");
if(fp18i==NULL){fprintf(stderr,"Can't open %s!\n",_C18O_INT);	exit(103);}
fp18p=fopen(_C18O_PK,"r");
if(fp18p==NULL){fprintf(stderr,"Can't open %s!\n",_C18O_PK);	exit(104);}
fpc1i=fopen(_C1_INT,"r");
if(fpc1i==NULL){fprintf(stderr,"Can't open %s!\n",_C1_INT);	exit(103);}
fpc1p=fopen(_C1_PK,"r");
if(fpc1p==NULL){fprintf(stderr,"Can't open %s!\n",_C1_PK);	exit(104);}



while(fscanf(fp13i,"%lf %lf %lf",&x,&y,&co13i)==3){
	fscanf(fp13p,"%lf %lf %lf",&x,&y,&co13p);
	fscanf(fp18i,"%lf %lf %lf",&x,&y,&co18i);
	fscanf(fp18p,"%lf %lf %lf",&x,&y,&co18p);
	fscanf(fpc1i,"%lf %lf %lf",&x,&y,&c1i);
	fscanf(fpc1p,"%lf %lf %lf",&x,&y,&c1p);
	
	if(co18p<=0.0)	{tau13=0.0; tex=0.0;}
	else{
		Tau(co13p/co18p, _13CO_C18O, &tau13);
		Tex(NU13,co13p,tau13,&tex);
	}
	tau18=tau13/(_13CO_C18O);

	if(tex<15.0)	tex=15.0;

#if TEXFIX
tex=20.0;
#endif

	j=0;
	X=5.55e17*tex/pow(NU13,2.0)*exp((double)(j+2)*PLNK*NU13*1.0e9
		/(2.0*BLTM*tex))/(1.0-(J(TBB,NU13)/J(tex,NU13)));
	if(tau13<=0.0)	TAU = 1.0;
	else		TAU = tau13/(1.0-exp(-tau13));
	Nco13=X*TAU*co13i;

	X=5.55e17*tex/pow(NU18,2.0)*exp((double)(j+2)*PLNK*NU18*1.0e9
		/(2.0*BLTM*tex))/(1.0-(J(TBB,NU18)/J(tex,NU18)));
	if(tau18<=0.0)	TAU = 1.0;
	else		TAU = tau18/(1.0-exp(-tau18));
	Nco18=X*TAU*co18i;


	if((c1p<=0.0)||(tex==0.0))	
		tauc1=0.0;
	else	tauc1 = (-1.0)*log(1.0 - c1p/(J(tex,NUc1)-J(TBB,NUc1)));

#if TEXFIX
tauc1=0.0;
#endif

	if(tauc1==0.0)	TAU=1.0;
	else		TAU = tauc1/(1.0-exp(-tauc1));
	Nc1=1.98e15*(exp(23.6/tex)+3.0+5.0*exp(-38.9/tex))
        	*c1i*TAU/(1.0-(J(TBB,NUc1)/J(tex,NUc1)) );
	if(Nc1<0.0)	Nc1=0.0;


	fprintf(stdout,"%lf\t%lf\t%lf\t%e\t%e\t%e\n",x,y,tex,Nco13,Nco18,Nc1);


}








/*
nu = NU18;	j=0;
X=5.55e17*tex/pow(nu,2.0)*
	exp((double)(j+2)*PLNK*nu*1.0e9/(2.0*BLTM*tex))
	/(1.0-(J(TBB,nu)/J(Tex,nu)));
if(tau<=0.0)	TAU = 1.0;
else		TAU = tau18/(1.0-exp(-tau18));
N=X*TAU*W;
fprintf(stdout,"N(CO) = %e*%e*%e = %e\n",X,TAU,W,N);
*/




}


double	J(T,nu)
double	T, nu;
{
double	J, c;
c=PLNK/BLTM*(nu*1.0e9);
J=c/(exp(c/T)-1.0);
return(J);
}


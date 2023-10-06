#include	<stdio.h>
#include	<math.h>

#define	MAX_DATA_NUM	2048
#define	MAX_CHAR_LENGTH	256
#define	MAX_ITERATION	10000
#define	RAND_TRIAL_NUM	1000
#define	EPS	1.0e-10

#define BLTM    (1.38054e-23)
#define PLNK    (6.62559e-34)
#define LIGHT   (2.99792458e8)
#define ATOM_UNIT   (1.66053873e-27)
#define	PI	(3.1415927)


double	A32, B32, C32;
double	A34, B34, C34;
double	I32, I34;
extern	double	f32();
extern	double	f34();
extern	double	max();

int	main(argc,argv)
int	argc;
char	*argv[];
{
double	Br32, Br34, sigma_Br32, sigma_Br34;
double	dBr32, dBr34;
double	mo, mc, ms, M;
double	r1, r2, dr1, dr2;
int	ii, jj, it;
double	r10, r20, sigma_r1, sigma_r2;
double	rnd1, rnd2;
double	s1, ss1, s2, ss2;
double	r1m, r2m;

/*	rotational constants (measured)*/
Br32 = 6.0815e9;	sigma_Br32 = 0.03115e6;
Br34 = 5.9329e9;	sigma_Br34 = 0.03115e6*2.0;

/*	atomic constants (from literature)	*/
mo = 16.0*ATOM_UNIT;	mc = 12.0*ATOM_UNIT;
ms = 32.0*ATOM_UNIT;	M = (mo+mc+ms);
A32 = (mc+ms)*mo/M;	B32 = 2.0*mo*ms/M;	C32 = (mc+mo)*ms/M;
A32 /= ATOM_UNIT;	B32 /= ATOM_UNIT;	C32 /= ATOM_UNIT;
ms = 34.0*ATOM_UNIT;	M = (mo+mc+ms);
A34 = (mc+ms)*mo/M;	B34 = 2.0*mo*ms/M;	C34 = (mc+mo)*ms/M;
A34 /= ATOM_UNIT;	B34 /= ATOM_UNIT;	C34 /= ATOM_UNIT;

/*	most provable values	*/
I32 = PLNK/(8.0*PI*PI*Br32);	I34 = PLNK/(8.0*PI*PI*Br34);
I32 /= ATOM_UNIT*1.0e-20;	I34 /= ATOM_UNIT*1.0e-20;	
r1 = 1.16;	r2 = 1.56;
it = calc_r1r2(&r1, &r2);
r1m = r1;	r2m = r2;

/*	errors	*/
s1=0.0; s2=0.0; ss1=0.0; ss2=0.0;
for(ii=0;ii<RAND_TRIAL_NUM;ii++){

	rnd1 = rand()/(pow(2.0,31.0)-1.0);
	rnd2 = rand()/(pow(2.0,31.0)-1.0);
	dBr32 = sigma_Br32*pow((-2.0)*log(rnd1), 0.5)*cos(2.0*PI*rnd2);
	dBr34 = sigma_Br34*pow((-2.0)*log(rnd2), 0.5)*sin(2.0*PI*rnd1);

/*	fprintf(stderr,"%e\t%e\t%lf\t%lf\n",rnd1,rnd2,dBr32,dBr34);*/

	I32 = PLNK/(8.0*PI*PI*(Br32+dBr32));	
	I34 = PLNK/(8.0*PI*PI*(Br34+dBr34));
	I32 /= ATOM_UNIT*1.0e-20;	I34 /= ATOM_UNIT*1.0e-20;	

	r1 = 1.16;	r2 = 1.56;
	it = calc_r1r2(&r1, &r2);

/*	fprintf(stderr,"%lf\t%lf\t%d\n",r1,r2,it);	*/
	s1 += r1;	s2 += r2;
	ss1 += r1*r1;	ss2 += r2*r2;
}
sigma_r1=pow((ss1/(double)ii)-pow((s1/(double)ii),2.0),0.5);
sigma_r2=pow((ss2/(double)ii)-pow((s2/(double)ii),2.0),0.5);

fprintf(stderr,"r1 = %lf +- %lf\nr2 = %lf +- %lf\n",r1m,sigma_r1,r2m,sigma_r2);

}



int	calc_r1r2(r1p, r2p)
double	*r1p, *r2p;
{
int	ii;
double	r1, r2, dr1, dr2;
double	a, b, c, d;


r1=(*r1p);	r2=(*r2p);

for(ii=0;ii<MAX_ITERATION;ii++){
	a=2.0*A32*r1+B32*r2;	b=B32*r1+2.0*C32*r2;
	c=2.0*A34*r1+B34*r2;	d=B34*r1+2.0*C34*r2;
	
	dr1 = (b*f34(r1,r2)-d*f32(r1,r2))/(a*d-b*c);
	dr2 = (c*f32(r1,r2)-a*f34(r1,r2))/(a*d-b*c);

	if(max(fabs(dr1/r1),fabs(dr2/r2))<EPS)	break;

	r1 += dr1;	r2 += dr2;
}

(*r1p)=r1;	(*r2p)=r2;

if(ii<MAX_ITERATION)	return(ii);
else	return(-1);
}







double	f32(r1, r2)
double	r1, r2;
{
double	f;
f = A32*r1*r1 + B32*r1*r2 + C32*r2*r2 - I32;
return(f);
}

double	f34(r1, r2)
double	r1, r2;
{
double	f;
f = A34*r1*r1 + B34*r1*r2 + C34*r2*r2 - I34;
return(f);
}

double	max(x1, x2)
double	x1, x2;
{
if(x1>=x2)	return(x1);
else		return(x2);
}
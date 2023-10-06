#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define	DEBUG	0

#include <profhead.h>
#include <starlib.h>
#include <fitshead.h>

#define	DEBUG	0
#define	DISTANCE	8.5e3
#define	PI	3.1415927

#define	LOG_S_MIN	0.146
#define	LOG_S_MAX	1.5
#define	LOG_S_STP	0.1
#define	LOG_V_MIN	0.301
#define	LOG_V_MAX	1.5
#define	LOG_V_STP	0.1

#define	TP_MIN		10.0
#define	TP_MAX		40.0
#define	TP_STEP		2.0

#define	R_GRID	1.4
#define	V_GRID	2.0


main(argc,argv)
int	argc;
char	*argv[];
{
double	Tpeak, Tmin, T;
double	lS, lsv, S, Sd, sv, svd, sx, sy, sxd, syd;
double	r, x, y, V, I, L; 
double	rs_mean, rs2_mean, rv_mean, rv2_mean, rl_mean, rl2_mean;
double	err_rs, err_rv, err_rl;
int	data_num;

if(argc<2){
	fprintf(stderr,"Usage : %s (Tmin)\n",argv[0]);
	exit(999);
}

/*Tpeak = atof(argv[1]);*/
Tmin = atof(argv[1]);


for(Tpeak=Tmin*2.0;Tpeak<=TP_MAX;Tpeak=Tpeak+TP_STEP){

rs_mean=0.0;	rs2_mean=0.0; 
rv_mean=0.0; 	rv2_mean=0.0;
rl_mean=0.0; 	rl2_mean=0.0;

data_num=0;
for(lS=LOG_S_MIN;lS<=LOG_S_MAX;lS=lS+LOG_S_STP){
for(lsv=LOG_V_MIN;lsv<=LOG_V_MAX;lsv=lsv+LOG_V_STP){
	S = pow(10.0,lS);
	sv = pow(10.0,lsv);

	sxd=0.0; syd=0.0; svd=0.0; I=0.0; L=0.0;

	for(x=(-5.0)*S;x<=5.0*S;x=x+R_GRID){
	for(y=(-5.0)*S;y<=5.0*S;y=y+R_GRID){
		r = sqrt(x*x+y*y);		
	for(V=(-5.0)*sv;V<=5.0*sv;V=V+V_GRID){
		T = Tpeak*exp(-0.5*(pow((r/S),2.0) + pow((V/sv),2.0)));
		L = L + T;
		if(T>=Tmin){
			sxd = sxd + pow(x,2.0)*T;
			syd = syd + pow(y,2.0)*T;
			svd = svd + pow(V,2.0)*T;
			I = I + T;
		}
	}}}
	if(I==0.0)	continue;
	sxd = sqrt(sxd/I);	syd = sqrt(syd/I);
	Sd = sqrt(sxd*syd);
	svd = sqrt(svd/I);

/*	fprintf(stderr,"%lf\t%lf\t%lf\t%lf\n",S,Sd,sv,svd);	*/

	rs_mean = rs_mean + Sd/S;
	rs2_mean = rs2_mean + pow(Sd/S,2.0);
	rv_mean = rv_mean + svd/sv;
	rv2_mean = rv2_mean + pow(svd/sv,2.0);
	rl_mean = rl_mean + I/L;
	rl2_mean = rl2_mean + pow(I/L,2.0);
	data_num++;
}}
rs_mean = rs_mean/(double)data_num;
rs2_mean = rs2_mean/(double)data_num;
rv_mean = rv_mean/(double)data_num;
rv2_mean = rv2_mean/(double)data_num;
rl_mean = rl_mean/(double)data_num;
rl2_mean = rl2_mean/(double)data_num;

err_rs = pow((rs2_mean - pow(rs_mean, 2.0)), 0.5);
err_rv = pow((rv2_mean - pow(rv_mean, 2.0)), 0.5);
err_rl = pow((rl2_mean - pow(rl_mean, 2.0)), 0.5);
fprintf(stderr,"Sout/Sin = %lf +- %lf \n", rs_mean, err_rs);
fprintf(stderr,"svout/svin = %lf +- %lf \n", rv_mean, err_rv);
fprintf(stderr,"Lout/Lin = %lf +- %lf \n\n", rl_mean,err_rl);

fprintf(stdout,"%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf\n",
	Tpeak/Tmin,rs_mean, err_rs, rv_mean, err_rv, rl_mean, err_rl);
}	/*	Tpeak loop	*/




}
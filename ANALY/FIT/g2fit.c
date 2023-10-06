#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <math.h>

#include <profhead.h>
#include <starlib.h>
#include <fitshead.h>

#include <nucalc.h>

#define	DEBUG	0

typedef struct {
	float	*x;
	float	*y;
	long	num;
} SPECT;

typedef struct {/*	y = ag exp(-(x-xg)^2/sg^2) + al/(1 + ((x-xl)/sl)^2)	*/
	double	ag;
	double	sg;
	double	xg;
	double	al;
	double	sl;
	double	xl;
} GAUSS;

extern	int TwoGaussian_fit(SPECT *, GAUSS *);

main()
{
double	x,y;
long	num;
SPECT	spect;
GAUSS	gauss;
int	ii, jj, err_code;

spect.x = malloc(4);	spect.y = malloc(4);

num=0;
while(fscanf(stdin,"%lf %lf",&x,&y)==2){
	num++;
	spect.x = (float *)realloc(spect.x, 4*num);
	spect.y = (float *)realloc(spect.y, 4*num);
	spect.x[num-1]=x;	spect.y[num-1]=y;
}
spect.num=num;

#if DEBUG
for(ii=0;ii<spect.num;ii++){
	fprintf(stderr,"%lf\t%lf\n",spect.x[ii],spect.y[ii]);
}
fprintf(stderr,"num=%d\n",spect.num);
#endif

err_code=TwoGaussian_fit(&spect, &gauss);
if(err_code!=0){
	fprintf(stderr,"Error in the fitting procedure. \n");
	exit(999);
}

fprintf(stderr,"ag = %lf\t sg = %lf\t xg = %lf\n", gauss.ag, gauss.sg, gauss.xg);
fprintf(stderr,"al = %lf\t sl = %lf\t xl = %lf\n", gauss.al, gauss.sl, gauss.xl);

for(ii=0;ii<spect.num;ii++){
	y = gauss.ag * exp(-pow(((spect.x[ii]-gauss.xg)/gauss.sg),2.0)) + gauss.al * exp(-pow(((spect.x[ii]-gauss.xl)/gauss.sl),2.0)); 
	fprintf(stdout,"%lf\t%lf\t%lf\n",spect.x[ii],y,spect.y[ii]);
}


}


#define	IT_FACT	4.0
#define	EPS1	1.0e-4

int	TwoGaussian_fit(spect, gauss)
SPECT	*spect;
GAUSS	*gauss;
{
int	i, j, ii, jj;
int	ch_min, ch_max, chc;
double	ag, sg, xg, al, sl, xl;
double	sgn, xgn, sln, xln;
double	dxg, dxl, dsg, dsl;
double	xg_p, xl_p, sg_p, sl_p;
double	xg_m, xl_m, sg_m, sl_m;
short	err_code;
double	I, Ip, Im;
double	a[2][2], b[2];
float	max, xc, width;


/* Setting initial values */
max=(-999.9);
for(ii=0;ii<(*spect).num;ii++){
	if((*spect).y[ii]>max)	{max=(*spect).y[ii]; xc=(*spect).x[ii]; chc=ii;}
}
width=fabs((*spect).x[0]-(*spect).x[(*spect).num-1]);
/*ag=(double)max; al=(double)max/5.0;*/
xg=(double)xc; xl=xg; 
sg=width/100.0; sl=sg*3.0;
dxg=sg/IT_FACT; dxl=xl/IT_FACT;
dsg=sg/IT_FACT; dsl=sl/IT_FACT;


for(i=0;i<MAX_ITERATION;i++){
ch_min=chc-(int)(sg*5.0/width*(double)(*spect).num); ch_max=chc+(int)(sg*5.0/width*(double)(*spect).num); 
if(ch_min<0) ch_min=0;	if(ch_max>=(*spect).num) ch_max=(*spect).num-1; 
/*  Linear Part  */
	for(ii=0;ii<1;ii++){for(jj=0;jj<1;jj++){a[ii][jj]=0.0;}b[ii]=0.0;}
	for(ii=ch_min;ii<ch_max;ii++){
		a[0][0]=a[0][0]+exp(-pow((((*spect).x[ii]-xg)/sg),2.0))*exp(-pow((((*spect).x[ii]-xg)/sg),2.0));
		a[0][1]=a[0][0]+exp(-pow((((*spect).x[ii]-xg)/sg),2.0))*exp(-pow((((*spect).x[ii]-xl)/sl),2.0));
		a[1][1]=a[1][1]+exp(-pow((((*spect).x[ii]-xl)/sl),2.0))*exp(-pow((((*spect).x[ii]-xl)/sl),2.0));
		b[0]=b[0]+exp(-pow((((*spect).x[ii]-xg)/sg),2.0))*(*spect).y[ii];
		b[1]=b[1]+exp(-pow((((*spect).x[ii]-xl)/sl),2.0))*(*spect).y[ii];
	}
		a[1][0]=a[0][1];
	err_code=sweepout(a,b,2,2,EPS);	
	ag=b[0]; al=b[1];
	if(ag<0.0) ag=fabs(ag);	if(al<0.0) al=fabs(al); 

/*  Non-linear Part  */
	I=0.0; Ip=0.0; Im=0.0; 
	for(ii=ch_min;ii<ch_max;ii++){
		I=I+pow(((ag*exp(-pow((((*spect).x[ii]-xg)/sg),2.0))+al*exp(-pow((((*spect).x[ii]-xl)/sl),2.0)))-(*spect).y[ii]),2.0);
		Ip=Ip+pow(((ag*exp(-pow((((*spect).x[ii]-(xg+dxg))/sg),2.0))+al*exp(-pow((((*spect).x[ii]-xl)/sl),2.0)))-(*spect).y[ii]),2.0);
		Im=Im+pow(((ag*exp(-pow((((*spect).x[ii]-(xg-dxg))/sg),2.0))+al*exp(-pow((((*spect).x[ii]-xl)/sl),2.0)))-(*spect).y[ii]),2.0);
	}
	if((I<=Ip)&&(I<=Im))	{dxg=dxg/IT_FACT; xgn=xg;}	
	else if((I>Ip)&&(I<Im))	xgn=xg+dxg;
	else if((I<Ip)&&(I>Im))	xgn=xg-dxg;

	Ip=0.0; Im=0.0; 
	for(ii=ch_min;ii<ch_max;ii++){
		Ip=Ip+pow(((ag*exp(-pow((((*spect).x[ii]-xg)/(sg+dsg)),2.0))+al*exp(-pow((((*spect).x[ii]-xl)/sl),2.0)))-(*spect).y[ii]),2.0);
		Im=Im+pow(((ag*exp(-pow((((*spect).x[ii]-xg)/(sg-dsg)),2.0))+al*exp(-pow((((*spect).x[ii]-xl)/sl),2.0)))-(*spect).y[ii]),2.0);
	}
	if((I<=Ip)&&(I<=Im))	{dsg=dsg/IT_FACT; sgn=sg;}	
	else if((I>Ip)&&(I<Im))	sgn=sg+dsg;
	else if((I<Ip)&&(I>Im))	sgn=sg-dsg;

	Ip=0.0; Im=0.0; 
	for(ii=ch_min;ii<ch_max;ii++){
		Ip=Ip+pow(((ag*exp(-pow((((*spect).x[ii]-xg)/sg),2.0))+al*exp(-pow((((*spect).x[ii]-(xl+dxl))/sl),2.0)))-(*spect).y[ii]),2.0);
		Im=Im+pow(((ag*exp(-pow((((*spect).x[ii]-xg)/sg),2.0))+al*exp(-pow((((*spect).x[ii]-(xl-dxl))/sl),2.0)))-(*spect).y[ii]),2.0);
	}
	if((I<=Ip)&&(I<=Im))	{dxl=dxl/IT_FACT; xln=xl;}	
	else if((I>Ip)&&(I<Im))	xln=xl+dxl;
	else if((I<Ip)&&(I>Im))	xln=xl-dxl;

	Ip=0.0; Im=0.0; 
	for(ii=ch_min;ii<ch_max;ii++){
		Ip=Ip+pow(((ag*exp(-pow((((*spect).x[ii]-xg)/sg),2.0))+al*exp(-pow((((*spect).x[ii]-xl)/(sl+dsl)),2.0)))-(*spect).y[ii]),2.0);
		Im=Im+pow(((ag*exp(-pow((((*spect).x[ii]-xg)/sg),2.0))+al*exp(-pow((((*spect).x[ii]-xl)/(sl-dsl)),2.0)))-(*spect).y[ii]),2.0);
	}
	if((I<=Ip)&&(I<=Im))	{dsl=dsl/IT_FACT; sln=sl;}	
	else if((I>Ip)&&(I<Im))	sln=sl+dsl;
	else if((I<Ip)&&(I>Im))	sln=sl-dsl;

	xg=xgn; sg=sgn; xl=xln; sl=sln;

	if((fabs(dsg/sg)<EPS1)&&(fabs(dsl/sl)<EPS1)&&
		(fabs(dxg/xg)<EPS1)&&(fabs(dxl/xl)<EPS1) )	break;

	fprintf(stderr,"%d\t%lf\t%lf\t%lf\t%lf\n",i,dxg/xg,dsg/sg,dxl/xl,dsl/sl);
}

(*gauss).ag = ag; (*gauss).sg = sg; (*gauss).xg = xg;	
(*gauss).al = al; (*gauss).sl = sl; (*gauss).xl = xl;

return(0);
}

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


extern int	Gaussians_fit(SPECT *, GAUSS *, double, double, double);
extern int	SortGaussians(GAUSS *);
extern double	weight(double, double, double);


#define	IT_FACT	4.0
#define	EPS1	1.0e-4

int	Gaussians_fit(spect, gauss, width, peak, fit_width)
SPECT	*spect;
GAUSS	*gauss;
double	width, peak, fit_width;
{
int	i, j, ii, jj;
int	ch_min, ch_max, chc;
double	ag, bg, cg, sg, xg;
double	sgn, xgn;
double	dxg, dsg;
short	err_code;
double	I, Ip, Im;
double	a[3][3], b[3];
float	max, xc, band, yr, yr2, rms;
int	irr;


/* Calculating the rms */
/*
yr=0.0; yr2=0.0;
for(ii=(*spect).num/10*8;ii<(*spect).num/10*9;ii++){
	yr=yr+(*spect).y[ii];	yr2=yr2+(*spect).y[ii]*(*spect).y[ii];
}
	yr=yr/(float)((*spect).num/10); yr2=yr2/(float)((*spect).num/10);
	rms=pow(yr2-yr*yr, 0.5);
*/

/* Gaussian seraching loop */
(*gauss).num=0;
for(i=0;i<GAUSSIAN_NUM;i++){irr=0;

/* Setting initial values */
max=(-999.9);	
for(ii=0;ii<(*spect).num;ii++){
	if((*spect).y[ii]>max)	{max=(*spect).y[ii]; xc=(*spect).x[ii]; chc=ii;}
}
band=fabs((*spect).x[0]-(*spect).x[(*spect).num-1]);
xg=(double)xc; sg=width; dxg=sg/IT_FACT; dsg=sg/IT_FACT;
if(max<peak)	break;


/* Non-linear fit iteration */
for(j=0;j<MAX_ITERATION/5;j++){

ch_min=chc-(int)(sg*fit_width/band*(double)(*spect).num)-1; 
ch_max=chc+(int)(sg*fit_width/band*(double)(*spect).num)+1; 
if((ch_max<0)||(ch_min>=(*spect).num)){	irr=1;	break;}
if(ch_min<0) ch_min=0; if(ch_max>=(*spect).num) ch_max=(*spect).num-1;	

/*  Linear Part  */
	for(ii=0;ii<2;ii++){for(jj=0;jj<2;jj++){a[ii][jj]=0.0;}b[ii]=0.0;}
	for(ii=ch_min;ii<=ch_max;ii++){
		a[0][0] = a[0][0] + 1.0*weight((*spect).x[ii],xg,sg);
		a[0][1] = a[0][1] + exp(-pow(((*spect).x[ii]-xg)/sg,2.0))*weight((*spect).x[ii],xg,sg);
		a[1][1] = a[1][1] + exp(-pow(((*spect).x[ii]-xg)/sg,2.0))*exp(-pow(((*spect).x[ii]-xg)/sg,2.0))*weight((*spect).x[ii],xg,sg);
		b[0] = b[0] + (*spect).y[ii]*weight((*spect).x[ii],xg,sg);
		b[1] = b[1] + exp(-pow(((*spect).x[ii]-xg)/sg,2.0))*(*spect).y[ii]*weight((*spect).x[ii],xg,sg);
	}
		a[1][0] = a[0][1];	
	err_code=sweepout(a,b,3,2,EPS);	
	if(err_code!=0){	irr=1;	break;}

	ag=b[0];  bg=0.0; cg=b[1];
	if(ag<0.0) ag=0.0;	if(cg<0.0) cg=fabs(cg);

/*  Non-linear Part  */
	I=0.0; Ip=0.0; Im=0.0; 
	for(ii=ch_min;ii<=ch_max;ii++){
		I=I+pow( ( ag + bg*((*spect).x[ii]-xg) + cg*exp(-pow(((*spect).x[ii]-xg)/sg,2.0)) - (*spect).y[ii] )*weight((*spect).x[ii],xg,sg),2.0);
		Ip=Ip+pow( ( ag + bg*((*spect).x[ii]-xg) + cg*exp(-pow(((*spect).x[ii]-xg)/(sg+dsg),2.0)) - (*spect).y[ii] )*weight((*spect).x[ii],xg,sg),2.0);
		Im=Im+pow( ( ag + bg*((*spect).x[ii]-xg) + cg*exp(-pow(((*spect).x[ii]-xg)/(sg-dsg),2.0)) - (*spect).y[ii] )*weight((*spect).x[ii],xg,sg),2.0);
	}
	if((I<=Ip)&&(I<=Im))	{dsg=dsg/IT_FACT; sgn=sg;}
	else if((I>Ip)&&(I<Im))	sgn=sg+dsg;
	else if((I<Ip)&&(I>Im))	sgn=sg-dsg;

	Ip=0.0; Im=0.0; 
	for(ii=ch_min;ii<=ch_max;ii++){
		Ip=Ip+pow( ( ag + bg*((*spect).x[ii]-xg) + cg*exp(-pow(((*spect).x[ii]-(xg+dxg))/sg,2.0)) - (*spect).y[ii] )*weight((*spect).x[ii],xg,sg),2.0);
		Im=Im+pow( ( ag + bg*((*spect).x[ii]-xg) + cg*exp(-pow(((*spect).x[ii]-(xg-dxg))/sg,2.0)) - (*spect).y[ii] )*weight((*spect).x[ii],xg,sg),2.0);
	}
	if((I<=Ip)&&(I<=Im))	{dxg=dxg/IT_FACT; xgn=xg;}	
	else if((I>Ip)&&(I<Im))	xgn=xg+dxg;
	else if((I<Ip)&&(I>Im))	xgn=xg-dxg;

	xg=xgn; sg=sgn;
	if(sg<=0.0) sg=sg+dsg/2.0;
	if((fabs(dsg/sg)<EPS1)&&(fabs(dxg/xg)<EPS1))	break;

}	/* j loop */

#if DEBUG
fprintf(stderr,"%d %d\t%lf\t%lf\t%lf\t%lf\t%lf\t%d\n",i,j, ag,bg,cg,xg,sg,irr);
#endif

if(irr==0){
(*gauss).ag[(*gauss).num] = ag; 
(*gauss).bg[(*gauss).num] = bg; 
(*gauss).cg[(*gauss).num] = cg; 
(*gauss).sg[(*gauss).num] = sg; 
(*gauss).xg[(*gauss).num] = xg;	
(*gauss).num++;
}
for(ii=ch_min;ii<=ch_max;ii++){
	(*spect).y[ii]=(*spect).y[ii]-cg*exp(-pow(((*spect).x[ii]-xg)/sg,2.0));
}

}	/* i loop */


return(0);

}


int	SortGaussian(gauss)
GAUSS	*gauss;
{
int	ii, jj;
double	a, b, c, x, s;

if((*gauss).num==0){
	(*gauss).ag[0]=0.0;	(*gauss).bg[0]=0.0;	(*gauss).cg[0]=0.0;
	(*gauss).xg[0]=0.0;	(*gauss).sg[0]=0.0;
}

for(ii=0;ii<(*gauss).num-1;ii++){
	for(jj=ii+1;jj<(*gauss).num;jj++){
		if((*gauss).sg[ii]<(*gauss).sg[jj]){
			a=(*gauss).ag[ii]; (*gauss).ag[ii]=(*gauss).ag[jj]; (*gauss).ag[jj]=a;
			b=(*gauss).bg[ii]; (*gauss).bg[ii]=(*gauss).bg[jj]; (*gauss).bg[jj]=b;
			c=(*gauss).cg[ii]; (*gauss).cg[ii]=(*gauss).cg[jj]; (*gauss).cg[jj]=c;
			x=(*gauss).xg[ii]; (*gauss).xg[ii]=(*gauss).xg[jj]; (*gauss).xg[jj]=x;
			s=(*gauss).sg[ii]; (*gauss).sg[ii]=(*gauss).sg[jj]; (*gauss).sg[jj]=s;
		}
	}
}

return(0);
}


double	weight(x, xs, s)
double	x, xs, s;
{
double	w;

/* w=1.0/( 1.0 + 1.0*exp(-pow(((x-xs)/s), 2.0)) );*/	
w=1.0;

return(w);
}

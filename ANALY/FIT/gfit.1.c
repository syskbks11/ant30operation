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
#define	GAUSSIAN_NUM	10
typedef struct {
	float	*x;
	float	*y;
	long	num;
} SPECT;

typedef struct {/*	y = a + b (x-xg) + c exp(-(x-xg)^2/s^2) 	*/
	double	ag[GAUSSIAN_NUM];
	double	bg[GAUSSIAN_NUM];
	double	cg[GAUSSIAN_NUM];
	double	sg[GAUSSIAN_NUM];
	double	xg[GAUSSIAN_NUM];
	int	num;
} GAUSS;

extern	int Gaussians_fit(SPECT *, GAUSS *);

main()
{
double	x,y;
long	num;
SPECT	spect, spect_new;
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
spect_new.x = (float *)malloc(4*num);
spect_new.y = (float *)malloc(4*num);

#if DEBUG
for(ii=0;ii<spect.num;ii++){
	fprintf(stderr,"%lf\t%lf\n",spect.x[ii],spect.y[ii]);
}
fprintf(stderr,"num=%d\n",spect.num);
#endif


for(ii=0;ii<spect.num;ii++){
	spect_new.x[ii]=spect.x[ii]; spect_new.y[ii]=spect.y[ii]; spect_new.num=spect.num;
}

err_code=Gaussians_fit(&spect_new, &gauss);
if(err_code!=0){
	fprintf(stderr,"Error in the fitting procedure. \n");
	exit(999);
}

for(jj=0;jj<gauss.num;jj++){
fprintf(stderr,"%d: a = %lf\t b = %lf\t c = %lf\t xg = %lf\t sg = %lf\n"
	, jj, gauss.ag[jj], gauss.bg[jj], gauss.cg[jj], gauss.xg[jj], gauss.sg[jj]);
}

for(ii=0;ii<spect.num;ii++){
	fprintf(stdout,"%lf\t%lf\t",spect.x[ii],spect.y[ii]);
	for(jj=0;jj<gauss.num;jj++){
		y = gauss.cg[jj]*exp(-pow(((spect.x[ii]-gauss.xg[jj])/gauss.sg[jj]),2.0)) ; 
		fprintf(stdout,"%lf\t",y);
	}
	fprintf(stdout,"\n");
}


}


#define	IT_FACT	4.0
#define	EPS1	1.0e-4

int	Gaussians_fit(spect, gauss)
SPECT	*spect;
GAUSS	*gauss;
{
int	i, j, ii, jj;
int	ch_min, ch_max, chc;
double	ag, bg, cg, sg, xg;
double	sgn, xgn;
double	dxg, dsg;
short	err_code;
double	I, Ip, Im;
double	a[3][3], b[3];
float	max, xc, width, yr, yr2, rms;


for(i=0;i<GAUSSIAN_NUM;i++){

/* Setting initial values */
max=(-999.9);
for(ii=0;ii<(*spect).num;ii++){
	if((*spect).y[ii]>max)	{max=(*spect).y[ii]; xc=(*spect).x[ii]; chc=ii;}
}
width=fabs((*spect).x[0]-(*spect).x[(*spect).num-1]);
xg=(double)xc; sg=width/75.0; dxg=sg/IT_FACT; dsg=sg/IT_FACT; 

yr=0.0; yr2=0.0;
for(ii=(*spect).num/8*2;ii<(*spect).num/8*3;ii++){
	yr=yr+(*spect).y[ii];	yr2=yr2+(*spect).y[ii]*(*spect).y[ii];
}
	yr=yr/(float)((*spect).num/8); yr2=yr2/(float)((*spect).num/8);
	rms=pow(yr2-yr*yr, 0.5);
if(max<rms*5.0){
	(*gauss).num=i;
	return(0);
}


for(j=0;j<MAX_ITERATION;j++){
ch_min=chc-(int)(sg*2.0/width*(double)(*spect).num); ch_max=chc+(int)(sg*2.0/width*(double)(*spect).num); 
if(ch_min<0) ch_min=0;	if(ch_max>=(*spect).num) ch_max=(*spect).num-1; 
/*  Linear Part  */
	for(ii=0;ii<3;ii++){for(jj=0;jj<3;jj++){a[ii][jj]=0.0;}b[ii]=0.0;}
	for(ii=ch_min;ii<ch_max;ii++){
		a[0][0] = a[0][0] + 1.0;
		a[0][1] = a[0][1] + ((*spect).x[ii]-xg);
		a[0][2] = a[0][2] + exp(-pow(((*spect).x[ii]-xg)/sg,2.0));
		a[1][1] = a[1][1] + ((*spect).x[ii]-xg)*((*spect).x[ii]-xg);
		a[1][2] = a[1][2] + ((*spect).x[ii]-xg)*exp(-pow(((*spect).x[ii]-xg)/sg,2.0));			
		a[2][2] = a[2][2] + exp(-pow(((*spect).x[ii]-xg)/sg,2.0))*exp(-pow(((*spect).x[ii]-xg)/sg,2.0));
		b[0] = b[0] + (*spect).y[ii];
		b[1] = b[1] + ((*spect).x[ii]-xg)*(*spect).y[ii];
		b[2] = b[2] + exp(-pow(((*spect).x[ii]-xg)/sg,2.0))*(*spect).y[ii];
	}
		a[1][0] = a[0][1];	a[2][0] = a[0][2];	a[2][1] = a[1][2];
	err_code=sweepout(a,b,3,3,EPS);	if(err_code!=0){fprintf(stderr,"Error in sweepout()!\n"); exit(err_code);}
	ag=b[0]; bg=b[1]; cg=b[2];
	if(cg<0.0) cg=fabs(cg);


/*  Non-linear Part  */
	I=0.0; Ip=0.0; Im=0.0; 
	for(ii=ch_min;ii<ch_max;ii++){
		I=I+pow( ( ag + bg*((*spect).x[ii]-xg) + cg*exp(-pow(((*spect).x[ii]-xg)/sg,2.0)) - (*spect).y[ii] ),2.0);
		Ip=Ip+pow( ( ag + bg*((*spect).x[ii]-xg) + cg*exp(-pow(((*spect).x[ii]-(xg+dxg))/sg,2.0)) - (*spect).y[ii] ),2.0);
		Im=Im+pow( ( ag + bg*((*spect).x[ii]-xg) + cg*exp(-pow(((*spect).x[ii]-(xg-dxg))/sg,2.0)) - (*spect).y[ii] ),2.0);
	}
	if((I<=Ip)&&(I<=Im))	{dxg=dxg/IT_FACT; xgn=xg;}	
	else if((I>Ip)&&(I<Im))	xgn=xg+dxg;
	else if((I<Ip)&&(I>Im))	xgn=xg-dxg;

	Ip=0.0; Im=0.0; 
	for(ii=ch_min;ii<ch_max;ii++){
		Ip=Ip+pow( ( ag + bg*((*spect).x[ii]-xg) + cg*exp(-pow(((*spect).x[ii]-xg)/(sg+dsg),2.0)) - (*spect).y[ii] ),2.0);
		Im=Im+pow( ( ag + bg*((*spect).x[ii]-xg) + cg*exp(-pow(((*spect).x[ii]-xg)/(sg-dsg),2.0)) - (*spect).y[ii] ),2.0);
	}
	if((I<=Ip)&&(I<=Im))	{dsg=dsg/IT_FACT; sgn=sg;}
	else if((I>Ip)&&(I<Im))	sgn=sg+dsg;
	else if((I<Ip)&&(I>Im))	sgn=sg-dsg;

	xg=xgn; sg=sgn;

	if((fabs(dsg/sg)<EPS1)&&(fabs(dxg/xg)<EPS1))	break;
}

(*gauss).ag[i] = ag; (*gauss).bg[i] = bg; (*gauss).cg[i] = cg; (*gauss).sg[i] = sg; (*gauss).xg[i] = xg;	
(*gauss).num++;
for(ii=ch_min;ii<ch_max;ii++){
	(*spect).y[ii]=(*spect).y[ii]-cg*exp(-pow(((*spect).x[ii]-xg)/(sg-dsg),2.0));
}	/* j loop */
fprintf(stderr,"%d\n",i);

}	/* i loop */


}

#include	<math.h>
#include	<stdio.h>
#include	<nucalc.h>


#define		DEBUG	0


extern short	sweepout();

short	spline_interpolation(x,y,num,xx,yy)

double	x[], y[];
unsigned short	num;
double	xx;
double	*yy;

{
unsigned short	i,j,k;
unsigned short	ifix;
short		err_code;
double		h[SPLINE_MAX_DATA_NUM];
double		M[SPLINE_MAX_DATA_NUM];
double 		lambda[SPLINE_MAX_DATA_NUM], mu[SPLINE_MAX_DATA_NUM];
double		d[SPLINE_MAX_DATA_NUM]; 
double		array[SPLINE_MAX_DATA_NUM][SPLINE_MAX_DATA_NUM];
double		c[SPLINE_MAX_DATA_NUM][SPLINE_ORDER];
double		S;


for(i=0;i<num-1;i++){
	h[i]=x[i+1]-x[i];
}

lambda[0]=1.0; 
d[0]=0.0;
for(i=1;i<num-1;i++){
	lambda[i]=h[i]/(h[i-1]+h[i]);
	mu[i]=1.0-lambda[i];
	d[i]=6.0/(h[i-1]+h[i])*( (y[i+1]-y[i])/h[i]-(y[i]-y[i-1])/h[i-1] );
}
mu[num-1]=1.0;
d[num-1]=0.0;

for(i=0;i<num;i++){
	for(j=0;j<num;j++){
		if(i+1==j)		array[i][j]=lambda[i];
		else if(i==j)	array[i][j]=2.0;
		else if(i==j+1)	array[i][j]=mu[i];
		else			array[i][j]=0.0;
	}
}

err_code=sweepout(array,d,SPLINE_MAX_DATA_NUM,num,EPS);
if(err_code!=0){
	fprintf(stderr,"Error in sweepout(). Error code = %d\n",err_code);
	return(100);
}

for(i=0;i<num;i++){
	M[i]=d[i];
}

for(i=0;i<num-1;i++){
	c[i][2]=(M[i+1]-M[i])/(6.0*h[i]);
	c[i][1]=M[i]/2.0;
	c[i][0]=(y[i+1]-y[i])/h[i]-c[i][1]*h[i]-c[i][2]*h[i]*h[i];
}

for(i=0;i<num-1;i++){
	if( (x[i]<=xx)&&(xx<x[i+1]) ){
		ifix=i;
	}
	else	continue;
}

S= 	y[ifix]	+ c[ifix][0]*(xx-x[ifix])
			+ c[ifix][1]*(xx-x[ifix])*(xx-x[ifix])
			+ c[ifix][2]*(xx-x[ifix])*(xx-x[ifix])*(xx-x[ifix]);

*yy=S;

return(0);

}


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



main()
{
double	x, y, ys;
long	num;
SPECT	spect, spect_new;
GAUSS	gauss;
int	ii, jj, kk, ll, err_code;

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

err_code=Gaussians_fit(&spect_new, &gauss, 2.0, 1.0, 2.0);
if(err_code!=0){
	fprintf(stderr,"Error in the fitting procedure. \n");
}

/*
SortGaussian(&gauss);
*/

for(jj=0;jj<gauss.num;jj++){
fprintf(stderr,"%d: a = %lf\t c = %lf\t xg = %lf\t sg = %lf\n"
	, jj, gauss.ag[jj], gauss.cg[jj], gauss.xg[jj], gauss.sg[jj]);
}

for(ll=0;ll<gauss.num;ll++){
	for(ii=0;ii<spect.num;ii++){
		if((gauss.sg[ll]>=4.0)||(gauss.cg[ll]<2.0/2.0))
		spect_new.y[ii] = spect_new.y[ii] +
			gauss.cg[ll]*exp(-pow((spect.x[ii]-gauss.xg[ll])/gauss.sg[ll], 2.0));
		
	}
}

for(ii=0;ii<spect.num;ii++){
	fprintf(stdout,"%lf\t%lf\t%lf\t%lf\n",spect.x[ii],spect.y[ii],
		spect_new.y[ii],spect.y[ii]-spect_new.y[ii]);
}


}




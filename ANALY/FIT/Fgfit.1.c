/*	Velocity Smoothing FITS DATA 	*/

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


int	option(argc,argv,width,peak,wing)
int	argc;
char	*argv[];
double	*width, *peak, *wing;
{
int	ii, jj, count;
char	*token;

/* default values */

*width=3.0;
*peak=2.0;
*wing=5.0;

for(ii=1;ii<argc;ii++){
	if(argv[ii][0]=='-'){
		if(strcmp(argv[ii],"-iwidth")==0){
			ii++;
			*width=atof(argv[ii]);
		}
		else if(strcmp(argv[ii],"-peak")==0){
			ii++;
			*peak=atof(argv[ii]);
		}
		else if(strcmp(argv[ii],"-wing")==0){
			ii++;
			*wing=atof(argv[ii]);
		}
	}
}


return(0);
}




int	FitGaussiansToFits(fits, fits_new, width, peak, wing)
FITS	*fits, *fits_new;
double	width, peak, wing;
{
long	ch, n, pix[MAX_DIMENSION];
int	ii, jj, kk;
int	err_code;
long	data_number;
SPECT	spect;
GAUSS	gauss;
double	int_q, int_w;

copy_FitsHeader(fits, fits_new);
for(jj=1;jj<=(*fits).head.naxis[0];jj++){
	if((*fits).head.naxis[jj]<=1)	(*fits).head.naxis[0]--;	
}
(*fits_new).head.naxis[3]=3;

data_number=1;
for(ii=1;ii<=(*fits_new).head.naxis[0];ii++){
	data_number=data_number*(*fits_new).head.naxis[ii];
}
(*fits_new).data=(float *)calloc(data_number,4);


spect.x = calloc((*fits).head.naxis[3],4);	
spect.y = calloc((*fits).head.naxis[3],4);
spect.num = (*fits).head.naxis[3];

for(ii=0;ii<(*fits).head.naxis[1];ii++){
for(jj=0;jj<(*fits).head.naxis[2];jj++){
/*	int_q=0.0;
*/	for(kk=0;kk<spect.num;kk++){
		spect.x[kk]=(*fits).head.crval[3] + 
			(*fits).head.cdelt[3] * ((double)(kk+1)-(*fits).head.crpix[3]);
		pix[0]=ii; pix[1]=jj; pix[2]=kk;
		spect.y[kk] = (*fits).data[DataPosition(fits,pix)];
/*		if(Blank((float)spect.y[kk])==0)
			int_q = int_q + spect.y[kk]*fabs((*fits).head.cdelt[3]); 			
*/	}
/*	if(int_q==0.0)	int_q=DEFAULT_BLANK_VALUE;
*/	err_code=Gaussians_fit(&spect, &gauss, width, peak);
	SortGaussian(&gauss);

	for(kk=0;kk<gauss.num;kk++){
		gauss.sg[kk]=gauss.sg[kk]*fabs((*fits).head.cdelt[3]);
	}
/*
	if((gauss.sg[0]>=wing)&&(gauss.cg[0]>=peak))	
		int_w = gauss.cg[0]*gauss.sg[0]*pow(PI,0.5); 
	else	int_w = DEFAULT_BLANK_VALUE;
*/
	pix[2]=0;	(*fits_new).data[DataPosition(fits_new,pix)] = (float)gauss.cg[0];
	pix[2]=1;	(*fits_new).data[DataPosition(fits_new,pix)] = (float)gauss.xg[0];
	pix[2]=2;	(*fits_new).data[DataPosition(fits_new,pix)] = (float)gauss.sg[0];

fprintf(stderr,"%d %d\t%5.2lf\t%5.2lf\t%5.2lf\t%5.2lf\t%5.2lf\n",
		ii,jj,int_q,int_w,
		(float)gauss.cg[0],(float)gauss.xg[0],(float)gauss.sg[0]);

}
}	



return(0);
}







main(argc,argv)
int	argc;
char	*argv[];
{
FILE		*fp_in, *fp_out;
int		err_code;
FITS		fits, fits_new;
int		ii, jj;
int		dim;
double		width, peak, wing;


err_code=option(argc,argv, &width, &peak, &wing);
if(err_code!=0){
	fprintf(stderr,"Error in reading option switch. Error code=%d\n",err_code);	
	exit(999);
}

fp_in=stdin;
fp_out=stdout;

fprintf(stderr,"Reading the FITS file.\n");
err_code=read_Fits(&fits, fp_in);
if(err_code!=0){
	fprintf(stderr,
	"Error in reading the FITS file. Error code=%d\n"
	,err_code);	
	exit(999);
}


fprintf(stderr,"Searching high-velocity wing emission.\n");
err_code=FitGaussiansToFits(&fits, &fits_new, width, peak, wing);
if(err_code!=0){
	fprintf(stderr,
	"Error in smoothing the FITS data. Error code=%d\n"
	,err_code);	
	exit(999);
}


fprintf(stderr,"Writing a new FITS file.\n");
err_code=output_Fits(&fits_new, fp_out);
if(err_code!=0){
	fprintf(stderr,
	"Error in writing the FITS file. Error code=%d\n"
	,err_code);	
	exit(999);
}


}	/*main*/



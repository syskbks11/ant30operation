/*	Fit Multiple-Gaussians to the spectra at each position
	and return the parameters of Gaussians with largest widths	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <profhead.h>
#include <starlib.h>
#include <fitshead.h>

#include <nucalc.h>

#define	DEBUG	0


int	option(argc,argv,width,peak,wing,fit_w)
int	argc;
char	*argv[];
double	*width, *peak, *wing, *fit_w;
{
int	ii, jj, count;
char	*token;

/* default values */

*width=3.0;
*peak=1.0;
*wing=4.0;
*fit_w=2.5;

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
		else if(strcmp(argv[ii],"-fit")==0){
			ii++;
			*fit_w=atof(argv[ii]);
		}
	}
}


return(0);
}




int	FitGaussiansToFits(fits, fits_new, width, peak, wing, fit_w)
FITS	*fits, *fits_new;
double	width, peak, wing, fit_w;
{
long	ch, n, pix[MAX_DIMENSION];
int	ii, jj, kk, ll;
int	err_code;
long	data_number;
SPECT	spect;
GAUSS	gauss;
double	int_q, int_w;

copy_FitsHeader(fits, fits_new);
for(jj=1;jj<=(*fits).head.naxis[0];jj++){
	if((*fits).head.naxis[jj]<=1)	(*fits).head.naxis[0]--;	
}
copy_FitsHeader(fits, fits_new);

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
	for(kk=0;kk<(*fits).head.naxis[3];kk++){
		spect.x[kk]=(float)( (*fits).head.crval[3] + 
			(*fits).head.cdelt[3] * ((double)(kk+1)-(*fits).head.crpix[3]) );
		pix[0]=ii; pix[1]=jj; pix[2]=kk;
		spect.y[kk] = (float)( (*fits).data[DataPosition(fits,pix)] );
	}
	err_code=Gaussians_fit(&spect, &gauss, width, peak, fit_w);
	SortGaussian(&gauss);

	for(ll=0;ll<gauss.num;ll++){
	for(kk=0;kk<spect.num;kk++){
		if((gauss.sg[ll]>=wing)||(gauss.cg[ll]<peak/2.0))
		spect.y[kk] = spect.y[kk] +
			gauss.cg[ll]*exp(-pow((spect.x[kk]-gauss.xg[ll])/gauss.sg[ll], 2.0));
		
	}
	}
	

	for(kk=0;kk<(*fits).head.naxis[3];kk++){
		pix[2]=kk;
		(*fits_new).data[DataPosition(fits_new,pix)] = spect.y[kk];
	}

fprintf(stderr,"%d %d\t%5.2lf\t%5.2lf\t%5.2lf\t%d\n",
		ii,jj,(float)gauss.cg[0],(float)gauss.xg[0],(float)gauss.sg[0],gauss.num);

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
double		width, peak, wing, fit_w;


err_code=option(argc,argv, &width, &peak, &wing, &fit_w);
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
err_code=FitGaussiansToFits(&fits, &fits_new, width, peak, wing, fit_w);
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



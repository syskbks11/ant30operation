/*	Velocity integration of a FITS DATA 	*/

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <math.h>

#include <profhead.h>
#include <starlib.h>
#include <fitshead.h>

#define	DEBUG	0
#define	RESTRICT	3.0
#define	GRID		3.0


int	option(argc,argv,v)
int	argc;
char	*argv[];
RANGE	*v;
{
int	ii, jj, count;
char	*token;

/* default values */

(*v).min=-100.0;
(*v).max=100.0;


for(ii=1;ii<argc;ii++){
	if(argv[ii][0]=='-'){
		if(strcmp(argv[ii],"-v")==0){
			ii++;
			token=strtok(argv[ii],",");
			if(token!=NULL)	(*v).min=atof(token);
			token=strtok(NULL,",");
			if(token!=NULL)	(*v).max=atof(token);
		}
	}
}


return(0);
}




int	Calc_Integrated_Intensity(fits, fits_new, v)
FITS	*fits, *fits_new;
RANGE	*v;
{
long	ch, n, pix[MAX_DIMENSION];
RANGE_long	vch;
int	ii, jj;
long	data_number;
double	x, x2, disp, w;



copy_FitsHeader(fits, fits_new);
for(jj=1;jj<=(*fits).head.naxis[0];jj++){
	if((*fits).head.naxis[jj]<=1)	(*fits).head.naxis[0]--;	
}
(*fits_new).head.naxis[0]=(*fits).head.naxis[0]-1;


data_number=1;
for(ii=1;ii<=(*fits_new).head.naxis[0];ii++){
	data_number=data_number*(*fits_new).head.naxis[ii];
}
(*fits_new).data=(float *)calloc(data_number,4);

vch.min= (long)( ((*v).min-(*fits).head.crval[3])/(*fits).head.cdelt[3] + (*fits).head.crpix[3] - 1 );
vch.max= (long)( ((*v).max-(*fits).head.crval[3])/(*fits).head.cdelt[3] + (*fits).head.crpix[3] - 1 );
STAR_SortRangeLong(&vch);

if(vch.min<0)	vch.min=0;
if(vch.max>((*fits).head.crval[3]+(*fits).head.cdelt[3]*((*fits).head.naxis[3]-(*fits).head.crpix[3])))	
		vch.max=(*fits).head.naxis[3]-1;


for(pix[0]=0;pix[0]<=(*fits).head.naxis[1]-1;pix[0]++){
for(pix[1]=0;pix[1]<=(*fits).head.naxis[2]-1;pix[1]++){
w=0.0;
for(pix[2]=vch.min;pix[2]<=vch.max;pix[2]++){
	ch=DataPosition(fits,pix);
	if(Blank((*fits).data[ch])==0){
		w=w+(double)(*fits).data[ch];
	}
}
if(w==0.0)	w=DEFAULT_BLANK_VALUE;
n = DataPosition(fits_new,pix);
(*fits_new).data[n] = (float)(w*fabs((*fits).head.cdelt[3]));
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
RANGE		v;


err_code=option(argc,argv,&v);
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


fprintf(stderr,"Calculating integrated intensities.\n");
err_code=Calc_Integrated_Intensity(&fits, &fits_new, &v);
if(err_code!=0){
	fprintf(stderr,
	"Error in calculating integrated intensities. Error code=%d\n"
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



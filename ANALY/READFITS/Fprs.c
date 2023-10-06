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





int	Calc_Integrated_Intensity(fits, fits_new, press)
FITS	*fits, *fits_new;
int	press;
{
long	ch, n, pix[MAX_DIMENSION];
RANGE_long	vch;
int	ii, jj;
int	i1,i2;
long	data_number, wn;
double	w;
float	min, max;

copy_FitsHeader(fits, fits_new);
(*fits_new).head.naxis[press]=1;
(*fits_new).head.crpix[press]=1;
min=(*fits).head.crval[press]
	+(*fits).head.cdelt[press]*(1.0-(*fits).head.crpix[press]);
max=(*fits).head.crval[press]
	+(*fits).head.cdelt[press]*((*fits).head.naxis[press]-(*fits).head.crpix[press]);
(*fits_new).head.crval[press]=(min+max)/2.0;
(*fits_new).data=(float *)calloc(DataNumber(fits_new),4);

if(press==1){	i1=2; i2=3;}
else if(press==2){i1=1; i2=3;}
else if(press==3){i1=1; i2=2;}

for(pix[i1-1]=0;pix[i1-1]<(*fits).head.naxis[i1];pix[i1-1]++){
for(pix[i2-1]=0;pix[i2-1]<(*fits).head.naxis[i2];pix[i2-1]++){
w=0.0; wn=0;
for(pix[press-1]=0;pix[press-1]<(*fits).head.naxis[press];pix[press-1]++){
	ch=PIXtoPOS(fits,pix);
	if(Blank((*fits).data[ch])==0){
		w=w+(double)(*fits).data[ch];
		wn++;
	}
}
pix[press-1]=0;
n = PIXtoPOS(fits_new,pix);
if(w==0.0)	(*fits_new).data[n] = DEFAULT_BLANK_VALUE;
else		(*fits_new).data[n] = (float)(w/(double)wn);
		/* (*fits_new).data[n] = (float)(w*fabs((*fits).head.cdelt[press])/(double)wn);
*/
}}


return(0);
}







main(argc,argv)
int	argc;
char	*argv[];
{
FILE	*fp_in, *fp_out;
int	err_code;
FITS	fits, fits_new;
int	ii, jj;
int	dim;
int	press;
char	filehead[80];
char	out_filename[80];



fprintf(stderr,"Input FITS filename : ");
fscanf(stdin,"%s",filehead);
fp_in=open_Fits(filehead);
if(fp_in==NULL)	exit(1);


fprintf(stderr,"Reading the FITS file.\n");
err_code=read_Fits(&fits, fp_in);
if(err_code!=0){
	fprintf(stderr,
	"Error in reading the FITS file. Error code=%d\n"
	,err_code);	
	exit(999);
}
fclose(fp_in);

if(fits.head.naxis[0]<3){
	fprintf(stderr,"FITS dimension must be 3!\n");
	exit(2);
}

press=3;
fprintf(stderr,"Compresson dimension (1-%d) : ", fits.head.naxis[0]);
fscanf(stdin,"%d",&press);


fprintf(stderr,"Calculating integrated intensities.\n");
err_code=Calc_Integrated_Intensity(&fits, &fits_new, press);
if(err_code!=0){
	fprintf(stderr,
	"Error in calculating integrated intensities. Error code=%d\n"
	,err_code);	
	exit(999);
}

sprintf(out_filename,"%s.p%d.FITS",filehead,press);
fp_out=fopen(out_filename,"w");
if(fp_out==NULL){
	fprintf(stderr,"Can't open %s !\n",out_filename);
	exit(1);
}

fprintf(stderr,"Writing to %s\n",out_filename);
err_code=output_Fits(&fits_new, fp_out);
if(err_code!=0){
	fprintf(stderr,
	"Error in writing the FITS file. Error code=%d\n"
	,err_code);	
	exit(999);
}
fclose(fp_out);


}	/*main*/



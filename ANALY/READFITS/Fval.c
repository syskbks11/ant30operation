/*	Get a value at the specified position from a FITS DATA 	*/

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <math.h>

/*#include <profhead.h>
#include <starlib.h>*/
#include <fitshead.h>

#define	DEBUG	0






main(argc,argv)
int	argc;

char	*argv[];
{
FILE		*fp_in, *fp_out;
int		err_code;
FITS		fits, fits_new;
int		ii, jj;
int		dim;
char		in_filename[80];
char		filehead[80];
char		buffer[80];
char		*token;
long     	pix[MAX_DIMENSION], data_pos;
double     	x[MAX_DIMENSION], xx[MAX_DIMENSION], data;
double  	min, max, declination;
int     	ra, dec;
long	ch, n;
double	y, y2, rms;


if(argc<2){
	fprintf(stderr,"Usage: Fval (filename)\n");
	exit(1);
}

/*fp_in=stdin;*/
strcpy(buffer,argv[1]);
if((strstr(buffer,".fits")==NULL)&&(strstr(buffer,".FITS")==NULL)){	
	sprintf(in_filename,"%s.FITS",buffer);
	strcpy(filehead,buffer);
	fp_in=fopen(in_filename,"r");
	if(fp_in==NULL){
		sprintf(in_filename,"%s.fits",filehead);
		fp_in=fopen(in_filename,"r");
		if(fp_in==NULL){
			fprintf(stderr,"Can't open %s !\n",in_filename);
			exit(1);
		}
	}
}
else{	strncpy(filehead,buffer,strlen(buffer)-5);
	sprintf(in_filename,"%s",buffer);
	fp_in=fopen(in_filename,"r");
	if(fp_in==NULL){
		fprintf(stderr,"Can't open %s !\n",in_filename);
		exit(1);
	}
}fp_out=stdout;

err_code=read_Fits(&fits, fp_in);
if(err_code!=0){
	fprintf(stderr,
	"Error in reading the FITS file. Error code=%d\n"
	,err_code);	
	exit(999);
}




ra=(-1);        dec=(-1);
for(ii=1;ii<=fits.head.naxis[0];ii++){
        if((strstr(fits.head.ctype[ii],"RA")!=NULL)||
        (strstr(fits.head.ctype[ii],"GLON")!=NULL))  ra=ii;
        if((strstr(fits.head.ctype[ii],"DEC")!=NULL)||
        (strstr(fits.head.ctype[ii],"GLAT")!=NULL))  dec=ii;
}
if(dec==(-1))	declination=0.0;
else		declination=fits.head.crval[dec];


for(ii=1;ii<=fits.head.naxis[0];ii++){
	if(fits.head.naxis[ii]<=1){	pix[ii-1]=0; break;	}
	if(ii==ra){
		pix[ii-1]=tint((double)fits.head.crpix[ii] + 
                ((double)x[ii-1]-fits.head.crval[ii])/(fits.head.cdelt[ii]/cos(declination*PI/180.0)))-1;
		min=fits.head.crval[ii] 
			+ fits.head.cdelt[ii]/cos(declination*PI/180.0)*(1.0-fits.head.crpix[ii]);
		max=fits.head.crval[ii] 
			+ fits.head.cdelt[ii]/cos(declination*PI/180.0)*((double)fits.head.naxis[ii]-fits.head.crpix[ii]);
	}
	else{
		pix[ii-1]=tint((double)fits.head.crpix[ii] + 
                ((double)x[ii-1]-fits.head.crval[ii])/(fits.head.cdelt[ii]))-1;
		min=fits.head.crval[ii] + fits.head.cdelt[ii]*(1.0-fits.head.crpix[ii]);
		max=fits.head.crval[ii] + fits.head.cdelt[ii]*((double)fits.head.naxis[ii]-fits.head.crpix[ii]);
	}

        fprintf(stderr,"Axis #%d: (%lf, %lf) = ",ii,min,max);
	fscanf(stdin,"%lf",&(x[ii-1]));

	if(ii==ra)
		pix[ii-1]=tint((double)fits.head.crpix[ii] + 
                ((double)x[ii-1]-fits.head.crval[ii])/(fits.head.cdelt[ii]/cos(declination*PI/180.0)))-1;
	else	pix[ii-1]=tint((double)fits.head.crpix[ii] + 
                ((double)x[ii-1]-fits.head.crval[ii])/(fits.head.cdelt[ii]))-1;
}
fprintf(stderr,"\n");
for(ii=1;ii<=fits.head.naxis[0];ii++){
	if(ii==ra)
		xx[ii-1]=fits.head.crval[ii] +
		fits.head.cdelt[ii]/cos(declination*PI/180.0)*((double)(pix[ii-1]+1) - fits.head.crpix[ii]);
	else
		xx[ii-1]=fits.head.crval[ii] +
		fits.head.cdelt[ii]*((double)(pix[ii-1]+1) - fits.head.crpix[ii]);
	fprintf(stderr,"Pix #%d: = %d (Axis #%d: = %lf)\n",ii,pix[ii-1],ii,xx[ii-1]);
}


data_pos=DataPosition(&fits,pix);
fprintf(stderr,"Data pos = %ld\tdata_number=%ld\n\n",data_pos,fits.head.data_num);
if((data_pos<0)||(data_pos>=fits.head.data_num)){
	fprintf(stderr,"Beyond the data range. exit(1)\n");
	exit(1);
}
data=fits.data[data_pos];
fprintf(stdout,"Intensity = %lf\n",data);



n=0; y=0.0; y2=0.0;
for(ch=0;ch<fits.head.data_num;ch++){
	if(Blank(fits.data[ch])==(-1)){
		continue;
	}
	else{
		if(fits.data[ch]<0.0){y=y+fits.data[ch]; y2=y2+pow(fits.data[ch],2.0); n++;}
	}
}
rms=pow(y2/(double)n,0.5);
fprintf(fp_out,"RMS noise = %lf \n",rms);





}	/*main*/



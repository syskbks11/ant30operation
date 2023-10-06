/*	FITS -> Profile			*/
/*	output unit : [K] or [Jy/beam]	*/
/*	integrted for steradian		*/

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <math.h>

#include <profhead.h>
#include <starlib.h>
#include <fitshead.h>

#define	DEBUG	0
#define	GRID		3.0





int	Calc_Integrated_Intensity(fits, fits_new, sp, o_beam)
FITS	*fits, *fits_new;
int	sp;
double	o_beam;
{
long	ch, n, pix[MAX_DIMENSION];
RANGE_long	vch;
int	ii, jj;
int	i1,i2;
long	data_number, wn;
double	w;
float	min, max;

if(sp==1){	i1=2; i2=3;}
else if(sp==2){i1=1; i2=3;}
else if(sp==3){i1=1; i2=2;}


copy_FitsHeader(fits, fits_new);
(*fits_new).head.naxis[i1]=1;
(*fits_new).head.crpix[i1]=1;
min=(*fits).head.crval[i1]
	+(*fits).head.cdelt[i1]*(1.0-(*fits).head.crpix[i1]);
max=(*fits).head.crval[i1]
	+(*fits).head.cdelt[i1]*((*fits).head.naxis[i1]-(*fits).head.crpix[i1]);
(*fits_new).head.crval[i1]=(min+max)/2.0;
(*fits_new).head.naxis[i2]=1;
(*fits_new).head.crpix[i2]=1;
min=(*fits).head.crval[i2]
	+(*fits).head.cdelt[i2]*(1.0-(*fits).head.crpix[i2]);
max=(*fits).head.crval[i2]
	+(*fits).head.cdelt[i2]*((*fits).head.naxis[i2]-(*fits).head.crpix[i2]);
(*fits_new).head.crval[i2]=(min+max)/2.0;
(*fits_new).data=(float *)calloc(DataNumber(fits_new),4);

fprintf(stderr,"Target dimension : %d,%d,%d\n", 
	(*fits_new).head.naxis[1], (*fits_new).head.naxis[2], (*fits_new).head.naxis[3]);

for(pix[sp-1]=0;pix[sp-1]<(*fits).head.naxis[sp];pix[sp-1]++){
w=0.0; wn=0;
for(pix[i1-1]=0;pix[i1-1]<(*fits).head.naxis[i1];pix[i1-1]++){
for(pix[i2-1]=0;pix[i2-1]<(*fits).head.naxis[i2];pix[i2-1]++){
	ch=PIXtoPOS(fits,pix);
	if(Blank((*fits).data[ch])==0){
		w=w+(double)(*fits).data[ch];
		wn++;
	}
}}
/* fprintf(stderr,"%d\n",pix[sp-1]); */
pix[i1-1]=0; pix[i2-1]=0;
n = PIXtoPOS(fits_new,pix);
if(w==0.0)	(*fits_new).data[n] = DEFAULT_BLANK_VALUE;
else{	if(strncmp((*fits).head.bunit,"K",1)==0)
		(*fits_new).data[n] 
			= w * fabs((*fits).head.cdelt[i1]*(*fits).head.cdelt[i2])*3.0461742e-4 
				*2.413538916e-18/1.0e-26;
	else if(strstr((*fits).head.bunit,"K km/s")!=0)
		(*fits_new).data[n] 
			= w * fabs((*fits).head.cdelt[i1]*(*fits).head.cdelt[i2])*3.0461742e-4 
				*2.413538916e-18/1.0e-26
				/ (fabs((*fits).head.cdelt[sp]));
	else if(strstr((*fits).head.bunit,"JY/BEAM")!=0)
		(*fits_new).data[n] 
			= w * fabs((*fits).head.cdelt[i1]*(*fits).head.cdelt[i2])*3.0461742e-4 
				/ o_beam;
}

}

if(strstr((*fits).head.bunit,"K km/s")!=0){
	fprintf(stderr,"%s -> \'K \'\n",(*fits).head.bunit);	
	strcpy((*fits_new).head.bunit,"\'K \'");
}
else if(strstr((*fits).head.bunit,"JY/BEAM")!=0){
	fprintf(stderr,"%s ->\'JY \'\n",(*fits).head.bunit);	
	strcpy((*fits_new).head.bunit,"\'JY \'");
}

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
int	sp;
char	in_filename[80];
char	out_filename[80];
char	filehead[80];
char	buffer[80];
char	*token;
double	beam_a, beam_b, o_beam;





fprintf(stderr,"Input FITS filename : ");
fscanf(stdin,"%s",buffer);
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
	filehead[strlen(buffer)-5]='\0';
	sprintf(in_filename,"%s",buffer);
	fp_in=fopen(in_filename,"r");
	if(fp_in==NULL){
		fprintf(stderr,"Can't open %s !\n",in_filename);
		exit(1);
	}
}


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

sp=3;
fprintf(stderr,"Free axis (1-%d) : ", fits.head.naxis[0]);
fscanf(stdin,"%d",&sp);

fprintf(stderr,"Beamsize (a,b)[arcsec] : ", fits.head.naxis[0]);
fscanf(stdin,"%s",buffer);
if(strstr(buffer,",")!=0){
	token=strtok(buffer,",");	
	beam_a = atof(token);
	token=strtok(NULL,",");
	beam_b=atof(token);
	o_beam=PI*pow((PI/180.0),2.0)*(beam_a/2.0/3600.0)*(beam_b/2.0/3600.0);
}
else	o_beam=PI*pow((PI/180.0),2.0)*pow((atof(buffer)/2.0/3600.0),2.0);	

fprintf(stderr,"Beam steradian = %e\n",o_beam);


fprintf(stderr,"Calculating integrated intensities.\n");
err_code=Calc_Integrated_Intensity(&fits, &fits_new, sp, o_beam);
if(err_code!=0){
	fprintf(stderr,
	"Error in calculating integrated intensities. Error code=%d\n"
	,err_code);	
	exit(999);
}

sprintf(out_filename,"%s.sp%d.FITS",filehead,sp);
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



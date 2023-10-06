/*
1993/9/1	Tomoharu OKA	:	Coding äJén
1993/9/2	Tomoharu OKA	:	Ver.1.00 äÆê¨
1994/9/7	Tomoharu OKA	:	Bug fix for pixel inner structure
1998/3/3	Tomo OKA	:	FITS intensity calibration
1998/4/30	Tomo OKA	:	FITS intensity clipping
2003/1/29	Tomo OKA	:	FITS coordinate flip
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <profhead.h>
#include <starlib.h>
#include <fitshead.h>

#define	DEBUG	0




main(argc,argv)
int	argc;
char	*argv[];
{
FILE	*fp_in, *fp_out;
int	ii, jj, err_code,flip;
long	pos,posn;
FITS	fits,fits_new;
char	in_filename[80];
char	out_filename[80];
char	filehead[80];
char	buffer[80];
char	*token;
int     pix[MAX_DIMENSION], pixn[MAX_DIMENSION];


/*fp_in=stdin;*/
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

fprintf(stderr,"Reading %s\n",in_filename);
err_code=read_Fits(&fits, fp_in);
if(err_code!=0){
	fprintf(stderr,"Error in reading the FITS header. Error code=%d\n",err_code);
	exit(999);
}
fclose(fp_in);

flip=1;
fprintf(stderr,"Flip coordinate (1-%d): ",fits.head.naxis[0]);
fscanf(stdin,"%d",&flip);

copy_Fits(&fits,&fits_new);
for(jj=1;jj<=fits.head.naxis[0];jj++){
	if(jj==flip){
		fits_new.head.crpix[jj]=fits.head.naxis[jj]-fits.head.crpix[jj]+1.0;
		fits_new.head.cdelt[jj]=(-1.0)*fits.head.cdelt[jj];
	}
}

for(pos=0;pos<fits.head.data_num;pos++){
	POStoPIX(&fits,pos,pix);
	for(jj=0;jj<fits.head.naxis[0];jj++){
		if((jj+1)==flip)	pixn[jj]=(fits.head.naxis[jj+1]-1)-pix[jj];
		else		pixn[jj]=pix[jj];
	}
	fits_new.data[PIXtoPOS(&fits_new,pixn)]=fits.data[PIXtoPOS(&fits,pix)];
}


/*fp_out=stdout;*/
sprintf(out_filename,"%s.f%d.FITS",filehead,flip);
fp_out=fopen(out_filename,"w");
if(fp_out==NULL){
	fprintf(stderr,"Can't open %s !\n",out_filename);
	exit(2);
}

fprintf(stderr,"Writing to %s\n",out_filename);
err_code=output_Fits(&fits_new, fp_out);
if(err_code!=0){
	fprintf(stderr,"Error in outputing the FITS data. Error code=%d\n",err_code);
	exit(999);
}
fclose(fp_out);

}	/*main*/


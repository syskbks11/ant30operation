/*	Rotating FITS axis	

axis1->3
axis2->1
axis3->2

2003/1/9	Tomo OKA	:	Start Coding 
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <profhead.h>
#include <starlib.h>
#include <fitshead.h>

#define	DEBUG	0



int	rot_FitsData(fits, fits_new, mode)
FITS	*fits, *fits_new;
int	mode;
{
int	ii, jj ,n;
double	declination;
int	dim;
int     pix[MAX_DIMENSION], pixn[MAX_DIMENSION];
long	data_num, data_n_num, pos;


dim=(*fits).head.naxis[0];
copy_Fits(fits, fits_new);

if(mode==0){
for(jj=1;jj<=dim-1;jj++){
	(*fits_new).head.naxis[jj]=(*fits).head.naxis[jj+1];
	(*fits_new).head.crval[jj]=(*fits).head.crval[jj+1];
	(*fits_new).head.crpix[jj]=(*fits).head.crpix[jj+1];
	(*fits_new).head.cdelt[jj]=(*fits).head.cdelt[jj+1];
	(*fits_new).head.crota[jj]=(*fits).head.crota[jj+1];
	(*fits_new).head.ctype[jj]=malloc(strlen((*fits).head.ctype[jj+1])+1);
	strcpy((*fits_new).head.ctype[jj],(*fits).head.ctype[jj+1]);	
}
(*fits_new).head.naxis[dim]=(*fits).head.naxis[1];
(*fits_new).head.crval[dim]=(*fits).head.crval[1];
(*fits_new).head.crpix[dim]=(*fits).head.crpix[1];
(*fits_new).head.cdelt[dim]=(*fits).head.cdelt[1];
(*fits_new).head.crota[dim]=(*fits).head.crota[1];
(*fits_new).head.ctype[dim]=malloc(strlen((*fits).head.ctype[1])+1);
strcpy((*fits_new).head.ctype[dim],(*fits).head.ctype[1]);	

data_num=1;
for(jj=1;jj<=dim;jj++){
	data_num*=(*fits).head.naxis[jj];
}
for(ii=0;ii<data_num;ii++){
	POStoPIX(fits,ii,pix);
	for(jj=0;jj<dim-1;jj++){
		pixn[jj]=pix[jj+1];
	}
		pixn[dim-1]=pix[0];
	pos=PIXtoPOS(fits_new,pixn);
	(*fits_new).data[pos]=(*fits).data[ii];
}

}/*if(mode==0)*/

else{
for(jj=1;jj<=dim-1;jj++){
	(*fits_new).head.naxis[jj+1]=(*fits).head.naxis[jj];
	(*fits_new).head.crval[jj+1]=(*fits).head.crval[jj];
	(*fits_new).head.crpix[jj+1]=(*fits).head.crpix[jj];
	(*fits_new).head.cdelt[jj+1]=(*fits).head.cdelt[jj];
	(*fits_new).head.crota[jj+1]=(*fits).head.crota[jj];
	(*fits_new).head.ctype[jj+1]=malloc(strlen((*fits).head.ctype[jj])+1);
	strcpy((*fits_new).head.ctype[jj+1],(*fits).head.ctype[jj]);	
}
(*fits_new).head.naxis[1]=(*fits).head.naxis[dim];
(*fits_new).head.crval[1]=(*fits).head.crval[dim];
(*fits_new).head.crpix[1]=(*fits).head.crpix[dim];
(*fits_new).head.cdelt[1]=(*fits).head.cdelt[dim];
(*fits_new).head.crota[1]=(*fits).head.crota[dim];
(*fits_new).head.ctype[1]=malloc(strlen((*fits).head.ctype[dim])+1);
strcpy((*fits_new).head.ctype[1],(*fits).head.ctype[dim]);	

data_num=1;
for(jj=1;jj<=dim;jj++){
	data_num*=(*fits).head.naxis[jj];
}
for(ii=0;ii<data_num;ii++){
	POStoPIX(fits,ii,pix);
	for(jj=0;jj<dim-1;jj++){
		pixn[jj+1]=pix[jj];
	}
		pixn[0]=pix[dim-1];
	pos=PIXtoPOS(fits_new,pixn);
	(*fits_new).data[pos]=(*fits).data[ii];
}

}/*else*/




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
int		mode;
char		in_filename[80];
char		out_filename[80];
char		filehead[80];
char		buffer[80];
char		*token;
int		i, ra, dec;
double		cosd[MAX_DIMENSION];


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
else{	
	strncpy(filehead,buffer,strlen(buffer)-5);
	filehead[strlen(buffer)-5]='\0';
	sprintf(in_filename,"%s",buffer);
	fp_in=fopen(in_filename,"r");
	if(fp_in==NULL){
		fprintf(stderr,"Can't open %s !\n",in_filename);
		exit(1);
	}
}

fprintf(stderr,"Mode (0:2->1, 1:1->2) : ");
fscanf(stdin,"%d",&mode);
if((mode!=0)&&(mode!=1))	mode=0;

fprintf(stderr,"Reading %s\n",in_filename);
err_code=read_Fits(&fits, fp_in);
if(err_code!=0){
	fprintf(stderr,
	"Error in reading %s. Error code=%d\n"
	,err_code,in_filename);	
	exit(999);
}
fclose(fp_in);



fprintf(stderr,"Rotating axes.\n");
err_code=rot_FitsData(&fits, &fits_new, mode);
if(err_code!=0){
	fprintf(stderr,
	"Error in rotating FITS axes. Error code=%d\n"
	,err_code);	
	exit(999);
}


sprintf(out_filename,"%s.rot.FITS",filehead);
fp_out=fopen(out_filename,"w");
if(fp_out==NULL){
	fprintf(stderr,"Can't open %s !%d\n",out_filename,strlen(filehead));
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



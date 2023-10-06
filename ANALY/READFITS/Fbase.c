/*
1993/9/1	Tomoharu OKA	:	Coding äJén
1993/9/2	Tomoharu OKA	:	Ver.1.00 äÆê¨
1994/9/7	Tomoharu OKA	:	Bug fix for pixel inner structure
1998/3/3	Tomo OKA	:	FITS intensity calibration
1998/4/30	Tomo OKA	:	FITS intensity clipping
2003/1/27	Tomo OKA	:	FITS intensity clipping
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <profhead.h>
#include <starlib.h>
#include <fitshead.h>

#include "../../lib/nucalc.h"

#define	MAX_BASE_POINT	5


#define	DEBUG	0




main(argc,argv)
int	argc;
char	*argv[];
{
FILE		*fp_in, *fp_out;
int		ii, jj, err_code;
FITS		fits;
char		in_filename[80];
char		out_filename[80];
char		filehead[80];
char		buffer[80];
char		*token;
char		command[256];
double     	bx[MAX_BASE_POINT][MAX_DIMENSION];


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

if(fits.head.naxis[0]!=2){
	fprintf(stderr,"FITS must be 2-dimensional.\n");
	exit(101);
}

sprintf(command,"Fxy %s\n",in_filename);
system(command);


for(ii=0;ii<3;ii++){
while(1){fprintf(stderr,"Base position #%d [x,y]: ",ii+1);
	fscanf(stdin,"%s",buffer);
	token=strtok(buffer,",");	
	if(token!=NULL)		bx[ii][0]=atof(token);
	else	continue;
	token=strtok(NULL,",");
	if(token!=NULL){	bx[ii][1]=atof(token);	break;}
	else	continue;
}
}

fprintf(stderr,"Subtracting base plane from %s.\n",in_filename);
err_code=sub_baseplane(&fits,bx);


/*fp_out=stdout;*/
sprintf(out_filename,"%s.base.FITS",filehead);
fp_out=fopen(out_filename,"w");
if(fp_out==NULL){
	fprintf(stderr,"Can't open %s !\n",out_filename);
	exit(2);
}

fprintf(stderr,"Writing to %s\n",out_filename);
err_code=output_Fits(&fits, fp_out);
if(err_code!=0){
	fprintf(stderr,"Error in outputing the FITS data. Error code=%d\n",err_code);
	exit(999);
}
fclose(fp_out);

}	/*main*/






int	sub_baseplane(fits,bx)
FITS	*fits;
double	bx[MAX_BASE_POINT][MAX_DIMENSION];
{
int	ii, jj, err_code;
int	pix[MAX_DIMENSION];
double	a[3][3], b[3], x[3];

for(ii=0;ii<3;ii++){
	fprintf(stderr,"%lf\t%lf\n",bx[ii][0],bx[ii][1]);
}

for(ii=0;ii<3;ii++){
	x[0]=bx[ii][0]; x[1]=bx[ii][1];
	TXtoPIX(fits,x,pix);
	b[ii]=(*fits).data[PIXtoPOS(fits,pix)];
	a[ii][0]=x[0];	a[ii][1]=x[1]; a[ii][2]=1.0;
}
fprintf(stderr,"%lf\t%lf\t%lf\n",b[0],b[1],b[2]);

err_code = sweepout(a,b,3,3,1.0e-10);
fprintf(stderr,"%lf\t%lf\t%lf\terr_code=%d\n",b[0],b[1],b[2],err_code);


for(ii=0;ii<(*fits).head.data_num;ii++){
	POStoPIX(fits,ii,pix);
	PIXtoTX(fits,x,pix);
	(*fits).data[ii] = (*fits).data[ii]-(b[0]*x[0]+b[1]*x[1]+b[2]);
}

return(0);
}




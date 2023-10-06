/*	World coordinate format -> standard FITS	
2003/06/09	Tomo OKA	:	Start Coding 
2003/06/15	Tomo OKA	:	revise 
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <profhead.h>
#include <starlib.h>
#include <fitshead.h>

#define	DEBUG	0
#define	SM_PIX	1
#define	APPND	"w"


int	resample_FitsData(fits, fits_new)
FITS	*fits, *fits_new;
{
int	ii, jj ,n;
int	data_num, data_n_num;
int	pix[MAX_DIMENSION], pixn[MAX_DIMENSION], pixs[MAX_DIMENSION];
double	x[MAX_DIMENSION], min[MAX_DIMENSION], max[MAX_DIMENSION];
double	xx[MAX_DIMENSION], xy[MAX_DIMENSION];
XY_POS	radec, lb;
int	eqx_mode, eqx_mode_r;
double	declination;
double	data, smooth, sigma2, dist2, weight;


for(jj=1;jj<=(*fits).head.naxis[0];jj++){
	if((*fits).head.naxis[jj]<=1)	(*fits).head.naxis[0]--;
}
copy_FitsHeader(fits, fits_new);


pix[0]=0; pix[1]=0;
PIXtoWX(fits,x,pix);
min[0]=x[0]; min[1]=x[1];	max[0]=x[0]; max[1]=x[1]; 

pix[0]=0; pix[1]=(*fits).head.naxis[2]-1;
PIXtoWX(fits,x,pix);	
if(x[0]<min[0]) min[0]=x[0];	if(x[0]>max[0]) max[0]=x[0];
if(x[1]<min[1]) min[1]=x[1];	if(x[1]>max[1]) max[1]=x[1];

pix[0]=(*fits).head.naxis[1]-1; pix[1]=0;
PIXtoWX(fits,x,pix);	
if(x[0]<min[0]) min[0]=x[0];	if(x[0]>max[0]) max[0]=x[0];
if(x[1]<min[1]) min[1]=x[1];	if(x[1]>max[1]) max[1]=x[1];

pix[0]=(*fits).head.naxis[1]-1; pix[1]=(*fits).head.naxis[2]-1;
PIXtoWX(fits,x,pix);	
if(x[0]<min[0]) min[0]=x[0];	if(x[0]>max[0]) max[0]=x[0];
if(x[1]<min[1]) min[1]=x[1];	if(x[1]>max[1]) max[1]=x[1];

for(jj=0;jj<(*fits).head.naxis[0];jj++){
	fprintf(stderr,"Axis#%d:\tmin  = %lf,\tmax = %lf\n",jj,min[jj],max[jj]);
}
fprintf(stderr,"\n");


pix[0]=(*fits).head.crpix[1];	pix[1]=(*fits).head.crpix[2];
PIXtoWX(fits,x,pix);	

(*fits_new).head.cdelt[1]=pow(pow((*fits).head.cd[1][1],2.0)+pow((*fits).head.cd[2][1],2.0),0.5);
(*fits_new).head.cdelt[2]=pow(pow((*fits).head.cd[1][2],2.0)+pow((*fits).head.cd[2][2],2.0),0.5);
(*fits_new).head.naxis[1]=(int)(fabs((max[0]-min[0])/(*fits_new).head.cdelt[1]*cos(x[1]*RAD)));
(*fits_new).head.naxis[2]=(int)(fabs((max[1]-min[1])/(*fits_new).head.cdelt[2]));
(*fits_new).head.crval[1]=(max[0]+min[0])/2.0;
(*fits_new).head.crval[2]=(max[1]+min[1])/2.0;
(*fits_new).head.crpix[1]=(*fits_new).head.naxis[1]/2;
(*fits_new).head.crpix[2]=(*fits_new).head.naxis[2]/2;
sigma2=fabs((*fits_new).head.cdelt[1]*(*fits_new).head.cdelt[2]);
(*fits_new).head.cd_mode=0;

data_num=1.0;
for(jj=1;jj<=(*fits_new).head.naxis[0];jj++){
	data_num=data_num*(*fits_new).head.naxis[jj];
}
(*fits_new).data=calloc(data_num,4);

for(jj=1;jj<=(*fits_new).head.naxis[0];jj++){
	fprintf(stderr,"crval[%d]=%lf,\tcrpix[%d]=%lf\n",jj
		,(*fits_new).head.crval[jj],jj,(*fits_new).head.crpix[jj]);
}
fprintf(stderr,"\n");



if((*fits_new).head.naxis[0]==2){
for(pixn[0]=0;pixn[0]<(*fits_new).head.naxis[1];pixn[0]++){
for(pixn[1]=0;pixn[1]<(*fits_new).head.naxis[2];pixn[1]++){
	PIXtoWX(fits_new,x,pixn); 
	WXtoPIX(fits,x,pix);
/* fprintf(stderr,"%lf,%lf\t%d,%d\n",x[0],x[1],pix[0],pix[1]); */
	if((pix[0]>=0)&&(pix[0]<(*fits).head.naxis[1])&&(pix[1]>=0)&&(pix[1]<(*fits).head.naxis[2])){
		smooth=0.0; weight=0.0;	
		for(pixs[0]=pix[0]-SM_PIX;pixs[0]<=pix[0]+SM_PIX;pixs[0]++){
		for(pixs[1]=pix[1]-SM_PIX;pixs[1]<=pix[1]+SM_PIX;pixs[1]++){
			if((pixs[0]<0)||(pixs[0]>=(*fits).head.naxis[1])||
				(pixs[1]<0)||(pixs[1]>=(*fits).head.naxis[2]))	continue;
			PIXtoWX(fits,xy,pixs);
			dist2 = pow((x[0]-xy[0])*cos(x[1]*RAD),2.0) + pow((x[1]-xy[1]),2.0);
			data = (*fits).data[PIXtoPOS(fits,pixs)];
			if(Blank(data)==(-1))	continue;
			smooth = smooth + exp( (-1.0)*dist2/(sigma2/(4.0*log(2.0))) ) * data;
			weight = weight + exp( (-1.0)*dist2/(sigma2/(4.0*log(2.0))) );
		}}
		if(weight>0.0)	
			(*fits_new).data[PIXtoPOS(fits_new,pixn)] = (float)smooth/weight;
		else	(*fits_new).data[PIXtoPOS(fits_new,pixn)] = (float)DEFAULT_BLANK_VALUE;	
	}
	else	
		(*fits_new).data[PIXtoPOS(fits_new,pixn)] = (float)DEFAULT_BLANK_VALUE;

}}
}	/*	if(naxis[0]==2)	*/

if((*fits_new).head.naxis[0]==3){
for(pixn[0]=0;pixn[0]<(*fits_new).head.naxis[1];pixn[0]++){
fprintf(stderr,"%d/%d\n",pixn[0],(*fits_new).head.naxis[1]);
for(pixn[1]=0;pixn[1]<(*fits_new).head.naxis[2];pixn[1]++){
for(pixn[2]=0;pixn[2]<(*fits_new).head.naxis[3];pixn[2]++){
	PIXtoWX(fits_new,x,pixn); 
	WXtoPIX(fits,x,pix);
	if((pix[0]>=0)&&(pix[0]<(*fits).head.naxis[1])&&(pix[1]>=0)&&(pix[1]<(*fits).head.naxis[2])){
		smooth=0.0; weight=0.0;	
		pixs[2]=pix[2];
		for(pixs[0]=pix[0]-SM_PIX;pixs[0]<=pix[0]+SM_PIX;pixs[0]++){
		for(pixs[1]=pix[1]-SM_PIX;pixs[1]<=pix[1]+SM_PIX;pixs[1]++){
			if((pixs[0]<0)||(pixs[0]>=(*fits).head.naxis[1])||
				(pixs[1]<0)||(pixs[1]>=(*fits).head.naxis[2]))	continue;
			PIXtoWX(fits,xy,pixs);
			dist2 = pow((x[0]-xy[0])*cos(x[1]*RAD),2.0) + pow((x[1]-xy[1]),2.0);
			data = (*fits).data[PIXtoPOS(fits,pixs)];
			if(Blank(data)==(-1))	continue;
			smooth = smooth + exp( (-1.0)*dist2/(sigma2/(4.0*log(2.0))) ) * data;
			weight = weight + exp( (-1.0)*dist2/(sigma2/(4.0*log(2.0))) );
		}}
		if(weight>0.0)	
			(*fits_new).data[PIXtoPOS(fits_new,pixn)] = (float)smooth/weight;
		else	(*fits_new).data[PIXtoPOS(fits_new,pixn)] = (float)DEFAULT_BLANK_VALUE;	
	}
	else	
		(*fits_new).data[PIXtoPOS(fits_new,pixn)] = (float)DEFAULT_BLANK_VALUE;

}}}
}	/*	if(naxis[0]==3)	*/





return(0);

}





main(argc,argv)
int	argc;
char	*argv[];
{
FILE	*fp_in, *fp_out;
int	err_code;
FITS	fits, fits_new;
int	i, ii, jj;
RANGE	range[MAX_DIMENSION];
char	in_filename[80], out_filename[80];
char	filehead[80], buffer[80], *token;



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
	fprintf(stderr,
	"Error in reading %s. Error code=%d\n"
	,err_code,in_filename);	
	exit(999);
}
fclose(fp_in);

/*if(fits.head.naxis[0]!=2){
	fprintf(stderr,"Input FITS must be two dimensional.\n");
	exit(1);
}*/


fprintf(stderr,"Transforming the data.\n");
err_code=resample_FitsData(&fits, &fits_new);
if(err_code!=0){
	fprintf(stderr,
	"Error in reforming the FITS data. Error code=%d\n"
	,err_code);	
	exit(999);
}





sprintf(out_filename,"%s.%s.FITS",filehead,APPND);
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



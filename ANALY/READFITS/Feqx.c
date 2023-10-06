/*	Changing the Equinox (B1950/J2000)	
1998/3/1	Tomo OKA	:	Start Coding 
1998/3/2	Tomo OKA	:	Ver.1.00
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


int	equinox_FitsData(fits, fits_new, eqnx)
FITS	*fits, *fits_new;
double	eqnx;
{
int	ii, jj ,n;
int	data_num, data_n_num;
int	pix[MAX_DIMENSION], pixn[MAX_DIMENSION], pixs[MAX_DIMENSION];
double	x[MAX_DIMENSION], min[MAX_DIMENSION], max[MAX_DIMENSION];
double	xx[MAX_DIMENSION], xy[MAX_DIMENSION];
double	corner[4][MAX_DIMENSION];
XY_POS	radec, lb;
int	eqx_mode, eqx_mode_r;
double	declination;
double	data, smooth, sigma2, dist2, weight;


copy_FitsHeader(fits, fits_new);
(*fits_new).head.epoch=eqnx;
if(eqnx==2000.0){eqx_mode=0; eqx_mode_r=1;}
else if(eqnx==1950.0){eqx_mode=1; eqx_mode_r=0;}
else	return(1);


pix[0]=0; pix[1]=0;
PIXtoTX(fits,x,pix);	radec.x=x[0];	radec.y=x[1];
equinox(&radec,eqx_mode);
corner[0][0]=radec.x; corner[0][1]=radec.y;	

pix[0]=0; pix[1]=(*fits).head.naxis[2]-1;
PIXtoTX(fits,x,pix);	radec.x=x[0];	radec.y=x[1];
equinox(&radec,eqx_mode);
corner[1][0]=radec.x; corner[1][1]=radec.y;	

pix[0]=(*fits).head.naxis[1]-1; pix[1]=0;
PIXtoTX(fits,x,pix);	radec.x=x[0];	radec.y=x[1];
equinox(&radec,eqx_mode);
corner[2][0]=radec.x; corner[2][1]=radec.y;	

pix[0]=(*fits).head.naxis[1]-1; pix[1]=(*fits).head.naxis[2]-1;
PIXtoTX(fits,x,pix);	radec.x=x[0];	radec.y=x[1];
equinox(&radec,eqx_mode);
corner[3][0]=radec.x; corner[3][1]=radec.y;	

min[0]=corner[0][0];	min[1]=corner[0][1];
max[0]=corner[0][0];	max[1]=corner[0][1];
for(ii=1;ii<4;ii++){
	if(corner[ii][0]<min[0])	min[0]=corner[ii][0];
	if(corner[ii][0]>max[0])	max[0]=corner[ii][0];
	if(corner[ii][1]<min[1])	min[1]=corner[ii][1];
	if(corner[ii][1]>max[1])	max[1]=corner[ii][1];
}

if(fabs(max[0]-min[0])>270.0){
if(corner[0][0]>270.0)		corner[0][0] -= 360.0;
min[0]=corner[0][0];	
max[0]=corner[0][0];
for(ii=1;ii<4;ii++){
	if(corner[ii][0]>270.0)		corner[ii][0] -= 360.0;
	if(corner[ii][0]<min[0])	min[0]=corner[ii][0];
	if(corner[ii][0]>max[0])	max[0]=corner[ii][0];
}
}


for(jj=0;jj<(*fits).head.naxis[0];jj++){
	fprintf(stderr,"Axis#%d:\tmin  = %lf,\tmax = %lf\n",jj,min[jj],max[jj]);
}
fprintf(stderr,"\n");


pix[0]=(*fits).head.crpix[1];	pix[1]=(*fits).head.crpix[2];
PIXtoTX(fits,x,pix);	radec.x=x[0];	radec.y=x[1];
equinox(&radec,eqx_mode);
(*fits_new).head.naxis[1]=(int)(fabs((max[0]-min[0])/(*fits).head.cdelt[1]*cos(radec.y*RAD)));
(*fits_new).head.naxis[2]=(int)(fabs((max[1]-min[1])/(*fits).head.cdelt[2]));
(*fits_new).head.crval[1]=(max[0]+min[0])/2.0;
(*fits_new).head.crval[2]=(max[1]+min[1])/2.0;
(*fits_new).head.crpix[1]=(*fits_new).head.naxis[1]/2;
(*fits_new).head.crpix[2]=(*fits_new).head.naxis[2]/2;
(*fits_new).head.cdelt[1]=(*fits).head.cdelt[1];
(*fits_new).head.cdelt[2]=(*fits).head.cdelt[2];

if((*fits).head.naxis[0]==3){
	(*fits_new).head.naxis[3]=(*fits).head.naxis[3];
	(*fits_new).head.crval[3]=(*fits).head.crval[3];
	(*fits_new).head.crpix[3]=(*fits).head.crpix[3];
	(*fits_new).head.cdelt[3]=(*fits).head.cdelt[3];
	strcpy((*fits_new).head.ctype[3],(*fits).head.ctype[3]);
}


sigma2=fabs((*fits).head.cdelt[1]*(*fits).head.cdelt[2]);

data_num=(*fits_new).head.naxis[1];
for(jj=2;jj<=(*fits_new).head.naxis[0];jj++){
	data_num=data_num*(*fits_new).head.naxis[jj];
}
(*fits_new).data=calloc(data_num,4);

for(jj=1;jj<=(*fits_new).head.naxis[0];jj++){
	fprintf(stderr,"crval[%d]=%lf,\tcrpix[%d]=%lf,\tcdelt[%d]=%lf\n",jj
		,(*fits_new).head.crval[jj],jj,(*fits_new).head.crpix[jj]
		,jj,(*fits_new).head.cdelt[jj]);
}
fprintf(stderr,"\n");



if((*fits_new).head.naxis[0]==2){
for(pixn[0]=0;pixn[0]<(*fits_new).head.naxis[1];pixn[0]++){
for(pixn[1]=0;pixn[1]<(*fits_new).head.naxis[2];pixn[1]++){
	PIXtoTX(fits_new,x,pixn); radec.x=x[0];	radec.y=x[1];
	equinox(&radec,eqx_mode_r); x[0]=radec.x; x[1]=radec.y;	
	TXtoPIX(fits,x,pix);
	if((pix[0]>=0)&&(pix[0]<(*fits).head.naxis[1])&&(pix[1]>=0)&&(pix[1]<(*fits).head.naxis[2])){
		smooth=0.0; weight=0.0;	
		for(pixs[0]=pix[0]-SM_PIX;pixs[0]<=pix[0]+SM_PIX;pixs[0]++){
		for(pixs[1]=pix[1]-SM_PIX;pixs[1]<=pix[1]+SM_PIX;pixs[1]++){
			if((pixs[0]<0)||(pixs[0]>=(*fits).head.naxis[1])||
				(pixs[1]<0)||(pixs[1]>=(*fits).head.naxis[2]))	continue;
			PIXtoTX(fits,xy,pixs);
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
for(pixn[1]=0;pixn[1]<(*fits_new).head.naxis[2];pixn[1]++){
for(pixn[2]=0;pixn[2]<(*fits_new).head.naxis[3];pixn[2]++){
	PIXtoTX(fits_new,x,pixn); radec.x=x[0];	radec.y=x[1];
	equinox(&radec,eqx_mode_r); x[0]=radec.x; x[1]=radec.y;	
	TXtoPIX(fits,x,pix);
	if((pix[0]>=0)&&(pix[0]<(*fits).head.naxis[1])&&(pix[1]>=0)&&(pix[1]<(*fits).head.naxis[2])){
		smooth=0.0; weight=0.0;	
		pixs[2]=pix[2];
		for(pixs[0]=pix[0]-SM_PIX;pixs[0]<=pix[0]+SM_PIX;pixs[0]++){
		for(pixs[1]=pix[1]-SM_PIX;pixs[1]<=pix[1]+SM_PIX;pixs[1]++){
			if((pixs[0]<0)||(pixs[0]>=(*fits).head.naxis[1])||
				(pixs[1]<0)||(pixs[1]>=(*fits).head.naxis[2]))	continue;
			PIXtoTX(fits,xy,pixs);
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
double	eqnx;



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


fprintf(stderr,"Target Equinox (B1950/J2000): ");
fscanf(stdin,"%s",buffer);
if((strstr(buffer,"1950")!=NULL)&&(fits.head.epoch==2000.0)){
	eqnx=1950.0;
}
else if((strstr(buffer,"2000")!=NULL)&&(fits.head.epoch==1950.0)){
	eqnx=2000.0;
}
else{
	fprintf(stderr,"Input FITS is already in equinox=%4.0lf.\n",fits.head.epoch);
	exit(2);
}



fprintf(stderr,"Transforming the data.\n");
err_code=equinox_FitsData(&fits, &fits_new, eqnx);
if(err_code!=0){
	fprintf(stderr,
	"Error in reforming the FITS data. Error code=%d\n"
	,err_code);	
	exit(999);
}





sprintf(out_filename,"%s.%4.0lf.FITS",filehead,fits_new.head.epoch);
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



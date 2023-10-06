/*	Cutting out FITS DATA	
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

int	me;


int	cut_FitsData(fits, fits_new, range)
FITS	*fits, *fits_new;
RANGE	range[];
{
int	ii, jj ,n;
int	data_num, data_n_num;
int	pix[MAX_DIMENSION], pixn[MAX_DIMENSION];
int	ra, dec;
int	min[MAX_DIMENSION], max[MAX_DIMENSION];
double	xmin[MAX_DIMENSION], xmax[MAX_DIMENSION], tmp;


for(jj=1;jj<=(*fits).head.naxis[0];jj++){
	if(((strstr((*fits).head.ctype[jj],"RA")!=NULL)
	||(strstr((*fits).head.ctype[jj],"GLON")!=NULL))
	&&((*fits).head.crval[jj]<(-90.0)))	(*fits).head.crval[jj]+=360.0;	
}
copy_FitsHeader(fits, fits_new);

for(ii=1;ii<=(*fits).head.naxis[0];ii++){
	xmin[ii-1]=range[ii-1].min;	xmax[ii-1]=range[ii-1].max;
}
if(me==1){
	XtoPIX(fits,xmin,min);
	XtoPIX(fits,xmax,max);
}
else{
	TXtoPIX(fits,xmin,min);
	TXtoPIX(fits,xmax,max);
}
for(ii=1;ii<=(*fits).head.naxis[0];ii++){
	if(min[ii-1]>max[ii-1]){tmp=min[ii-1]; min[ii-1]=max[ii-1]; max[ii-1]=tmp;}
}


for(ii=1;ii<=(*fits).head.naxis[0];ii++){
	fprintf(stderr,"Range #%d: (%lf,%lf)=",ii,range[ii-1].min,range[ii-1].max);
	fprintf(stderr,"(%d,%d)\n",min[ii-1],max[ii-1]);
}

for(ii=1;ii<=(*fits).head.naxis[0];ii++){
	(*fits_new).head.naxis[ii]=(max[ii-1]-min[ii-1])+1;
	(*fits_new).head.cdelt[ii]=(*fits).head.cdelt[ii];
	(*fits_new).head.crval[ii]=(*fits).head.crval[ii];
	(*fits_new).head.crpix[ii]=(*fits).head.crpix[ii]-(double)(min[ii-1]);
}

data_num=1;
data_n_num=1;
for(jj=1;jj<=(*fits).head.naxis[0];jj++){
	data_num=data_num*(*fits).head.naxis[jj];
	data_n_num=data_n_num*(*fits_new).head.naxis[jj];
}

(*fits_new).data=calloc(data_n_num,4);


if((*fits_new).head.naxis[0]==3){
for(pixn[0]=0;pixn[0]<(*fits_new).head.naxis[1];pixn[0]++){
for(pixn[1]=0;pixn[1]<(*fits_new).head.naxis[2];pixn[1]++){
for(pixn[2]=0;pixn[2]<(*fits_new).head.naxis[3];pixn[2]++){
	pix[0]=pixn[0]+(min[0]);
	pix[1]=pixn[1]+(min[1]);
	pix[2]=pixn[2]+(min[2]);
	if( (pix[0]>=0)&&(pix[1]>=0)&&(pix[2]>=0)&&
		(pix[0]<(*fits).head.naxis[1])&&(pix[1]<(*fits).head.naxis[2])&&(pix[2]<(*fits).head.naxis[3]) )	
		(*fits_new).data[DataPosition(fits_new,pixn)] = (*fits).data[DataPosition(fits,pix)];
	else	(*fits_new).data[DataPosition(fits_new,pixn)] = DEFAULT_BLANK_VALUE;
}
}
}
}	/*if	*/

else if((*fits_new).head.naxis[0]==2){
for(pixn[0]=0;pixn[0]<(*fits_new).head.naxis[1];pixn[0]++){
for(pixn[1]=0;pixn[1]<(*fits_new).head.naxis[2];pixn[1]++){
	pix[0]=pixn[0]+(min[0]);
	pix[1]=pixn[1]+(min[1]);
	pix[2]=0;
	if( (pix[0]>=0)&&(pix[1]>=0)&&
		(pix[0]<(*fits).head.naxis[1])&&(pix[1]<(*fits).head.naxis[2]))	
		(*fits_new).data[DataPosition(fits_new,pixn)] = (*fits).data[DataPosition(fits,pix)];
	else	(*fits_new).data[DataPosition(fits_new,pixn)] = DEFAULT_BLANK_VALUE;
/*	fprintf(stderr,"(%d, %d):%f\n",pix[0],pix[1],(*fits_new).data[DataPosition(fits_new,pixn)]);	*/
}
}
}	/*else if	*/



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
RANGE		range[MAX_DIMENSION];
int		cc;
char		in_filename[80];
char		out_filename[80];
char		filehead[80];
char		buffer[80];
char		*token;
int	min[MAX_DIMENSION], max[MAX_DIMENSION];
double	xmin[MAX_DIMENSION], xmax[MAX_DIMENSION], tmp;


me=0;
if(argc>=2){
	if(strcmp(argv[1],"-me"))	me=1;	/*Mercator*/
}

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


fprintf(stderr,"Reading %s\n",in_filename);
err_code=read_Fits(&fits, fp_in);
if(err_code!=0){
	fprintf(stderr,
	"Error in reading %s. Error code=%d\n"
	,err_code,in_filename);	
	exit(999);
}
fclose(fp_in);


for(ii=0;ii<fits.head.naxis[0];ii++){
	min[ii]=0;	max[ii]=fits.head.naxis[ii+1]-1;	
}
PIXtoTX(&fits,xmin,min);
PIXtoTX(&fits,xmax,max);
for(ii=0;ii<fits.head.naxis[0];ii++){
	range[ii].min=xmin[ii];
	range[ii].max=xmax[ii];
}

for(ii=0;ii<fits.head.naxis[0];ii++){
fprintf(stderr,"Range #%d (%lf,%lf): ",ii+1,range[ii].min,range[ii].max);
	fscanf(stdin,"%s",buffer);
	token=strtok(buffer,",");	
	if(token!=NULL)	range[ii].min=atof(token);
	else	continue;
	token=strtok(NULL,",");		
	if(token!=NULL)	range[ii].max=atof(token);
	else	continue;
}



fprintf(stderr,"Cutting the data.\n");
err_code=cut_FitsData(&fits, &fits_new, range);
if(err_code!=0){
	fprintf(stderr,
	"Error in cutting the FITS data. Error code=%d\n"
	,err_code);	
	exit(999);
}


sprintf(out_filename,"%s.cut.FITS",filehead);
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



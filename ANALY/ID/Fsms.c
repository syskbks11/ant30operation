/*	Smoothing FITS DATA	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <profhead.h>
#include <starlib.h>
#include <fitshead.h>

#define	DEBUG	0
#define	RESTRICT	3.0
#define	GRID		4.0





int	Gaussian_smooth(fits,wx,smoothed_data,weight)
FITS	*fits;
float	wx[];
float	*smoothed_data, *weight;
{
int	i,j,k,ii,jj,kk, cc;
int	min[MAX_DIMENSION], max[MAX_DIMENSION];
int	pix[MAX_DIMENSION];
int	pixr[MAX_DIMENSION];
long	pos, posr;


if(((*fits).head.naxis[0]!=2)&&((*fits).head.naxis[0]!=3))	return(-1);

if((*fits).head.naxis[0]==3){
for(pix[2]=0;pix[2]<(*fits).head.naxis[3];pix[2]++){
for(pix[1]=0;pix[1]<(*fits).head.naxis[2];pix[1]++){
for(pix[0]=0;pix[0]<(*fits).head.naxis[1];pix[0]++){
	pos=DataPosition(fits,pix);
	for(cc=0;cc<(*fits).head.naxis[0];cc++){
		min[cc]=pix[cc]-(long)(RESTRICT/2.0*wx[cc]); 	
		if(min[cc]<0) min[cc]=0;
		max[cc]=pix[cc]+(long)(RESTRICT/2.0*wx[cc]);	
		if(max[cc]>=(*fits).head.naxis[cc+1]) max[cc]=(*fits).head.naxis[cc+1]-1;
	}
	for(pixr[2]=min[2];pixr[2]<=max[2];pixr[2]++){
	for(pixr[1]=min[1];pixr[1]<=max[1];pixr[1]++){
	for(pixr[0]=min[0];pixr[0]<=max[0];pixr[0]++){
		{float	dist[MAX_DIMENSION];
		float	Exp;
		Exp=1.0;
		for(i=0;i<(*fits).head.naxis[0];i++){	
			dist[i]=fabs((float)(pix[i]-pixr[i]));
			Exp = Exp * exp(-pow((dist[i]/wx[i]),2.0));
		}
		posr=DataPosition(fits,pixr);
		smoothed_data[pos] = smoothed_data[pos] + (*fits).data[posr] * Exp;
		weight[pos] = weight[pos] + Exp;
		}
	}	
	}
	}	
}
}
}
}/*	if((*fits).head.naxis[0]==3)	*/


if((*fits).head.naxis[0]==2){
for(pix[1]=0;pix[1]<(*fits).head.naxis[2];pix[1]++){
for(pix[0]=0;pix[0]<(*fits).head.naxis[1];pix[0]++){
	pos=DataPosition(fits,pix);
	for(cc=0;cc<(*fits).head.naxis[0];cc++){
		min[cc]=pix[cc]-(long)(RESTRICT/2.0*wx[cc]); 	
		if(min[cc]<0) min[cc]=0;
		max[cc]=pix[cc]+(long)(RESTRICT/2.0*wx[cc]);	
		if(max[cc]>=(*fits).head.naxis[cc+1]) max[cc]=(*fits).head.naxis[cc+1]-1;
	}
	for(pixr[1]=min[1];pixr[1]<=max[1];pixr[1]++){
	for(pixr[0]=min[0];pixr[0]<=max[0];pixr[0]++){
		{float	dist[MAX_DIMENSION];
		float	Exp;
		Exp=1.0;
		for(i=0;i<(*fits).head.naxis[0];i++){	
			dist[i]=fabs((float)(pix[i]-pixr[i]));
			Exp = Exp * exp(-pow((dist[i]/wx[i]),2.0));
		}
		posr=DataPosition(fits,pixr);
		smoothed_data[pos] = smoothed_data[pos] + (*fits).data[posr] * Exp;
		weight[pos] = weight[pos] + Exp;
		}
	}	
	}
}
}

}/*	if((*fits).head.naxis[0]==2)	*/



return(0);

}


int	smooth_FitsData(fits, w)
FITS	*fits;
double	w[];
{
int	i, j, k, ii, jj;
int	dim, data_num;
float	wx[MAX_DIMENSION];
int	data_num1;
int	err_code;
float	*smoothed_data, *weight;


dim = (*fits).head.naxis[0];

data_num1=1;
for(i=0;i<dim;i++){
	wx[i]=(float)ceil(w[i]/fabs((*fits).head.cdelt[i+1]));
	data_num1=data_num1*(long)wx[i];
}


data_num=1;
for(jj=1;jj<=(*fits).head.naxis[0];jj++){
	data_num=data_num*(*fits).head.naxis[jj];
}

/*	Gaussian smoothing	*/

smoothed_data = (float *)calloc(data_num,4);
weight = (float *)calloc(data_num,4);
for(ii=0;ii<data_num;ii++){	
	smoothed_data[ii]=0.0;
	weight[ii]=0.0;
}

err_code=Gaussian_smooth(fits,wx,smoothed_data,weight);
if(err_code!=0)	return(-1);

for(ii=0;ii<data_num;ii++){	
	(*fits).data[ii]=smoothed_data[ii]/weight[ii];
}
free(smoothed_data);
free(weight);


return(0);
}







main(argc,argv)
int	argc;
char	*argv[];
{
FILE		*fp_in, *fp_out;
int		err_code;
static FITS	fits;
int		ii, jj;
double		w[MAX_DIMENSION];
int		dim;
char		in_filename[80];
char		out_filename[80];
char		filehead[80];
char		buffer[80];
char		*token;




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
	"Error in reading the FITS file. Error code=%d\n"
	,err_code);	
	exit(999);
}
fclose(fp_in);


fprintf(stderr,"Smoothing widths (");
for(ii=1;ii<fits.head.naxis[0];ii++){
fprintf(stderr,"%lf,",fabs(fits.head.cdelt[ii]));
}
fprintf(stderr,"%lf): ",fabs(fits.head.cdelt[fits.head.naxis[0]]));
fscanf(stdin,"%s",buffer);	
token=strtok(buffer,", \t");
for(jj=0;jj<=fits.head.naxis[0];jj++){
	if(token==NULL){
		for(ii=jj;ii<fits.head.naxis[0];ii++){w[ii]=fabs(fits.head.cdelt[ii+1])/GRID; }
		break;
	}
	else	w[jj]=atof(token);
	token=strtok(NULL,", \t");	
}

/*fp_out=stdout;*/
sprintf(out_filename,"%s.sm.FITS",filehead);
fp_out=fopen(out_filename,"w");
if(fp_out==NULL){
	fprintf(stderr,"Can't open %s !\n",out_filename);
	exit(1);
}



fprintf(stderr,"Smoothing the data.\n");
err_code=smooth_FitsData(&fits, w);
if(err_code!=0){
	fprintf(stderr,
	"Error in smoothing the FITS data. Error code=%d\n"
	,err_code);	
	exit(999);
}

fprintf(stderr,"Writing to %s\n",out_filename);
err_code=output_Fits(&fits, fp_out);
if(err_code!=0){
	fprintf(stderr,
	"Error in writing a FITS file. Error code=%d\n"
	,err_code);	
	exit(999);
}
fclose(fp_out);


}	/*main*/



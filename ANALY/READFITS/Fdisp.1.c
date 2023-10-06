/*	Caluculate dispersions weighted by the intensity	
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







int	calc_dispersions(fits, range, bound, disp, cent, peak, integ, peak_val)
FITS	*fits;
RANGE	range[];
double	disp[], cent[], peak[];
double	bound, *integ, *peak_val;
{
int	ii, jj ,n;
int	data_num, data_n_num;
int	pix[MAX_DIMENSION];
int	min[MAX_DIMENSION], max[MAX_DIMENSION];
double	declination;
int	ra, dec;
double	x[MAX_DIMENSION], xx[MAX_DIMENSION], tx[MAX_DIMENSION], x2[MAX_DIMENSION], w;
long	ch;
float	data_max;


for(jj=1;jj<=(*fits).head.naxis[0];jj++){
	if((*fits).head.naxis[jj]<=1)	(*fits).head.naxis[0]--;	
}
if(bound<=0.0)	bound=(*fits).head.datamin;


ra=(-1);	dec=(-1);
for(ii=1;ii<=(*fits).head.naxis[0];ii++){
	if((strstr((*fits).head.ctype[ii],"RA")!=NULL)||
	(strstr((*fits).head.ctype[ii],"GLON")!=NULL))	ra=ii;
	if((strstr((*fits).head.ctype[ii],"DEC")!=NULL)||
	(strstr((*fits).head.ctype[ii],"GLAT")!=NULL))	dec=ii;
}
if(dec==(-1))	declination=0.0;
else		declination=(*fits).head.crval[dec];

for(ii=1;ii<=(*fits).head.naxis[0];ii++){
	if(ii==ra){
	min[ii-1]=tint((double)(*fits).head.crpix[ii] + 
		((double)range[ii-1].min-(*fits).head.crval[ii])/((*fits).head.cdelt[ii]/cos(declination*PI/180.0)));
	max[ii-1]=tint((double)(*fits).head.crpix[ii] + 
		((double)range[ii-1].max-(*fits).head.crval[ii])/((*fits).head.cdelt[ii]/cos(declination*PI/180.0)));
	}
	else{
	min[ii-1]=tint((double)(*fits).head.crpix[ii] + 
		((double)range[ii-1].min-(*fits).head.crval[ii])/(*fits).head.cdelt[ii]);
	max[ii-1]=tint((double)(*fits).head.crpix[ii] + 
		((double)range[ii-1].max-(*fits).head.crval[ii])/(*fits).head.cdelt[ii]);
	}
	{int	tmp;
	if(min[ii-1]>max[ii-1]){tmp=min[ii-1]; min[ii-1]=max[ii-1]; max[ii-1]=tmp;}
	}
}

for(ii=1;ii<=(*fits).head.naxis[0];ii++){
	fprintf(stderr,"Range #%d: (%lf,%lf)=",ii,range[ii-1].min,range[ii-1].max);
	fprintf(stderr,"(%d,%d)\n",min[ii-1],max[ii-1]);
}


for(ii=0;ii<(*fits).head.naxis[0];ii++){
	x[ii]=0.0; x2[ii]=0.0;
}	w=0.0;

data_num=0; data_max=(float)(*fits).head.datamin;
if((*fits).head.naxis[0]==3){
for(pix[0]=min[0]-1;pix[0]<max[0];pix[0]++){
for(pix[1]=min[1]-1;pix[1]<max[1];pix[1]++){
for(pix[2]=min[2]-1;pix[2]<max[2];pix[2]++){
	ch=PIXtoPOS(fits,pix);
	PIXtoX(fits,xx,pix);
	if((Blank((*fits).data[ch])==0)&&((*fits).data[ch]>=bound)){
	for(ii=0;ii<(*fits).head.naxis[0];ii++){
		x[ii]=x[ii]+(double)(*fits).data[ch]*xx[ii];
		x2[ii]=x2[ii]+(double)(*fits).data[ch]*pow(xx[ii],2.0);
	}
	data_num++;
	if((*fits).data[ch]>data_max){
		data_max=(*fits).data[ch]; 
		PIXtoTX(fits,tx,pix);
		for(ii=0;ii<(*fits).head.naxis[0];ii++){peak[ii]=tx[ii];}
	}
	w=w+(double)(*fits).data[ch];}
}
}
}
for(ii=0;ii<(*fits).head.naxis[0];ii++){
	cent[ii]=x[ii]/w;
	disp[ii]=0.0;
}
for(pix[0]=min[0]-1;pix[0]<max[0];pix[0]++){
for(pix[1]=min[1]-1;pix[1]<max[1];pix[1]++){
for(pix[2]=min[2]-1;pix[2]<max[2];pix[2]++){
	ch=PIXtoPOS(fits,pix);
	PIXtoX(fits,xx,pix);
	if((Blank((*fits).data[ch])==0)&&((*fits).data[ch]>=bound)){
	for(ii=0;ii<(*fits).head.naxis[0];ii++){
		disp[ii]=disp[ii]+(double)(*fits).data[ch]*pow((xx[ii]-cent[ii]),2.0);
	}}
}
}
}
for(ii=0;ii<(*fits).head.naxis[0];ii++){
	disp[ii]=pow((disp[ii]/w),0.5);
}
}	/*if	*/



else if((*fits).head.naxis[0]==2){
for(pix[0]=min[0]-1;pix[0]<max[0];pix[0]++){
for(pix[1]=min[1]-1;pix[1]<max[1];pix[1]++){
	ch=PIXtoPOS(fits,pix);
	PIXtoX(fits,xx,pix);
	if(Blank((*fits).data[ch])==0){
	for(ii=0;ii<(*fits).head.naxis[0];ii++){
		x[ii]=x[ii]+(double)(*fits).data[ch]*xx[ii];
		x2[ii]=x2[ii]+(double)(*fits).data[ch]*pow(xx[ii],2.0);
	}
	data_num++;
	if((*fits).data[ch]>data_max){
		data_max=(*fits).data[ch]; 
		PIXtoTX(fits,tx,pix);
		for(ii=0;ii<(*fits).head.naxis[0];ii++){peak[ii]=tx[ii];}
	}
	w=w+(double)(*fits).data[ch];}
}
}
for(ii=0;ii<(*fits).head.naxis[0];ii++){
	cent[ii]=x[ii]/w;
	disp[ii]=0.0;
}
for(pix[0]=min[0]-1;pix[0]<max[0];pix[0]++){
for(pix[1]=min[1]-1;pix[1]<max[1];pix[1]++){
	ch=PIXtoPOS(fits,pix);
	PIXtoX(fits,xx,pix);
	if((Blank((*fits).data[ch])==0)&&((*fits).data[ch]>=bound)){
	for(ii=0;ii<(*fits).head.naxis[0];ii++){
		disp[ii]=disp[ii]+(double)(*fits).data[ch]*pow((xx[ii]-cent[ii]),2.0);
	}}
}
}
for(ii=0;ii<(*fits).head.naxis[0];ii++){
	disp[ii]=pow((disp[ii]/w),0.5);
}
}	/*else if	*/


fprintf(stderr,"data_num=%d\n",data_num);


*integ = w;
*peak_val=data_max;

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
int		i, ra, dec;
double		cosd[MAX_DIMENSION];
double		disp[MAX_DIMENSION];
double		peak[MAX_DIMENSION];
double		cent[MAX_DIMENSION];
double		bound, integ, peak_val;



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
	"Error in reading %s. Error code=%d\n"
	,err_code,in_filename);	
	exit(999);
}
fclose(fp_in);


fprintf(stderr,"Setting the ranges.\n");
ra=1; dec=2;
for(i=1;i<=fits.head.naxis[0];i++){
	if((strstr(fits.head.ctype[i],"RA")!=NULL)||
	(strstr(fits.head.ctype[i],"GLON")!=NULL))	ra=i;
	if((strstr(fits.head.ctype[i],"DEC")!=NULL)||
	(strstr(fits.head.ctype[i],"GLAT")!=NULL))	dec=i;
}

{double	min, max, declination;
/*min=fits.head.crval[dec] + fits.head.cdelt[dec]*(1.0-fits.head.crpix[dec]);
max=fits.head.crval[dec] + fits.head.cdelt[dec]*((double)fits.head.naxis[dec]-fits.head.crpix[dec]);
declination = (min+max)/2.0;*/
declination = fits.head.crval[dec];
for(i=1;i<=fits.head.naxis[0];i++){
	if(i==ra)	cosd[i]=cos(declination*PI/180.0);
	else		cosd[i]=1.0;
}}


for(ii=0;ii<fits.head.naxis[0];ii++){
range[ii].min=fits.head.crval[ii+1]+fits.head.cdelt[ii+1]/cosd[ii+1]*(1.0-fits.head.crpix[ii+1]);
range[ii].max=fits.head.crval[ii+1]+fits.head.cdelt[ii+1]/cosd[ii+1]*(fits.head.naxis[ii+1]-fits.head.crpix[ii+1]);
fprintf(stderr,"Range #%d (%lf,%lf): ",ii+1,range[ii].min,range[ii].max);
	fscanf(stdin,"%s",buffer);
	token=strtok(buffer,",");	
	if(token!=NULL)	range[ii].min=atof(token);
	else	continue;
	token=strtok(NULL,",");		
	if(token!=NULL)	range[ii].max=atof(token);
	else	continue;
}

fprintf(stderr,"Boundary (<=0: use full ranges)= ");
fscanf(stdin,"%lf",&bound);


fprintf(stderr,"Calculating the dispersions.\n");
err_code=calc_dispersions(&fits, range, bound, disp, cent, peak, &integ, &peak_val);
if(err_code!=0){
	fprintf(stderr,
	"Error in calculating dispersions. Error code=%d\n"
	,err_code);	
	exit(999);
}



for(ii=0;ii<fits.head.naxis[0];ii++){
	fprintf(stdout,"#%d: Mean = %lf\tPeak = %lf\tDisp = %lf\n",ii+1,cent[ii],peak[ii],disp[ii]);
}
fprintf(stdout,"Peak Value = %lf\n",peak_val);
fprintf(stdout,"Integ = %lf\n",integ);

}	/*main*/



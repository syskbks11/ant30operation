/*	Velocity Smoothing FITS DATA 	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <fitshead.h>




int	smooth_FitsData(fits, fits_new, g, w)
FITS	*fits, *fits_new;
double	g, w;
{
int	i, j, k, ii, jj, kk;
int	data_num, data_n_num;
double	min, max, tmp;
int	pix[MAX_DIMENSION], pixr[MAX_DIMENSION];
double	x[MAX_DIMENSION], xr[MAX_DIMENSION];
double	sum, wgt;
long	pos, posr;


copy_FitsHeader(fits, fits_new);

i=(*fits).head.naxis[0];
if(strstr((*fits).head.ctype[i],"VEL")==NULL){
    fprintf(stderr,"The last axis must be velocity!¥n");
    exit(111);
}


min=(*fits).head.crval[i]+(1.0-(*fits).head.crpix[i])*(*fits).head.cdelt[i];
max=(*fits).head.crval[i]+((double)(*fits).head.naxis[i]-(*fits).head.crpix[i])*(*fits).head.cdelt[i];
if(max<min){tmp=max;max=min;min=tmp;}

(*fits_new).head.naxis[i]=tint((max-min)/g)+1;
(*fits_new).head.crval[i]=floor((min+max)/2.0);
(*fits_new).head.crpix[i]=(double)tint((*fits_new).head.naxis[i]/2.0);
(*fits_new).head.cdelt[i]=g;
(*fits_new).head.crota[i]=0.0;

data_num=1;
data_n_num=1;
for(jj=1;jj<=(*fits).head.naxis[0];jj++){
	data_num=data_num*(*fits).head.naxis[jj];
	data_n_num=data_n_num*(*fits_new).head.naxis[jj];
}
(*fits_new).data=(float *)calloc(data_n_num,sizeof((*fits_new).data));


/*	Gaussian smoothing	*/
if((*fits).head.naxis[0]==3){		/*	3-d case	*/
for(pix[0]=0;pix[0]<(*fits_new).head.naxis[1];pix[0]++){
fprintf(stderr,"%d\n",pix[0]);
for(pix[1]=0;pix[1]<(*fits_new).head.naxis[2];pix[1]++){
for(pix[2]=0;pix[2]<(*fits_new).head.naxis[3];pix[2]++){
    pos=PIXtoPOS(fits_new,pix);
    PIXtoX(fits_new,x,pix);
    xr[0]=x[0]; xr[1]=x[1]; 
    sum=0.0;	wgt=0.0;
    for(ii=0;ii<(*fits).head.naxis[3];ii++){
	xr[2]=(*fits).head.crval[3]+(*fits).head.cdelt[3]*((double)(ii+1)-(*fits).head.crpix[3]);
	XtoPIX(fits,xr,pixr);
	posr=PIXtoPOS(fits,pixr);
	if(Blank((*fits).data[posr])!=0)	continue;
	sum+=(*fits).data[posr]*exp((-1.0)*pow((x[2]-xr[2])/w,2.0));
	wgt+=exp((-1.0)*pow((x[2]-xr[2])/w,2.0));
    }
    if(wgt==0.0)	(*fits_new).data[pos]=DEFAULT_BLANK_VALUE;
    else		(*fits_new).data[pos]=sum/wgt;
}}}
}

else if((*fits).head.naxis[0]==2){	/*	2-d case	*/

for(pix[0]=0;pix[0]<(*fits_new).head.naxis[1];pix[0]++){
fprintf(stderr,"%d\n",pix[0]);
for(pix[1]=0;pix[1]<(*fits_new).head.naxis[2];pix[1]++){
    pos=PIXtoPOS(fits_new,pix);
    PIXtoX(fits_new,x,pix);
    xr[0]=x[0]; 
    sum=0.0;	wgt=0.0;
    for(ii=0;ii<(*fits).head.naxis[2];ii++){
	xr[1]=(*fits).head.crval[2]+(*fits).head.cdelt[2]*((double)(ii+1)-(*fits).head.crpix[2]);
	XtoPIX(fits,xr,pixr);
	posr=PIXtoPOS(fits,pixr);
	if(Blank((*fits).data[posr])!=0)	continue;
	sum+=(*fits).data[posr]*exp((-1.0)*pow((x[1]-xr[1])/w,2.0));
	wgt+=exp((-1.0)*pow((x[1]-xr[1])/w,2.0));
    }
    if(wgt==0.0)	(*fits_new).data[pos]=DEFAULT_BLANK_VALUE;
    else		(*fits_new).data[pos]=sum/wgt;
}}
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
double	g, w;
char	in_filename[80], out_filename[80], filehead[80];
char	buffer[80], *token;




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

sprintf(out_filename,"%s.smv.FITS",filehead);
fp_out=fopen(out_filename,"w");
if(fp_out==NULL){
	fprintf(stderr,"Can't open %s !%d\n",out_filename,strlen(filehead));
	exit(1);
}



fprintf(stderr,"Velocity Grid : ");
fscanf(stdin,"%lf",&g);

fprintf(stderr,"Velocity Width : ");
fscanf(stdin,"%lf",&w);
w/=pow(log(2.0),0.5);




fprintf(stderr,"Reading the FITS file.\n");
err_code=read_Fits(&fits, fp_in);
if(err_code!=0){
	fprintf(stderr,
	"Error in reading the FITS file. Error code=%d\n"
	,err_code);	
	exit(999);
}
fclose(fp_in);

fprintf(stderr,"Smoothing the data.\n");
err_code=smooth_FitsData(&fits, &fits_new, g, w);
if(err_code!=0){
	fprintf(stderr,
	"Error in smoothing the FITS data. Error code=%d\n"
	,err_code);	
	exit(999);
}

fprintf(stderr,"Writing a new FITS file.\n");
err_code=output_Fits(&fits_new, fp_out);
if(err_code!=0){
	fprintf(stderr,
	"Error in writing the FITS file. Error code=%d\n"
	,err_code);	
	exit(999);
}
fclose(fp_out);


}	/*main*/



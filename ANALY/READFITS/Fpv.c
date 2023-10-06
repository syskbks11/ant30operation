/*	Make a P-V FITS from a FITS cube	
1998/12/24	Tomo OKA	:	Start Coding 
1998/12/2	Tomo OKA	:	Ver.1.00
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <profhead.h>
#include <starlib.h>
#include <fitshead.h>

#define	DEBUG	0
#define	RESTRICT	2.0



double	sign(x)
double	x;
{
double	s;

if(x>=0.0)	s=1.0;
else if(x<0.0)	s=(-1.0);

return(s);
}



int	slice_FitsData(fits, fits_new, range, w, g)
FITS	*fits, *fits_new;
RANGE	range[];
double	w[], g[];
{
int	ii, jj ,n;
int	axis[MAX_DIMENSION];
int	data_num, data_n_num;
int	pix[MAX_DIMENSION], pixn[MAX_DIMENSION];
int	min[MAX_DIMENSION], max[MAX_DIMENSION];
long	pos, posn;
double	x[MAX_DIMENSION], xn[MAX_DIMENSION];
double	sep[MAX_DIMENSION], cosd[MAX_DIMENSION];
double	Exp, weight, declination, length; 
int	blank, ra, dec;

	

copy_FitsHeader(fits, fits_new);

if(	fabs((range[0].max-range[0].min)*cosd[0]) < 
	fabs((range[1].max-range[1].min)*cosd[1]))
{axis[0]=2; axis[1]=1; axis[2]=0;}
else if(fabs((range[0].max-range[0].min)*cosd[0]) > 
	fabs((range[1].max-range[1].min)*cosd[1]))
{axis[0]=2; axis[1]=0; axis[2]=1;}


FindDec(fits,cosd);
length=pow(pow((range[axis[1]].max-range[axis[1]].min)*cosd[axis[1]] ,2.0) + 
	pow((range[axis[2]].max-range[axis[2]].min)*cosd[axis[2]] ,2.0), 0.5);

(*fits_new).head.cdelt[1]=sign(range[axis[0]].max-range[axis[0]].min)*g[axis[0]];
(*fits_new).head.crval[1]=range[axis[0]].min;
(*fits_new).head.crpix[1]=1.0;
strcpy((*fits_new).head.ctype[1],"\'V-LSR\'");

(*fits_new).head.cdelt[2]=(range[axis[1]].max-range[axis[1]].min)*cosd[axis[1]]/length*g[axis[1]];
(*fits_new).head.crval[2]=range[axis[1]].min;
(*fits_new).head.crpix[2]=1.0;
(*fits_new).head.crota[2]=atan((range[axis[1]].max-range[axis[1]].min)/(range[axis[2]].max-range[axis[2]].min))
			*180.0/PI;
strcpy((*fits_new).head.ctype[2],(*fits).head.ctype[axis[1]+1]);

(*fits_new).head.cdelt[3]=(range[axis[2]].max-range[axis[2]].min)*cosd[axis[2]]/length*g[axis[2]];
(*fits_new).head.crval[3]=range[axis[2]].min;
(*fits_new).head.crpix[3]=1.0;
strcpy((*fits_new).head.ctype[3],(*fits).head.ctype[axis[2]+1]);

(*fits_new).head.naxis[0]=3;
(*fits_new).head.naxis[1]=tint(fabs((range[axis[0]].max-range[axis[0]].min)*cosd[axis[0]]/(*fits_new).head.cdelt[1]))+1;
(*fits_new).head.naxis[2]=tint(fabs((range[axis[1]].max-range[axis[1]].min)*cosd[axis[1]]/(*fits_new).head.cdelt[2]))+1;
(*fits_new).head.naxis[3]=1;

for(jj=1;jj<=(*fits_new).head.naxis[0];jj++){
	fprintf(stderr,"NAXIS[%d] = %d\tCDELT[%d] = %lf\n",jj,(*fits_new).head.naxis[jj],jj,(*fits_new).head.cdelt[jj]);	
}


data_n_num=1;
for(jj=1;jj<=(*fits_new).head.naxis[0];jj++){
	data_n_num=data_n_num*(*fits_new).head.naxis[jj];
}
(*fits_new).data=calloc(data_n_num,4);

for(ii=0;ii<data_n_num;ii++){
	(*fits_new).data[ii]=0.0;
}

FindDec(fits_new,cosd);
for(pixn[0]=0;pixn[0]<(*fits_new).head.naxis[1];pixn[0]++){
fprintf(stderr,"%d/%d\n",pixn[0]+1,(*fits_new).head.naxis[1]);
for(pixn[1]=0;pixn[1]<(*fits_new).head.naxis[2];pixn[1]++){
pixn[2]=0;
	posn=PIXtoPOS(fits_new,pixn);
	blank=0;
	pixn[2]=pixn[1];
	PIXtoTX(fits_new,xn,pixn);	
	for(jj=0;jj<(*fits).head.naxis[0];jj++){x[axis[jj]]=xn[jj];}
	TXtoPIX(fits,x,pix);
	for(jj=0;jj<(*fits_new).head.naxis[0];jj++){
		min[axis[jj]] = pix[axis[jj]] - tint(fabs(w[axis[jj]]/2.0/cosd[jj]*RESTRICT/(*fits).head.cdelt[axis[jj]+1]));
		max[axis[jj]] = pix[axis[jj]] + tint(fabs(w[axis[jj]]/2.0/cosd[jj]*RESTRICT/(*fits).head.cdelt[axis[jj]+1]));
		if((min[axis[jj]]>=(*fits).head.naxis[axis[jj]+1])||(max[axis[jj]]<0)){
			blank++;
		}
		else{
			if(min[axis[jj]]<0)	min[axis[jj]]=0;
			if(max[axis[jj]]>=(*fits).head.naxis[axis[jj]+1])	
						max[axis[jj]]=(*fits).head.naxis[axis[jj]+1]-1;
		}
	}
	if(blank>0){
		(*fits_new).data[posn] = DEFAULT_BLANK_VALUE; 
		continue;	
	}
	weight=0.0; 

	for(pix[0]=min[0];pix[0]<=max[0];pix[0]++){
	for(pix[1]=min[1];pix[1]<=max[1];pix[1]++){
	for(pix[2]=min[2];pix[2]<=max[2];pix[2]++){
		pos=PIXtoPOS(fits,pix);
		if(Blank((*fits).data[pos])==(-1)){continue;}
		else{
		Exp=1.0; 	
		for(jj=0;jj<(*fits_new).head.naxis[0];jj++){
			PIXtoTX(fits,x,pix);
			sep[jj] = fabs(xn[jj]-x[axis[jj]]);
			Exp = Exp*exp(-pow((sep[jj]*cosd[jj]/(w[axis[jj]]/2.0/pow(log(2.0),0.5))),2.0));
		}
		(*fits_new).data[posn] = (*fits_new).data[posn] + (*fits).data[pos]*Exp;
		weight = weight + Exp; }	/* else	*/
	}}}	/*	pix loop	*/
	if(weight==0.0){	
		(*fits_new).data[posn] = (float)DEFAULT_BLANK_VALUE;}
	else	(*fits_new).data[posn] = (*fits_new).data[posn]/weight;
/* fprintf(stderr,"%lf\t%lf\t%lf :\t %lf\n",xn[0],xn[1],xn[2],(*fits_new).data[posn]); */
}
}	/*	pixn loop	*/


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
double		w[MAX_DIMENSION];
double		gn[MAX_DIMENSION];



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


if(fits.head.naxis[0]<3){
	fprintf(stderr,"FITS must be three dimensional.\n");
	exit(1);
}


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
	fprintf(stderr,"Range #%d (%lf,%lf) : %d\n",ii+1,range[ii].min,range[ii].max, fits.head.naxis[ii+1]);
}
fprintf(stderr,"\n");


while(1){fprintf(stderr,"Start Position [x,y]: ");
	fscanf(stdin,"%s",buffer);
	token=strtok(buffer,",");	
	if(token!=NULL)	range[0].min=atof(token);
	else	continue;
	token=strtok(NULL,",");
	if(token!=NULL){range[1].min=atof(token);	break;}
	else	continue;
}
while(1){fprintf(stderr,"End Position [x,y]  : ");
	fscanf(stdin,"%s",buffer);
	token=strtok(buffer,",");	
	if(token!=NULL)	range[0].max=atof(token);
	else	continue;
	token=strtok(NULL,",");	
	if(token!=NULL){range[1].max=atof(token);	break;}
	else	continue;
}
while(1){fprintf(stderr,"Velocity Range [min,max]: ");
	fscanf(stdin,"%s",buffer);
	token=strtok(buffer,",");	
	if(token!=NULL)	range[2].min=atof(token);
	else	continue;
	token=strtok(NULL,",");	
	if(token!=NULL){range[2].max=atof(token);	break;}
	else	continue;
}
while(1){fprintf(stderr,"Smoothing widths [spatial,velocity]:");
	fscanf(stdin,"%s",buffer);
	token=strtok(buffer,",");	
	if(token!=NULL){w[0]=atof(token); w[1]=atof(token);}
	else	continue;
	token=strtok(NULL,",");	
	if(token!=NULL){w[2]=atof(token);	break;}
	else	continue;
}
while(1){fprintf(stderr,"Grid spacings [spatial,velocity]   :");
	fscanf(stdin,"%s",buffer);
	token=strtok(buffer,",");	
	if(token!=NULL){gn[0]=atof(token); gn[1]=atof(token);}
	else	continue;
	token=strtok(NULL,",");	
	if(token!=NULL){gn[2]=atof(token);	break;}
	else	continue;
}



fprintf(stderr,"Slicing the data.\n");
err_code=slice_FitsData(&fits, &fits_new, range, w, gn);
if(err_code!=0){
	fprintf(stderr,
	"Error in cutting the FITS data. Error code=%d\n"
	,err_code);	
	exit(999);
}


sprintf(out_filename,"%s.pv.FITS",filehead);
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

fprintf(stderr,"Fpv ended successfully.\n",out_filename);


}	/*main*/



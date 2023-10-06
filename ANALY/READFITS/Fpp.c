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







int	slicecut_FitsData(fits, range, w, gs)
FITS	*fits;
RANGE	range[];
double	w, gs;
{
int	ii, jj ,n;
int	data_num;
int	pix[MAX_DIMENSION], pnum;
int	min[MAX_DIMENSION], max[MAX_DIMENSION];
long	pos, posn;
double	xx[MAX_DIMENSION], tx[MAX_DIMENSION], dx[MAX_DIMENSION];
double	sep[MAX_DIMENSION], cosd[MAX_DIMENSION];
double	data_new, Exp, weight, declination, length; 
int	blank, ra, dec;


if((*fits).head.naxis[0]!=2){
	fprintf(stderr,"Input FITS must be 2-dimensional.\n");
	return(1);
}

FindDec(fits,cosd);
length = pow(pow((fabs((range[0].max-range[0].min)/cosd[0])), 2.0) + 
		pow((fabs((range[1].max-range[1].min)/cosd[1])), 2.0), 0.5); 
pnum = tint(length/gs)+1; 
fprintf(stderr,"length = %lf\t pix_num = %d\n",length, pnum);

for(ii=0;ii<(*fits).head.naxis[0];ii++){
	dx[ii] = (range[ii].max-range[ii].min)/(double)(pnum-1) ; 
	fprintf(stderr,"dx[%d] = %lf\n",ii,dx[ii]);
}

for(ii=0;ii<(*fits).head.naxis[0];ii++){tx[ii] = range[ii].min;}
for(jj=0;jj<pnum;jj++){
	TXtoPIX(fits,tx,pix);
	for(ii=0;ii<(*fits).head.naxis[0];ii++){
		min[ii] = pix[ii] - tint(fabs(w/2.0/cosd[ii]*RESTRICT/(*fits).head.cdelt[ii+1]));	
		max[ii] = pix[ii] + tint(fabs(w/2.0/cosd[ii]*RESTRICT/(*fits).head.cdelt[ii+1]));	
		if(min[ii]<0)	min[ii]=0;
		if(max[ii]>=(*fits).head.naxis[ii+1])	max[ii]=(*fits).head.naxis[ii+1]-1;
	}

	data_new=0.0; weight=0.0;
	for(pix[0]=min[0];pix[0]<=max[0];pix[0]++){
	for(pix[1]=min[1];pix[1]<=max[1];pix[1]++){
		PIXtoX(fits,xx,pix);
		pos=PIXtoPOS(fits,pix);
		if(Blank((*fits).data[pos])==(-1))	continue;
		Exp=1.0;
		for(ii=0;ii<(*fits).head.naxis[0];ii++){
			sep[ii]=fabs(tx[ii]-xx[ii]);
			Exp = Exp*exp(-pow((sep[ii]*cosd[ii]/(w/2.0/pow(log(2.0),0.5))),2.0));
		}
		data_new = data_new + (*fits).data[pos]*Exp;
		weight = weight + Exp;
	}}
	if(weight==0.0)	data_new = 0.0;	
	else	data_new = data_new/weight;
	
	fprintf(stdout,"%lf\t%lf\t%lf\n",tx[0],tx[1],data_new);
	for(ii=0;ii<(*fits).head.naxis[0];ii++){tx[ii] = tx[ii]+dx[ii];}
}


return(0);
}






main(argc,argv)
int	argc;
char	*argv[];
{
FILE		*fp_in, *fp_out;
int		err_code;
FITS		fits;
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
double		w, gs;

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


if(fits.head.naxis[0]!=2){
	fprintf(stderr,"FITS must be 2-dimensional.\n");
	exit(1);
}


FindDec(&fits, cosd);
for(ii=0;ii<fits.head.naxis[0];ii++){
range[ii].min=fits.head.crval[ii+1]+fits.head.cdelt[ii+1]/cosd[ii]*(1.0-fits.head.crpix[ii+1]);
range[ii].max=fits.head.crval[ii+1]+fits.head.cdelt[ii+1]/cosd[ii]*(fits.head.naxis[ii+1]-fits.head.crpix[ii+1]);
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

while(1){fprintf(stderr,"Smoothing width :");
	fscanf(stdin,"%s",buffer);
	if(buffer!=NULL){w=atof(buffer); break;}
	else	continue;
}
while(1){fprintf(stderr,"Grid spacing    :");
	fscanf(stdin,"%s",buffer);
	if(buffer!=NULL){gs=atof(buffer); break;}
	else	continue;
}



fprintf(stderr,"Slicing the data.\n");
err_code=slicecut_FitsData(&fits, range, w, gs);
if(err_code!=0){
	fprintf(stderr,
	"Error in slicing the FITS data. Error code=%d\n"
	,err_code);	
	exit(999);
}


fprintf(stderr,"Fpp ended successfully.\n");


}	/*main*/



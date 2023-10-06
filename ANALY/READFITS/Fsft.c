/*	Coordinate Shift for a FITS DATA	
1997/12/	Tomo OKA	:	Start Coding, Ver.1.00
1998/3/2	Tomo OKA	:	Ver.1.10
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
#define	GRID		4.0



int	pix[MAX_DIMENSION];
int	pixr[MAX_DIMENSION];
long	pos, posr;
double	lsep[MAX_DIMENSION], gsep[MAX_DIMENSION];
int	edge;


void	Gaussian_smooth(fits,fits_new,w,weight,blank,ra,dec,cos_dec)
FITS	*fits, *fits_new;
double	w[];
float	weight[];
short	blank[];
int	ra, dec;
double	cos_dec;
{
static int	cc=0, nn=0;
int		min, max;
double	ch;


if(cc < (*fits).head.naxis[0] ){

if(cc+1==ra){
	ch=(*fits_new).head.crval[cc+1]+(*fits_new).head.cdelt[cc+1]/cos_dec*((double)pix[cc]+1.0-(*fits_new).head.crpix[cc+1]);
	min = (int)((*fits).head.crpix[cc+1]-1.0+(ch-(*fits).head.crval[cc+1])/(*fits).head.cdelt[cc+1]*cos_dec)
		-(int)(RESTRICT*w[cc]/2.0/fabs((*fits).head.cdelt[cc+1])*cos_dec); 	
	if(min<0) min=0;
	max=(int)((*fits).head.crpix[cc+1]-1.0+(ch-(*fits).head.crval[cc+1])/(*fits).head.cdelt[cc+1]*cos_dec)
		+(int)(RESTRICT*w[cc]/2.0/fabs((*fits).head.cdelt[cc+1])*cos_dec);
	if(max>=(*fits).head.naxis[cc+1])	max=(*fits).head.naxis[cc+1]-1;
}
else{
	ch=(*fits_new).head.crval[cc+1]+(*fits_new).head.cdelt[cc+1]*((double)pix[cc]+1.0-(*fits_new).head.crpix[cc+1]);
	min = (int)((*fits).head.crpix[cc+1]-1.0+(ch-(*fits).head.crval[cc+1])/(*fits).head.cdelt[cc+1])
		-(int)(RESTRICT*w[cc]/2.0/fabs((*fits).head.cdelt[cc+1])); 	
	if(min<0) min=0;
	max=(int)((*fits).head.crpix[cc+1]-1.0+(ch-(*fits).head.crval[cc+1])/(*fits).head.cdelt[cc+1])
		+(int)(RESTRICT*w[cc]/2.0/fabs((*fits).head.cdelt[cc+1]));
	if(max>=(*fits).head.naxis[cc+1])	max=(*fits).head.naxis[cc+1]-1;
}

	for(pixr[cc]=min;pixr[cc]<=max;pixr[cc]++){
		cc++;
		Gaussian_smooth(fits,fits_new,w,weight,blank,ra,dec,cos_dec);
		cc--;
	}
}
else{
	{int	ii, flag;
	double	sep[MAX_DIMENSION];
	double	Exp;
	Exp=1.0;
	for(ii=0;ii<(*fits).head.naxis[0];ii++){
		if(ii+1==ra){	
		sep[ii] = ((*fits).head.crval[ii+1]+(*fits).head.cdelt[ii+1]/cos_dec*((double)pixr[ii]-(*fits).head.crpix[ii+1]+1.0)) 
			- ((*fits_new).head.crval[ii+1]+(*fits_new).head.cdelt[ii+1]/cos_dec*((double)pix[ii]-(*fits_new).head.crpix[ii+1]+1.0));
		sep[ii] = sep[ii] * cos_dec;
		}		
		else
		sep[ii] = ((*fits).head.crval[ii+1]+(*fits).head.cdelt[ii+1]*((double)pixr[ii]-(*fits).head.crpix[ii+1]+1.0)) 
			- ((*fits_new).head.crval[ii+1]+(*fits_new).head.cdelt[ii+1]*((double)pix[ii]-(*fits_new).head.crpix[ii+1]+1.0));
		if(w[ii]==0.0){
			if(fabs(sep[ii])<=lsep[ii])	Exp = Exp;
			else if(fabs(sep[ii])>gsep[ii])	Exp = 0.0;
			else				Exp = Exp*(1.0 - (sep[ii]-lsep[ii])/fabs((*fits).head.cdelt[ii+1]));
		}
		else	Exp = Exp * exp(-pow((sep[ii]/(w[ii]/2.0/pow(log(2.0),0.5))),2.0));
	}
	posr=PIXtoPOS(fits,pixr);
	if(Blank((*fits).data[posr])==(-1)){
		if(edge==0){
			flag=0;
			for(ii=0;ii<(*fits).head.naxis[0];ii++){if(fabs(sep[ii])<=gsep[ii]) flag++;}
			if(flag==(*fits).head.naxis[0])	blank[pos]++;
		}
		else	blank[pos]++;
	}
	else{	(*fits_new).data[pos] = (*fits_new).data[pos] + (*fits).data[posr]* (float)Exp;
		weight[pos] = weight[pos] + Exp;
	}

 	nn++;
	}
}
}



void	Call_Gaussian_smooth(fits,fits_new,w,weight,blank,ra,dec,cos_dec)
FITS	*fits, *fits_new;
double	w[];
double	weight[];
short	blank[];
int	ra, dec;
double	cos_dec;
{
int	ii;
static int	c=0;
int	cc;

cc=(*fits_new).head.naxis[0]-c-1;
if(c < (*fits_new).head.naxis[0]){
	for(pix[cc]=0;pix[cc]<(*fits_new).head.naxis[cc+1];pix[cc]++){
		c++;
		Call_Gaussian_smooth(fits,fits_new,w,weight,blank,ra,dec,cos_dec);
		c--;
	}
}
else{
	pos=PIXtoPOS(fits_new,pix);
	Gaussian_smooth(fits,fits_new,w,weight,blank,ra,dec,cos_dec);
}

}




int	smooth_FitsData(fits, fits_new, w, g, anc, shift)
FITS	*fits, *fits_new;
double	w[], g[], anc[], shift[];
{
int	i, j, k, ii, jj;
int	data_num, data_n_num;
int	gr[MAX_DIMENSION];


/********************************/
double	min, max, declination, cos_dec;
int	ra, dec;
ra=(-1);	dec=(-1);
for(ii=1;ii<=(*fits).head.naxis[0];ii++){
	if((strstr((*fits).head.ctype[ii],"RA")!=NULL)||
	(strstr((*fits).head.ctype[ii],"GLON")!=NULL)){	ra=ii;
		if((*fits).head.crval[ii]<(-90.0))	
			(*fits).head.crval[ii]=(*fits).head.crval[ii]+360.0;
	}
	if((strstr((*fits).head.ctype[ii],"DEC")!=NULL)||
	(strstr((*fits).head.ctype[ii],"GLAT")!=NULL))	dec=ii;
}
if(dec==(-1))	cos_dec=1.0;
else{
min=(*fits).head.crval[dec]+(*fits).head.cdelt[dec]*(1.0-(*fits).head.crpix[dec]);
max=(*fits).head.crval[dec]+(*fits).head.cdelt[dec]*((double)(*fits).head.naxis[dec]-(*fits).head.crpix[dec]);
declination=(min+max)/2.0;
cos_dec = cos(declination*PI/180.0);
}
fprintf(stderr,"RA=%d, Dec=%d, cos(Dec)=%lf\n",ra,dec,cos_dec);
/********************************/

for(i=0;i<(*fits).head.naxis[0];i++){
if(i+1==ra)	(*fits).head.crval[i+1]=(*fits).head.crval[i+1] + shift[i]/cos_dec;
else		(*fits).head.crval[i+1]=(*fits).head.crval[i+1] + shift[i];
}

for(i=0;i<(*fits).head.naxis[0];i++){
	(*fits_new).head.naxis[i+1]=tint((double)(*fits).head.naxis[i+1]/g[i]*fabs((*fits).head.cdelt[i+1]));	
/*	(*fits_new).head.naxis[i+1]=(int)floor((double)(*fits).head.naxis[i+1]/g[i]*fabs((*fits).head.cdelt[i+1]));	*/
	(*fits_new).head.cdelt[i+1]=fabs(g[i]/(*fits).head.cdelt[i+1])*(*fits).head.cdelt[i+1];
	(*fits_new).head.crval[i+1]=(double)anc[i];
if(i+1==ra){
	(*fits_new).head.crpix[i+1]=(double)tint( (double)(*fits_new).head.naxis[i+1]/2.0 + 
				( (anc[i]-(*fits).head.crval[i+1])  + 
				(*fits).head.cdelt[i+1]/cos_dec*( (*fits).head.crpix[i+1]-(double)(*fits).head.naxis[i+1]/2.0 ) )
				/(*fits_new).head.cdelt[i+1]*cos_dec
					   );
}
else{
	(*fits_new).head.crpix[i+1]=(double)tint( (double)(*fits_new).head.naxis[i+1]/2.0 + 
				( (anc[i]-(*fits).head.crval[i+1])  + 
				(*fits).head.cdelt[i+1]*( (*fits).head.crpix[i+1]-(double)(*fits).head.naxis[i+1]/2.0 ) )
				/(*fits_new).head.cdelt[i+1]
					   );
}
	lsep[i] = (fabs((*fits_new).head.cdelt[i+1])-fabs((*fits).head.cdelt[i+1]))/2.0;
	gsep[i] = (fabs((*fits_new).head.cdelt[i+1])+fabs((*fits).head.cdelt[i+1]))/2.0;
}


fprintf(stderr,"(");
for(ii=0;ii<(*fits).head.naxis[0]-1;ii++){
	fprintf(stderr,"%d, ",(*fits).head.naxis[ii+1]);
}
fprintf(stderr,"%d)->",(*fits).head.naxis[(*fits).head.naxis[0]]);
fprintf(stderr,"(");
for(ii=0;ii<(*fits_new).head.naxis[0]-1;ii++){
	fprintf(stderr,"%d, ",(*fits_new).head.naxis[ii+1]);
}
fprintf(stderr,"%d)\n",(*fits_new).head.naxis[(*fits_new).head.naxis[0]]);


fprintf(stderr,"Coordinate shifts: (");
for(ii=0;ii<(*fits_new).head.naxis[0]-1;ii++){
	fprintf(stderr,"%lf, ",shift[ii]);
}
fprintf(stderr,"%lf)\n",shift[(*fits_new).head.naxis[0]-1]);


fprintf(stderr,"Smoothing widths: (");
for(ii=0;ii<(*fits_new).head.naxis[0]-1;ii++){
	fprintf(stderr,"%lf, ",w[ii]);
}
fprintf(stderr,"%lf)\n",w[(*fits_new).head.naxis[0]-1]);


fprintf(stderr,"Anchor position : (");
for(ii=0;ii<(*fits_new).head.naxis[0]-1;ii++){
	fprintf(stderr,"%lf, ",anc[ii]);
}
fprintf(stderr,"%lf)\n",(double)anc[(*fits_new).head.naxis[0]-1]);



data_num=(*fits).head.data_num;
data_n_num=1;
for(jj=1;jj<=(*fits_new).head.naxis[0];jj++){
	data_n_num=data_n_num*(*fits_new).head.naxis[jj];
}
(*fits_new).head.data_num=data_n_num;

/*	Gaussian smoothing	*/
{float	*weight; 
short	*blank;

weight = calloc(data_n_num,sizeof(weight));
blank = calloc(data_n_num,sizeof(blank));
(*fits_new).data=calloc(data_n_num,sizeof((*fits_new).data));

for(ii=0;ii<data_n_num;ii++){	
	(*fits_new).data[ii]=0.0;
	weight[ii]=0.0;
	blank[ii]=0;
}

Call_Gaussian_smooth(fits,fits_new,w,weight,blank,ra,dec,cos_dec);

for(ii=0;ii<data_n_num;ii++){
	if((blank[ii]>0)||(weight[ii]==0.0))	(*fits_new).data[ii]=(float)DEFAULT_BLANK_VALUE;
	else	(*fits_new).data[ii]=(*fits_new).data[ii]/weight[ii];
}
free(weight);
}
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
double		w[MAX_DIMENSION], g[MAX_DIMENSION], anc[MAX_DIMENSION];
double		shift[MAX_DIMENSION];
int		dim;
char		in_filename[80];
char		out_filename[80];
char		filehead[80];
char		buffer[80];
char		*token;
double	min, max, declination;
int	ra, dec;


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
	"Error in reading the FITS file. Error code=%d\n"
	,err_code);	
	exit(999);
}
fclose(fp_in);


copy_FitsHeader(&fits, &fits_new);


fprintf(stderr,"Coordinate shifts ( ");
for(ii=1;ii<fits.head.naxis[0];ii++){
fprintf(stderr,"0.0, ");
}
fprintf(stderr,"0.0 ): ");
fscanf(stdin,"%s",buffer);	
token=strtok(buffer,", \t");	
for(jj=0;jj<fits.head.naxis[0];jj++){
	g[jj]=fabs(fits.head.cdelt[jj+1]);
	if(token==NULL){
		for(ii=jj;ii<fits.head.naxis[0];ii++){shift[ii]=0.0; }
		break;
	}
	else	shift[jj]=atof(token);
	token=strtok(NULL,", \t");	
}


fprintf(stderr,"Smoothing widths ( ");
for(ii=1;ii<fits.head.naxis[0];ii++){
fprintf(stderr,"%lf,",fabs(fits.head.cdelt[ii]));
}
fprintf(stderr,"%lf ): ",fabs(fits.head.cdelt[fits.head.naxis[0]]));
fscanf(stdin,"%s",buffer);
token=strtok(buffer,", \t");
for(jj=0;jj<fits.head.naxis[0];jj++){
	if(token==NULL){
		for(ii=jj;ii<fits.head.naxis[0];ii++){w[ii]=fabs(fits.head.cdelt[ii+1])/GRID; }
		break;
	}
	else	w[jj]=atof(token);
	token=strtok(NULL,", \t");	
}


fprintf(stderr,"Anchor position  ( ");
for(ii=1;ii<fits.head.naxis[0];ii++){
fprintf(stderr,"%lf,",fits.head.crval[ii]);
}
fprintf(stderr,"%lf ): ",fits.head.crval[fits.head.naxis[0]]);
fscanf(stdin,"%s",buffer);
token=strtok(buffer,", \t");
for(jj=0;jj<=fits.head.naxis[0];jj++){
	if(token==NULL){
		for(ii=jj;ii<fits.head.naxis[0];ii++){anc[ii]=fits.head.crval[ii+1]; }
		break;
	}
	else	anc[jj]=atof(token);
	token=strtok(NULL,", \t");	
}

/*
fprintf(stderr,"Data edge hundling (0:blunt,1:sharp): ");
fscanf(stdin,"%d",&edge);
*/
edge=0;

sprintf(out_filename,"%s.sft.FITS",filehead);
fp_out=fopen(out_filename,"w");
if(fp_out==NULL){
	fprintf(stderr,"Can't open %s !\n",out_filename);
	exit(1);
}



fprintf(stderr,"\nSmoothing %s\n",in_filename);
err_code=smooth_FitsData(&fits, &fits_new, w, g, anc, shift);
if(err_code!=0){
	fprintf(stderr,
	"Error in smoothing FITS data. Error code=%d\n"
	,err_code);	
	exit(999);
}

fprintf(stderr,"Writing to %s\n",out_filename);
err_code=output_Fits(&fits_new, fp_out);
if(err_code!=0){
	fprintf(stderr,
	"Error in writing a FITS file. Error code=%d\n"
	,err_code);	
	exit(999);
}
fclose(fp_out);

fprintf(stderr,"Fsm ended successfully.\n",out_filename);


}	/*main*/



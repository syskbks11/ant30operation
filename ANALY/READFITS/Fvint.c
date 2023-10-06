/*	Velocity integration of a FITS DATA 	*/
/*	dispersion, velocity field	*/

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <math.h>

#include <profhead.h>
#include <starlib.h>
#include <fitshead.h>

#define	DEBUG	0
#define	RESTRICT	3.0
#define	GRID		3.0



int	option(argc,argv,mode)
int	argc;
char	*argv[];
int	*mode;
{
int	ii, jj, count;
char	*token;

/* default values */
*mode=0;

for(ii=1;ii<argc;ii++){
	if(argv[ii][0]=='-'){
		if(strcmp(argv[ii],"-int")==0)	*mode=0;
		else if(strcmp(argv[ii],"-dv")==0)	*mode=1;
		else if(strcmp(argv[ii],"-vf")==0)	*mode=2;
	}
}


return(0);
}



int	Calc_Integrated_Intensity(fits, fits_new, v, mode)
FITS	*fits, *fits_new;
RANGE	*v;
int	mode;
{
long	ch, n, num, pix[MAX_DIMENSION];
double	x[MAX_DIMENSION];
RANGE_long	vch;
int	ii, jj;
long	data_number;
double	w, ww, w2, wv, wv2, dv;



copy_FitsHeader(fits, fits_new);
(*fits_new).head.naxis[0]=(*fits).head.naxis[0]-1;


data_number=1;
for(ii=1;ii<=(*fits_new).head.naxis[0];ii++){
	data_number=data_number*(*fits_new).head.naxis[ii];
}
(*fits_new).data=(float *)calloc(data_number,4);

vch.min= (long)( ((*v).min-(*fits).head.crval[3])/(*fits).head.cdelt[3] + (*fits).head.crpix[3] - 1 );
vch.max= (long)( ((*v).max-(*fits).head.crval[3])/(*fits).head.cdelt[3] + (*fits).head.crpix[3] - 1 );
STAR_SortRangeLong(&vch);

fprintf(stderr,"Velocity range : %lf,%lf\n",(*v).min,(*v).max);
fprintf(stderr,"ch range : %d,%d\n",vch.min,vch.max);

if(vch.min<0)	vch.min=0;
if(vch.max>((*fits).head.crval[3]+(*fits).head.cdelt[3]*((*fits).head.naxis[3]-(*fits).head.crpix[3])))	
		vch.max=(*fits).head.naxis[3]-1;


for(pix[0]=0;pix[0]<=(*fits).head.naxis[1]-1;pix[0]++){
for(pix[1]=0;pix[1]<=(*fits).head.naxis[2]-1;pix[1]++){
w=0.0; w2=0.0; ww=0.0; wv=0.0; wv2=0.0; num=0;
for(pix[2]=vch.min;pix[2]<=vch.max;pix[2]++){
	ch=DataPosition(fits,pix);
	PIXtoTX(fits,x,pix);
	if(Blank((*fits).data[ch])==0){
		w += (double)(*fits).data[ch];
		w2 += (double)(*fits).data[ch]*(double)(*fits).data[ch];
		if((double)(*fits).data[ch]>0.0){
		ww += (double)(*fits).data[ch];
		wv += x[2]*(double)(*fits).data[ch];
		wv2 += x[2]*x[2]*(double)(*fits).data[ch];}
	}
}
if(w>=0.0){	wv /= ww;	
		wv2 /= ww;
		dv = pow((wv2-wv*wv),0.5);	}
pix[2]=0;
n = DataPosition(fits_new,pix);
if(mode==0){
    if(strstr((*fits).head.bunit,"m/s")==NULL)
	(*fits_new).data[n] = (float)(w*fabs((*fits).head.cdelt[3]));
    else
	(*fits_new).data[n] = (float)w;
}
else if(mode==1){
	if( (dv<fabs((*v).max-(*v).min))&&(dv>=0.0) )
		(*fits_new).data[n] = (float)(dv);
	else	(*fits_new).data[n] = DEFAULT_BLANK_VALUE;
}
else if(mode==2){
	if( (wv>=(*v).min)&&(wv<=(*v).max) )
		(*fits_new).data[n] = (float)(wv);
	else	(*fits_new).data[n] = DEFAULT_BLANK_VALUE;
}

}}

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
int		dim;
RANGE		v;
char		in_filename[80];
char		out_filename[80];
char		filehead[80];
char		buffer[80];
char		*token;
int		mode;
char		smode[8];


if(argc<2){
	fprintf(stderr,"Usage : Fvint [-int/-dv/-vf]\n");
    fprintf(stderr,"execute integration mode\n");
}


option(argc,argv,&mode);
if(mode==0)		strcpy(smode,"int");
else if(mode==1)	strcpy(smode,"dv");
else if(mode==2)	strcpy(smode,"vf");

/*fprintf(stderr,"smode = %s\n",smode);*/


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


fprintf(stderr,"Reading the FITS file.\n");
err_code=read_Fits(&fits, fp_in);
if(err_code!=0){
	fprintf(stderr,
	"Error in reading the FITS file. Error code=%d\n"
	,err_code);	
	exit(999);
}
fclose(fp_in);


v.min = fits.head.crval[3] + fits.head.cdelt[3]*(1.0-fits.head.crpix[3]) ; 
v.max = fits.head.crval[3] + fits.head.cdelt[3]*((double)fits.head.naxis[3]-fits.head.crpix[3]) ;
STAR_SortRange(&v);

while(1){
	fprintf(stderr,"Velocity range (%.3lf,%.3lf):",v.min,v.max);
	fscanf(stdin,"%s",buffer);
	token=strtok(buffer,",");	
	if(token!=NULL)	v.min=atof(token);
	else	continue;
	token=strtok(NULL,",");		
	if(token!=NULL){ v.max=atof(token); break;}
	else	continue;
}


fprintf(stderr,"Calculating integrated intensities (mode=%s).\n",smode);
err_code=Calc_Integrated_Intensity(&fits, &fits_new, &v, mode);
if(err_code!=0){
	fprintf(stderr,
	"Error in calculating integrated intensities. Error code=%d\n"
	,err_code);	
	exit(999);
}

sprintf(out_filename,"%s.%+.0lf%+.0lf.%s.FITS",filehead,v.min,v.max,smode);
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



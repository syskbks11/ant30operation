#include	<stdio.h>
#include	<string.h>
#include	<math.h>
#include 	<stdlib.h>

#include        <profhead.h>
#include        <starlib.h>
#include	<fitshead.h>


#define		T_STEP	2.0
#define		T_MIN	0
#define		T_MAX	20.0




main(argc,argv)
int	argc;
char	*argv[];
{
FILE	*fp_in, *fp_out;
int	ii, jj;
int	err_code;
double	scale, tex;
double	bunbo, bunshi, ff;
int	data_num;
FITS	fits;
char	in_filename[80], out_filename[80];
char	filehead[80];
char	buffer[80], *token;


/*fp_in=stdin;*/
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

fp_out=stdout;
scale=1.0;

fprintf(stderr,"Scaling factor =  ");
fscanf(stdin,"%lf",&scale);

fprintf(stderr,"Tex =  ");
fscanf(stdin,"%lf",&tex);


/*	Read FITS File from fp_in	*/
err_code=read_Fits(&fits, fp_in);
if(err_code!=0){
	fprintf(stderr,"Error in reading the FITS file. Error code=%d\n",err_code);
	exit(999);
}

data_num=1;
for(jj=1;jj<=fits.head.naxis[0];jj++){
	data_num = data_num * fits.head.naxis[jj];
}

/*	Create Histogram Array	*/
{
float	data_float;
    bunbo=0.0;
    bunshi=0.0;
for(ii=0;ii<data_num;ii++){
    data_float=fits.data[ii]*scale;
    bunshi+=(double)data_float;
    bunbo+=tex;
}
}

ff=bunshi/bunbo;

/*	Output the Histogram Array	*/
fprintf(fp_out,"ff = %lf / %lf = %lf\n", bunshi, bunbo, ff);


}	/*	end of the main	function	*/




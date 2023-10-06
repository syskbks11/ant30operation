#include	<stdio.h>
#include	<string.h>
#include	<math.h>
/* #include 	<malloc.h> */
#include	<stdlib.h>
#include        <profhead.h>
#include        <starlib.h>
#include	<fitshead.h>


#define	PLNK	6.62559e-27
#define	BLTM	1.38054e-16
#define	TBB	2.7
#define	CLIP	5.0


main(argc,argv)
int	argc;
char	*argv[];
{
FILE	*fp_in1, *fp_in2, *fp_out1, *fp_out2;
int	ii, jj;
int	err_code;
int	data_num, data_num2;
FITS	fits1, fits2, fitso1, fitso2;
char	in_filename1[MAX_CHAR_LENGTH], in_filename2[MAX_CHAR_LENGTH], Tex_filename[MAX_CHAR_LENGTH];
char	out_filename1[MAX_CHAR_LENGTH], out_filename2[MAX_CHAR_LENGTH];
char	filehead[MAX_CHAR_LENGTH];
char	buffer[MAX_CHAR_LENGTH];
char	*token;
double	abu, tau, tex, nu1, nu2, ncol, rat;
double	c1, c2;
double	rms, rms2;
float	data_float, data_max, data_max2;
int	pix[MAX_DIMENSION], max_pix;
long	ch, data_n_num;


/*	Default values	*/

fprintf(stderr,"IN_FITS(12C) -> OUT_FITS(12C peak)\nIN_FITS(13C) -> OUT_FITS(13C peak)\n");
fprintf(stderr,"Input FITS filename(12C) : ");
fscanf(stdin,"%s",buffer);
if((strstr(buffer,"fits")==NULL)&&(strstr(buffer,"FITS")==NULL)){	
	sprintf(in_filename1,"%s.FITS",buffer);
	strcpy(filehead,buffer);
	fp_in1=fopen(in_filename1,"r");
	if(fp_in1==NULL){
		sprintf(in_filename1,"%s.fits",filehead);
		fp_in1=fopen(in_filename1,"r");
		if(fp_in1==NULL){
			fprintf(stderr,"Can't open %s !\n",in_filename1);
			exit(1);
		}
	}
}
else{	strncpy(filehead,buffer,strlen(buffer)-5);
	sprintf(in_filename1,"%s",buffer);
	fp_in1=fopen(in_filename1,"r");
	if(fp_in1==NULL){
		fprintf(stderr,"Can't open %s !\n",in_filename1);
		exit(1);
	}
}

sprintf(out_filename1,"%s.pk.FITS",filehead);
fp_out1=fopen(out_filename1,"w");
if(fp_out1==NULL){
	fprintf(stderr,"Can't open %s !\n",out_filename1);
	exit(1);
}


fprintf(stderr,"Input FITS filename(13C) : ");
fscanf(stdin,"%s",buffer);
if((strstr(buffer,"fits")==NULL)&&(strstr(buffer,"FITS")==NULL)){	
	sprintf(in_filename2,"%s.FITS",buffer);
	strcpy(filehead,buffer);
	fp_in2=fopen(in_filename2,"r");
	if(fp_in2==NULL){
		sprintf(in_filename2,"%s.fits",filehead);
		fp_in2=fopen(in_filename2,"r");
		if(fp_in2==NULL){
			fprintf(stderr,"Can't open %s !\n",in_filename2);
			exit(1);
		}
	}
}
else{	strncpy(filehead,buffer,strlen(buffer)-5);
	sprintf(in_filename2,"%s",buffer);
	fp_in2=fopen(in_filename2,"r");
	if(fp_in2==NULL){
		fprintf(stderr,"Can't open %s !\n",in_filename2);
		exit(1);
	}
}

sprintf(out_filename2,"%s.pk.FITS",filehead);
fp_out2=fopen(out_filename2,"w");
if(fp_out2==NULL){
	fprintf(stderr,"Can't open %s !\n",out_filename2);
	exit(1);
}









/*	Reading input FITS files	*/
fprintf(stderr,"Reading %s\n",in_filename1);
err_code=read_Fits(&fits1, fp_in1);
if(err_code!=0){
	fprintf(stderr,
	"Error in reading %s. Error code=%d\n"
	,err_code,in_filename1);	
	exit(999);
}
fclose(fp_in1);
fprintf(stderr,"Reading %s\n",in_filename2);
err_code=read_Fits(&fits2, fp_in2);
if(err_code!=0){
	fprintf(stderr,
	"Error in reading %s. Error code=%d\n"
	,err_code,in_filename2);	
	exit(999);
}
fclose(fp_in2);


if((fits1.head.naxis[0]!=3)||(fits2.head.naxis[0]!=3)){
	fprintf(stderr,"Input FITS files must be 3-dimensional.\n");
	exit(1);
}

/*	Calculate data numbers	*/

copy_FitsHeader(&fits1, &fitso1);	fitso1.head.naxis[0]=2; 
copy_FitsHeader(&fits2, &fitso2);	fitso2.head.naxis[0]=2; 
data_n_num=fits1.head.naxis[1]*fits1.head.naxis[2];
fitso1.data=calloc(data_n_num,4);
fitso2.data=calloc(data_n_num,4);


for(pix[0]=0;pix[0]<fits1.head.naxis[1];pix[0]++){
for(pix[1]=0;pix[1]<fits1.head.naxis[2];pix[1]++){
data_max=fits1.head.datamin;
data_max2=fits2.head.datamin;
for(pix[2]=0;pix[2]<fits1.head.naxis[3];pix[2]++){
	ch=PIXtoPOS(fits1,pix);
	if((Blank(fits1.data[ch])==(-1))||(Blank(fits2.data[ch])==(-1))) continue;
	if(fits2.data[ch]>data_max2){ 
		data_max=fits1.data[ch];
		data_max2=fits2.data[ch];
	}
}
ch=PIXtoPOS(fitso1,pix);
if(data_max2==fits2.head.datamin){
	fitso1.data[ch]=DEFAULT_BLANK_VALUE;
	fitso2.data[ch]=DEFAULT_BLANK_VALUE;
fprintf(stderr,"(blank)\n");
}
else{
	fitso1.data[ch] = data_max;
	fitso2.data[ch] = data_max2;
}

}
}





/*	Output FITS file	*/
fprintf(stderr,"Writing to %s\n",out_filename1);
output_Fits(&fitso1, fp_out1);
fprintf(stderr,"Writing to %s\n",out_filename2);
output_Fits(&fitso2, fp_out2);

fclose(fp_out1);
fclose(fp_out2);

}	/*	end of the main	function	*/




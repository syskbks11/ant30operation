#include	<stdio.h>
#include	<string.h>
#include	<math.h>
#include 	<stdlib.h>
/*#include 	<limits.h>
#include 	<float.h>*/

#include        <profhead.h>
#include        <starlib.h>
#include	<fitshead.h>







main(argc,argv)
int	argc;
char	*argv[];
{
FILE	*fp_in1, *fp_in2, *fp_out;
int	ii, jj;
int	err_code;
double	clip;
double	scale1, scale2;
int	column;
int	data_num;
FITS	fits1, fits2;
char	in_filename1[MAX_CHAR_LENGTH], in_filename2[MAX_CHAR_LENGTH];
char	out_filename[MAX_CHAR_LENGTH];
char	filehead[MAX_CHAR_LENGTH];
char	buffer[MAX_CHAR_LENGTH];
char	*token;



/*	Default values	*/
clip=(-1.0)*FLT_MAX;
scale1=1.0;
scale2=1.0;


fprintf(stderr,"IN-FITS(1)/IN_FITS(2) -> OUT_FITS\n");
fprintf(stderr,"Input FITS filename(1) : ");
fscanf(stdin,"%s",buffer);
if((strstr(buffer,".fits")==NULL)&&(strstr(buffer,".FITS")==NULL)){	
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

fprintf(stderr,"Input FITS filename(2) : ");
fscanf(stdin,"%s",buffer);
if((strstr(buffer,".fits")==NULL)&&(strstr(buffer,".FITS")==NULL)){	
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



fprintf(stderr,"Clipping Intensity(1): ");
fscanf(stdin,"%s",buffer);
if((strncmp(buffer,"n",1)==0)||(strncmp(buffer,"N",1)==0))	clip=(-1.0)*FLT_MAX;
else	clip=atof(buffer);

fprintf(stderr,"Scaling factor(1): ");
fscanf(stdin,"%lf",&scale1);
fprintf(stderr,"Scaling factor(2): ");
fscanf(stdin,"%lf",&scale2);





/*fp_out=stdout;*/
fprintf(stderr,"Output FITS filename : ");
fscanf(stdin,"%s",buffer);
if((strstr(buffer,".fits")==NULL)&&(strstr(buffer,".FITS")==NULL)){	
	sprintf(out_filename,"%s.FITS",buffer);
	strcpy(filehead,buffer);
	fp_out=fopen(out_filename,"w");
	if(fp_out==NULL){
		sprintf(out_filename,"%s.fits",filehead);
		fp_out=fopen(out_filename,"w");
		if(fp_out==NULL){
			fprintf(stderr,"Can't open %s !\n",out_filename);
			exit(1);
		}
	}
}
else{	strncpy(filehead,buffer,strlen(buffer)-5);
	sprintf(out_filename,"%s",buffer);
	fp_out=fopen(out_filename,"w");
	if(fp_out==NULL){
		fprintf(stderr,"Can't open %s !\n",out_filename);
		exit(1);
	}
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





/*	Calculate data numbers	*/
data_num=1;
for(jj=1;jj<=fits1.head.naxis[0];jj++){
	data_num = data_num * fits1.head.naxis[jj];
}
{int	data_num2;
data_num2=1;
for(jj=1;jj<=fits2.head.naxis[0];jj++){
	data_num2 = data_num2 * fits2.head.naxis[jj];
}
if(data_num!=data_num2){
	fprintf(stderr,"FITS files must have the same matrix. (%d!=%d)\n",data_num,data_num2);
	exit(0);
}
}


/*	Calculate New Data 	*/
{float	data_float, w1, w2;
w1=0.0; w2=0.0;
for(ii=0;ii<data_num;ii++){
	if((fits1.data[ii]<clip)||
	(Blank(fits1.data[ii])==(-1))||
	(Blank(fits2.data[ii])==(-1))){	
		data_float = (float)DEFAULT_BLANK_VALUE;
	}
	else{		
		data_float = (fits1.data[ii]*scale1)/(fits2.data[ii]*scale2);
		w1=w1+fits1.data[ii]*scale1; w2=w2+fits2.data[ii]*scale2; 
	}
	fits1.data[ii]=data_float;
}
fprintf(stderr,"Integrated intensity (1) = %lf\n", w1);
fprintf(stderr,"Integrated intensity (2) = %lf\n", w2);
fprintf(stderr,"Intensity ratio (2)/(1) = %lf\n", w1/w2);
}

/*	Output FITS file	*/
output_Fits(&fits1,fp_out);

fclose(fp_out);

}	/*	end of the main	function	*/




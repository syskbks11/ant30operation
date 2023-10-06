/*
1993/9/1	Tomoharu OKA	:	Coding 開始
1993/9/2	Tomoharu OKA	:	Ver.1.00 完成
1994/9/7	Tomoharu OKA	:	Bug fix for pixel inner structure
1998/3/3	Tomo OKA	:	FITS intensity calibration
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <profhead.h>
#include <starlib.h>
#include <fitshead.h>

#define	DEBUG	0




main(argc,argv)
int	argc;
char	*argv[];
{
FILE		*fp_in, *fp_out;
int		err_code;
FITS		fits;
int		offset;
char		in_filename[80];
char		out_filename[80];
char		filehead[80];
char		buffer[80];
char		*token;
double		cal;
char		unit[16];


/*	入力ファイルのオープン	*/


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
	filehead[strlen(buffer)-5]='\0';
	sprintf(in_filename,"%s",buffer);
	fp_in=fopen(in_filename,"r");
	if(fp_in==NULL){
		fprintf(stderr,"Can't open %s !\n",in_filename);
		exit(1);
	}
}

cal=1.0;
fprintf(stderr,"Scaling factor (%lf): ",cal);
fscanf(stdin,"%lf",&cal);

fprintf(stderr,"Reading %s\n",in_filename);
err_code=read_Fits(&fits, fp_in);
if(err_code!=0){
	fprintf(stderr,"Error in reading the FITS header. Error code=%d\n",err_code);
	exit(999);
}
fclose(fp_in);

fprintf(stderr,"Unit (%s): ",fits.head.bunit);
fscanf(stdin,"%s",buffer);
strncpy(unit,buffer,16);
sprintf(fits.head.bunit,"\'%s \'",unit);
    //fprintf(stderr,"%s\n",fits.head.bunit);


fprintf(stderr,"Scaling %s\n",in_filename);
err_code=scaling_Fits(&fits,cal);


/*fp_out=stdout;*/
sprintf(out_filename,"%s.sc.FITS",filehead);
fp_out=fopen(out_filename,"w");
if(fp_out==NULL){
	fprintf(stderr,"Can't open %s !\n",out_filename);
	exit(2);
}

fprintf(stderr,"Writing to %s\n",out_filename);
err_code=output_Fits(&fits, fp_out);
if(err_code!=0){
	fprintf(stderr,"Error in outputing the FITS data. Error code=%d\n",err_code);
	exit(999);
}
fclose(fp_out);



}	/*main*/



int	scaling_Fits(fits,cal)
FITS	*fits;
double	cal;
{
int	ii, jj, data_num;

data_num=1;
for(jj=1;jj<=(*fits).head.naxis[0];jj++){
	data_num=data_num*(*fits).head.naxis[jj];
}

for(ii=0;ii<data_num;ii++){
	if(Blank((*fits).data[ii])==(-1))
		(*fits).data[ii] = DEFAULT_BLANK_VALUE;
	else
		(*fits).data[ii] = (*fits).data[ii]*cal;
}

return(0);
}




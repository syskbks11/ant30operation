/*
1993/9/1	Tomoharu OKA	:	Coding 開始
1993/9/2	Tomoharu OKA	:	Ver.1.00 完成
1994/9/7	Tomoharu OKA	:	Bug fix for pixel inner structure
1998/3/3	Tomo OKA	:	FITS intensity calibration
1998/4/30	Tomo OKA	:	FITS intensity clipping
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
double		clip;


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

fprintf(stderr,"Reading %s\n",in_filename);
err_code=read_Fits(&fits, fp_in);
if(err_code!=0){
	fprintf(stderr,"Error in reading the FITS header. Error code=%d\n",err_code);
	exit(999);
}
fclose(fp_in);

fprintf(stderr,"Clipping Intensity (%lf< clip < %lf): ",fits.head.datamin,fits.head.datamax);
fscanf(stdin,"%lf",&clip);


fprintf(stderr,"Clipping %s\n",in_filename);
err_code=clipping_Fits(&fits,clip);



/*fp_out=stdout;*/
sprintf(out_filename,"%s.clp.FITS",filehead);
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



int	clipping_Fits(fits,clip)
FITS	*fits;
double	clip;
{
int	ii, jj, data_num;

for(ii=0;ii<(*fits).head.data_num;ii++){
	if((*fits).data[ii]<clip)
		(*fits).data[ii] = DEFAULT_BLANK_VALUE;
}

return(0);
}




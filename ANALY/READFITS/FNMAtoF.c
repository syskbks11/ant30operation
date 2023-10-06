/*
1993/9/1	Tomoharu OKA	:	Coding ŠJŽn
1993/9/2	Tomoharu OKA	:	Ver.1.00 Š®¬
1994/9/7	Tomoharu OKA	:	Bug fix for pixel inner structure
1995/6/20	Tomoharu OKA	:	FITS -> MATRIX
1997/12/02	Tomo OKA	:	45m FITS CUBE (seq) -> FITS CUBE (vel)
1998/4/23	Tomo OKA	:	NMA FITS CUBE (FREQ) -> FITS CUBE (vel)
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
RANGE		v;
char		in_filename[80];
char		out_filename[80];
char		filehead[80];
char		buffer[80];
char		*token;
char		command[80];
double		freq;





if(argc<2){
	fprintf(stderr,"Usage: FNMAtoF (filename)\n");
	exit(1);
}


strcpy(buffer,argv[1]);
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

fprintf(stderr,"Rest frequency [GHz]: ");

fscanf(stdin,"%s",buffer);
token=strtok(buffer,", \t\n");
if(token[0]!=NULL)	freq=atof(token)*1.0e9;


err_code=read_Fits(&fits, fp_in);
if(err_code!=0){
	fprintf(stderr,"Error in reading the FITS header. Error code=%d\n",err_code);
	exit(999);
}
fclose(fp_in);

sprintf(command,"cp %s %s.bak\n",in_filename,in_filename);
system(command);


err_code=correct_Fits(&fits, freq);


strcpy(out_filename,in_filename);
fp_out=fopen(out_filename,"w");
if(fp_out==NULL){
	fprintf(stderr,"Can't open %s !\n",out_filename);
	exit(2);
}
err_code=output_Fits(&fits, fp_out);
if(err_code!=0){
	fprintf(stderr,"Error in outputing the Matrix data. Error code=%d\n",err_code);
	exit(999);
}
fclose(fp_out);

}	/*main*/



int	correct_Fits(fits,rest_freq)
FITS	*fits;
double	rest_freq;
{

(*fits).head.naxis[0]=3;

(*fits).head.crval[3]=(rest_freq-(*fits).head.crval[3])/rest_freq*LIGHT_SPEED;
(*fits).head.cdelt[3]=(-1.0)*(*fits).head.cdelt[3]/rest_freq*LIGHT_SPEED;
(*fits).head.crpix[3]=1.0;
(*fits).head.ctype[3]=malloc(10);
strncpy((*fits).head.ctype[3],"\'VEL-LSR\'",9);

return(0);
}




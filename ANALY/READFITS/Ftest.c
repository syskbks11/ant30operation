#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <profhead.h>
#include <starlib.h>
#include <fitshead.h>


main(argc,argv)
int	argc;
char	*argv[];
{
FILE	*fp_in, *fp_out;
int	err_code;
FITS	fits;
char	in_filename[80];
char	filehead[80];
char	buffer[80];
char	*token;
long	ii;

/*	入力ファイルのオープン	*/

if(argc<2){
	fprintf(stderr,"Usage: Fxy (filename)\n");
	exit(1);
}

/*fp_in=stdin;*/
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


err_code=read_Fits(&fits, fp_in);
if(err_code!=0){
	fprintf(stderr,
	"Error in reading %s. Error code=%d\n"
	,in_filename,err_code);	
	exit(999);
}
fclose(fp_in);

for(ii=0;ii<fits.head.data_num;ii++){
	fits.data[ii]=1.0;
}

fp_out=stdout;
err_code=output_Fits(&fits, fp_out);
if(err_code!=0){
	fprintf(stderr,
	"Error in writing a FITS file.  Error code=%d\n"
	,err_code);	
	exit(999);
}



}	/*main*/





